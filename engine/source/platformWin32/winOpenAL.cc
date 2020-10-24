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

#include "platformWin32/platformWin32.h"
#include "console/console.h"

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>

//#include <al/altypes.h>
//#include <al/alctypes.h>
#define INITGUID
//#include <al/eaxtypes.h>


// Define the OpenAL and Extension Stub functions
#define AL_FUNCTION(fn_return, fn_name, fn_args, fn_value) fn_return stub_##fn_name fn_args{ fn_value }
//#include <al/al_func.h>
//#include <al/alc_func.h>
//#include <al/eax_func.h>
#undef AL_FUNCTION


// Declare the OpenAL and Extension Function pointers
// And initialize them to the stub functions
#define AL_FUNCTION(fn_return,fn_name,fn_args, fn_value) fn_return (*fn_name)fn_args = stub_##fn_name;
//#include <al/al_func.h>
//#include <al/alc_func.h>
//#include <al/eax_func.h>
#undef AL_FUNCTION

// Declarations for the "emulated" functions (al functions that don't 
// exist in the loki openal implementation)
ALboolean emu_alGetBoolean(ALenum param);
ALint emu_alGetInteger(ALenum param);
ALfloat emu_alGetFloat(ALenum param);
ALdouble emu_alGetDouble(ALenum param);
void emu_alListeneri(ALenum param, ALint value);
void emu_alGetListener3f(ALenum pname, ALfloat *v1, ALfloat *v2, ALfloat *v3);
ALCdevice* emu_alcGetContextsDevice(ALCcontext *context);

static void *dlHandle = NULL;
static char* dlError = "no error";

/*!   Get an "emulated" function address and bind it to the function pointer
*/
static bool bindEmulatedFunction(void *&fnAddress, const char *name)
{
   fnAddress = NULL;

   if (dStrcmp(name, "alGetBoolean") == 0)
      fnAddress = (void*)&emu_alGetBoolean;
   else if (dStrcmp(name, "alGetInteger") == 0)
      fnAddress = (void*)&emu_alGetInteger;
   else if (dStrcmp(name, "alGetFloat") == 0)
      fnAddress = (void*)&emu_alGetFloat;
   else if (dStrcmp(name, "alGetDouble") == 0)
      fnAddress = (void*)&emu_alGetDouble;
   else if (dStrcmp(name, "alListeneri") == 0)
      fnAddress = (void*)&emu_alListeneri;
   else if (dStrcmp(name, "alGetListener3f") == 0)
      fnAddress = (void*)&emu_alGetListener3f;
   else if (dStrcmp(name, "alcGetContextsDevice") == 0)
      fnAddress = (void*)&emu_alcGetContextsDevice;

   return fnAddress != NULL;
}

/*!   Get a function address from the OpenAL DLL and bind it to the
*     function pointer
*/
static bool bindFunction(void *&fnAddress, const char *name)
{
   // JMQ: MinGW gcc 3.2 needs the cast to void*
   fnAddress = GetProcAddress(winState.hinstOpenAL, name);
   if (!fnAddress)
      if (bindEmulatedFunction(fnAddress, name))
         Con::warnf(ConsoleLogEntry::General, " Missing OpenAL function '%s', using emulated function", name);
      else
      Con::errorf(ConsoleLogEntry::General, " Missing OpenAL function '%s'", name);
   return (fnAddress != NULL);
}

/*!   Get a function address for an OpenAL extension function and bind it
*     to it's function pointer
*/
static bool bindExtensionFunction(void *&fnAddress, const char *name)
{
   fnAddress = alGetProcAddress( name );
   if( !fnAddress )
      Con::errorf(ConsoleLogEntry::General, " Missing OpenAL Extension function '%s'", name);
   return (fnAddress != NULL);
}

/*!   Bind the functions in the OpenAL DLL to the al interface functions
*/
static bool bindOpenALFunctions()
{
   bool result = true;
#define AL_FUNCTION(fn_return, fn_name, fn_args, fn_value) result &= bindFunction( *(void**)&fn_name, #fn_name);
   //#include <al/al_func.h>
   //#include <al/alc_func.h>
#undef AL_FUNCTION
   return result;
}

/*!   Bind the stub functions to the al interface functions
*/
static void unbindOpenALFunctions()
{
#define AL_FUNCTION(fn_return, fn_name, fn_args, fn_value) fn_name = stub_##fn_name;
   //#include <al/al_func.h>
   //#include <al/alc_func.h>
   //#include <al/eax_func.h>
#undef AL_FUNCTION
}

/*!   Bind the EAX Extension functions to the EAX interface functions
*/
static bool bindEAXFunctions()
{
   bool result = true;
#define AL_FUNCTION(fn_return, fn_name, fn_args, fn_value) result &= bindExtensionFunction( *(void**)&fn_name, #fn_name);
   //#include <al/eax_func.h>
#undef AL_FUNCTION
   return result;
}

// Definitions for the emulated functions
ALboolean emu_alGetBoolean(ALenum param)
{
   ALboolean alboolean;
   alGetBooleanv(param, &alboolean);
   return alboolean;
}

ALint emu_alGetInteger(ALenum param)
{
   ALint alint;
   alGetIntegerv(param, &alint);
   return alint;
}

ALfloat emu_alGetFloat(ALenum param)
{
   ALfloat alfloat;
   alGetFloatv(param, &alfloat);
   return alfloat;
}

ALdouble emu_alGetDouble(ALenum param)
{
   ALdouble aldouble;
   alGetDoublev(param, &aldouble);
   return aldouble;
}

void emu_alGetListener3f(ALenum pname, ALfloat *v0, ALfloat *v1, ALfloat *v2)
{
   ALfloat ptArray[10];
   ptArray[0] = *v0;
   ptArray[1] = *v1;
   ptArray[2] = *v2;
   alGetListenerfv(pname, ptArray);
   *v0 = ptArray[0];
   *v1 = ptArray[1];
   *v2 = ptArray[2];
}

void emu_alListeneri(ALenum param, ALint value)
{
   alListenerf(param, static_cast<ALfloat>(value));
}

ALCdevice* emu_alcGetContextsDevice(ALCcontext *context)
{
   // this function isn't emulated
   AssertFatal(false, "alcGetContextsDevice is not available");
   return NULL;
}

namespace Audio
{
   /*!   Shutdown and Unload the OpenAL DLL
   */
   void OpenALDLLShutdown()
   {
      if (winState.hinstOpenAL != NULL)
      {
         FreeLibrary(winState.hinstOpenAL);
      }
      winState.hinstOpenAL = NULL;

      unbindOpenALFunctions();
   }

   /*!   Dynamically Loads the OpenAL DLL if present and binds all the functions.
   *     If there is no DLL or an unexpected error occurs binding functions the
   *     stub functions are automatically bound.
   */
   bool OpenALDLLInit()
   {
      OpenALDLLShutdown();

      winState.hinstOpenAL = LoadLibraryA("OpenAL32.dll");

      if (winState.hinstOpenAL != NULL)
      {
         Con::printf("OpenAL library loaded.");

         // if the DLL loaded bind the OpenAL function pointers
         if (bindOpenALFunctions())
         {
            // if EAX is available bind it's function pointers
            if (alIsExtensionPresent("EAX"))
               bindEAXFunctions();

            Con::printf("OpenAL functions binded.");
            return(true);
         }

         // an error occured, shutdown
         OpenALDLLShutdown();
      }
      else
      {
         Con::errorf(ConsoleLogEntry::General, " Error loading OpenAL Library:");
      }
      return(false);
   }

} // end namespace Audio