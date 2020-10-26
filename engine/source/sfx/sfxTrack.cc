//-----------------------------------------------------------------------------
// Copyright (c) 2012 GarageGames, LLC
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

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX for MIT Licensed Open Source version of Torque 3D from GarageGames
// Copyright (C) 2015 Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
#include "sfx/sfxTrack.h"
#include "sfx/sfxTypes.h"
#include "sfx/sfxDescription.h"
//#include "sfx/sfxSystem.h"
#include "io/bitStream.h"

IMPLEMENT_CO_DATABLOCK_V1(SFXTrack);

//-----------------------------------------------------------------------------

SFXTrack::SFXTrack()
   : mDescription(NULL)
{
}

//-----------------------------------------------------------------------------

SFXTrack::SFXTrack(SFXDescription* description)
   : mDescription(description)
{
}

SFXTrack::SFXTrack(const SFXTrack& other) : SimDataBlock(other)
{
   mDescription = other.mDescription;
}
//-----------------------------------------------------------------------------

void SFXTrack::initPersistFields()
{
   addGroup("Sound");

   addField("description", TypeSFXDescriptionName, Offset(mDescription, SFXTrack),
      "Playback setup description for this track.\n\n"
      "If unassigned, the description named \"AudioEffects\" will automatically be assigned to the track.  If this description "
      "is not defined, track creation will fail.");

   endGroup("Sound");

   Parent::initPersistFields();
}

//-----------------------------------------------------------------------------

bool SFXTrack::processArguments(S32 argc, const char **argv)
{
   if (typeid(*this) == typeid(SFXTrack))
   {
      Con::errorf(ConsoleLogEntry::Script, "SFXTrack is an abstract base class that cannot be instantiated directly!");
      return false;
   }

   return Parent::processArguments(argc, argv);
}

//-----------------------------------------------------------------------------

void SFXTrack::packData(BitStream* stream)
{
   Parent::packData(stream);

   sfxWrite(stream, mDescription);

}

//-----------------------------------------------------------------------------

void SFXTrack::unpackData(BitStream* stream)
{
   Parent::unpackData(stream);

   sfxRead(stream, &mDescription);

}

//-----------------------------------------------------------------------------

bool SFXTrack::preload(bool server, char& errorStr)
{
   if (!Parent::preload(server, (char*)errorStr))
      return false;

   if (!server)
   {
      if (!sfxResolve(&mDescription, errorStr))
         return false;
   }

   return true;
}

//-----------------------------------------------------------------------------

bool SFXTrack::onAdd()
{
   if (!Parent::onAdd())
      return false;

   // If we have no SFXDescription, try to grab a default.

   if (!mDescription)
   {
      if (!Sim::findObject("AudioEffects", mDescription) && Sim::getSFXDescriptionSet()->size() > 0)
         mDescription = dynamic_cast<SFXDescription*>(Sim::getSFXDescriptionSet()->at(0));

      if (!mDescription)
      {
         Con::errorf(
            "SFXTrack(%s)::onAdd: The profile is missing a description!",
            getName());
         return false;
      }
   }

   Sim::getSFXTrackSet()->addObject(this);

   return true;
}

//-----------------------------------------------------------------------------

void SFXTrack::inspectPostApply()
{
   Parent::inspectPostApply();

   //if (SFX)
    //  SFX->notifyTrackChanged(this);
}
