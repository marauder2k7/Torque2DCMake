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

#ifndef _SFXENVIRONMENT_H_
#define _SFXENVIRONMENT_H_

#ifndef _SIMDATABLOCK_H_
#include "sim/simDatablock.h"
#endif
#ifndef _CONSOLETYPES_H_
#include "console/consoleTypes.h"
#endif
#ifndef _SFXCOMMON_H_
#include "sfx/sfxCommon.h"
#endif


/// A datablock that defines a reverb environment.
class SFXEnvironment : public SimDataBlock
{
public:

   typedef SimDataBlock Parent;

protected:

   /// The reverb properties.
   SFXReverbProperties mReverb;

public:

   SFXEnvironment();

   ///
   void validate();

   static void initPersistFields();

   virtual bool onAdd();
   virtual bool preload(bool server, char* errorStr);
   virtual void packData(BitStream* stream);
   virtual void unpackData(BitStream* stream);
   virtual void inspectPostApply();

   /// @return The reverb properties of the sound environment.
   const SFXReverbProperties& getReverb() const { return mReverb; }

   DECLARE_CONOBJECT(SFXEnvironment);
};

class SFXSampleEnvironment : public SimDataBlock
{
   typedef SimDataBlock Parent;

public:

   S32      mDirect;
   S32      mDirectHF;
   S32      mRoom;
   S32      mRoomHF;
   F32      mObstruction;
   F32      mObstructionLFRatio;
   F32      mOcclusion;
   F32      mOcclusionLFRatio;
   F32      mOcclusionRoomRatio;
   F32      mRoomRolloff;
   F32      mAirAbsorption;
   S32      mOutsideVolumeHF;
   S32      mFlags;

   SFXSampleEnvironment();
   static void initPersistFields();

   void packData(BitStream* stream);
   void unpackData(BitStream* stream);

   DECLARE_CONOBJECT(SFXSampleEnvironment);
};

#endif // _SFXENVIRONMENT_H_