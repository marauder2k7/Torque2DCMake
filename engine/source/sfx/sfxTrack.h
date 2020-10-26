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
#ifndef _SFXTRACK_H_
#define _SFXTRACK_H_

#ifdef _SIMDATABLOCK_H_
#include "sim/simDatablock.h"
#endif
#ifndef _CONSOLETYPES_H_
#include "console/consoleTypes.h"
#endif


class SFXDescription;


/// A datablock that describes sound data for playback.
class SFXTrack : public SimDataBlock
{
public:

   typedef SimDataBlock Parent;


protected:

   /// The description which controls playback settings.
   SFXDescription *mDescription;

   /// Overload this to disable direct instantiation of this class via script 'new'.
   virtual bool processArguments(S32 argc, const char **argv);

public:
   DECLARE_CONOBJECT(SFXTrack);
   ///
   SFXTrack();

   ///
   SFXTrack(SFXDescription* description);

   /// Returns the description object for this sound profile.
   SFXDescription* getDescription() const { return mDescription; }

   // SimDataBlock.
   virtual void packData(BitStream* stream);
   virtual void unpackData(BitStream* stream);
   virtual bool preload(bool server, char& errorStr);
   virtual bool onAdd();
   virtual void inspectPostApply();

   static void initPersistFields();

   

public:
   SFXTrack(const SFXTrack& other);
};

#endif // !_SFXTRACK_H_
