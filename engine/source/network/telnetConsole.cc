//-----------------------------------------------------------------------------
// Copyright (c) 2013 GarageGames, LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------

#include "platform/platform.h"
#include "platform/event.h"
#include "network/telnetConsole.h"
#include "game/gameInterface.h"

#include "telnetConsole_ScriptBinding.h"

TelnetConsole *TelConsole = NULL;

void TelnetConsole::create()
{
   TelConsole = new TelnetConsole;
}

void TelnetConsole::destroy()
{
   delete TelConsole;
   TelConsole = NULL;
}

static void telnetCallback(ConsoleLogEntry::Level level, const char *consoleLine)
{
   if (TelConsole)
      TelConsole->processConsoleLine(consoleLine);
}

TelnetConsole::TelnetConsole()
{
   Con::addConsumer(telnetCallback);

   mAcceptSocket = NetSocket::INVALID;
   mAcceptPort = -1;
   mClientList = NULL;
   mRemoteEchoEnabled = false;
}

TelnetConsole::~TelnetConsole()
{
   Con::removeConsumer(telnetCallback);
   if(mAcceptSocket != NetSocket::INVALID)
      Net::closeSocket(mAcceptSocket);
   TelnetClient *walk = mClientList, *temp;
   while(walk)
   {
      temp = walk->nextClient;
      if(walk->socket != NetSocket::INVALID)
         Net::closeSocket(walk->socket);
      delete walk;
      walk = temp;
   }
}

void TelnetConsole::setTelnetParameters(S32 port, const char *telnetPassword, const char *listenPassword, bool remoteEcho)
{
   if(port == mAcceptPort)
      return;

   mRemoteEchoEnabled = remoteEcho;

   if(mAcceptSocket != NetSocket::INVALID)
   {
      Net::closeSocket(mAcceptSocket);
      mAcceptSocket = NetSocket::INVALID;
   }
   mAcceptPort = port;
   if(mAcceptPort != -1 && mAcceptPort != 0)
   {
     NetAddress address;
     Net::getIdealListenAddress(&address);
     address.port = mAcceptPort;

      mAcceptSocket = Net::openSocket();
      Net::bindAddress(address, mAcceptSocket);
      Net::listen(mAcceptSocket, 4);

      Net::setBlocking(mAcceptSocket, false);
   }
   dStrncpy(mTelnetPassword, telnetPassword, PasswordMaxLength);
   dStrncpy(mListenPassword, listenPassword, PasswordMaxLength);
}

void TelnetConsole::processConsoleLine(const char *consoleLine)
{
   if (mClientList==NULL) return;  // just escape early.  don't even do another step...

   // ok, spew this line out to all our subscribers...
   S32 len = dStrlen(consoleLine)+1;
   for(TelnetClient *walk = mClientList; walk; walk = walk->nextClient)
   {
      if(walk->state == FullAccessConnected || walk->state == ReadOnlyConnected)
      {
            Net::send(walk->socket, (const unsigned char*)consoleLine, len);
            Net::send(walk->socket, (const unsigned char*)"\r\n", 2);
         }
      }
   }

void TelnetConsole::process()
{
   NetAddress address;

   if(mAcceptSocket != NetSocket::INVALID)
   {
      // ok, see if we have any new connections:
      NetSocket newConnection;
      newConnection = Net::accept(mAcceptSocket, &address);

      if(newConnection != NetSocket::INVALID)
      {
         char buffer[256];
         Net::addressToString(&address, buffer);
         Con::printf("Telnet connection from %s", buffer);

         TelnetClient *cl = new TelnetClient;
         cl->socket = newConnection;
         cl->curPos = 0;
#if defined(TORQUE_SHIPPING) && defined(TORQUE_DISABLE_TELNET_CONSOLE_PASSWORD)
         // disable the password in a ship build? WTF.  lets make an error:
         PleaseMakeSureYouKnowWhatYouAreDoingAndCommentOutThisLineIfSo.
#elif !defined(TORQUE_SHIPPING) && defined(TORQUE_DISABLE_TELNET_CONSOLE_PASSWORD)
         cl->state = FullAccessConnected;
#else
         cl->state = PasswordTryOne;
#endif

         Net::setBlocking(newConnection, false);

         const char *prompt = Con::getVariable("Con::Prompt");
         char connectMessage[1024];
         dSprintf(connectMessage, sizeof(connectMessage),
            "Torque Telnet Remote Console\r\n\r\n%s",
            cl->state == FullAccessConnected ? prompt : "Enter Password:");

            Net::send(cl->socket, (const unsigned char*)connectMessage, dStrlen(connectMessage)+1);
         cl->nextClient = mClientList;
         mClientList = cl;
      }
   }

   char recvBuf[256];
   char reply[1024];

   // see if we have any input to process...

   for(TelnetClient *client = mClientList; client; client = client->nextClient)
   {
      S32 numBytes;
      Net::Error err = Net::recv(client->socket, (unsigned char*)recvBuf, sizeof(recvBuf), &numBytes);

      if((err != Net::NoError && err != Net::WouldBlock) || numBytes == 0)
      {
            Net::closeSocket(client->socket);
            client->socket = NetSocket::INVALID;
         continue;
      }

      S32 replyPos = 0;
      for(S32 i = 0; i < numBytes;i++)
      {
         if(recvBuf[i] == '\r')
            continue;
         // execute the current command

         if(recvBuf[i] == '\n')
         {
            reply[replyPos++] = '\r';
            reply[replyPos++] = '\n';

            client->curLine[client->curPos] = 0;
            client->curPos = 0;

            if(client->state == FullAccessConnected)
            {
                  Net::send(client->socket, (const unsigned char*)reply, replyPos);
               replyPos = 0;

               dStrcpy(mPostEvent.data, client->curLine);
               mPostEvent.size = dStrlen(client->curLine) + 1;
               Game->postEvent(mPostEvent);

               // note - send prompt next
               const char *prompt = Con::getVariable("Con::Prompt");
                  Net::send(client->socket, (const unsigned char*)prompt, dStrlen(prompt));
            }
            else if(client->state == ReadOnlyConnected)
            {
                  Net::send(client->socket, (const unsigned char*)reply, replyPos);
               replyPos = 0;
            }
            else
            {
               client->state++;
               if(!dStrncmp(client->curLine, mTelnetPassword, PasswordMaxLength))
               {
                     Net::send(client->socket, (const unsigned char*)reply, replyPos);
                  replyPos = 0;

                  // send prompt
                  const char *prompt = Con::getVariable("Con::Prompt");
                     Net::send(client->socket, (const unsigned char*)prompt, dStrlen(prompt));
                  client->state = FullAccessConnected;
               }
               else if(!dStrncmp(client->curLine, mListenPassword, PasswordMaxLength))
               {
                     Net::send(client->socket, (const unsigned char*)reply, replyPos);
                  replyPos = 0;

                  // send prompt
                  const char *listenConnected = "Connected.\r\n";
                     Net::send(client->socket, (const unsigned char*)listenConnected, dStrlen(listenConnected));
                  client->state = ReadOnlyConnected;
               }
               else
               {
                  const char *sendStr;
                  if(client->state == DisconnectThisDude)
                     sendStr = "Too many tries... cya.";
                  else
                     sendStr = "Nope... try agian.\r\nEnter Password:";
                     Net::send(client->socket, (const unsigned char*)sendStr, dStrlen(sendStr));
                  if(client->state == DisconnectThisDude)
                  {
                     Net::closeSocket(client->socket);
                     client->socket = NetSocket::INVALID;
                  }
               }
            }
         }
         else if(recvBuf[i] == '\b')
         {
            // pull the old backspace manuever...
            if(client->curPos > 0)
            {
               client->curPos--;
               if(client->state == FullAccessConnected)
               {
                  reply[replyPos++] = '\b';
                  reply[replyPos++] = ' ';
                  reply[replyPos++] = '\b';
               }
            }
         }
         else if(client->curPos < Con::MaxLineLength-1)
         {
            client->curLine[client->curPos++] = recvBuf[i];
            // don't echo password chars...
            if(client->state == FullAccessConnected)
               reply[replyPos++] = recvBuf[i];
         }
      }

      // Echo the character back to the user, unless the remote echo
      // is disabled (by default)
      if(replyPos && mRemoteEchoEnabled)
         Net::send(client->socket, (const unsigned char*)reply, replyPos);
   }

   TelnetClient ** walk = &mClientList;
   TelnetClient *cl;
   while((cl = *walk) != NULL)
   {
      if(cl->socket == NetSocket::INVALID)
      {
         *walk = cl->nextClient;
         delete cl;
      }
      else
         walk = &cl->nextClient;
   }
}
