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

#ifndef _SFXDESCRIPTION_H_
#define _SFXDESCRIPTION_H_

#ifndef _CONSOLETYPES_H_
#include "console/consoleTypes.h"
#endif
#ifndef _SIMDATABLOCK_H_
#include "sim/simDatablock.h"
#endif

#ifndef _SFXCOMMON_H_
#include "sfx/sfxCommon.h"
#endif // !_SFXCOMMON_H_


//--------------------------------------------------------------------------

class SFXDescription : public SimDataBlock
{
   typedef SimDataBlock Parent;

public:

   F32  mVolume;    // 0-1    1=loudest volume
   S32  mVolumeChannel;
   bool mIsLooping;
   bool mIsStreaming;

   bool mIs3D;
   F32  mReferenceDistance;
   F32  mRollOffFactor;
   F32  mMaxDistance;
   S32  mConeInsideAngle;
   S32  mConeOutsideAngle;
   F32  mConeOutsideVolume;
   Point3F mConeVector;

   // environment info
   F32 mEnvironmentLevel;

   SFXDescription();
   // SimDataBlock.
   virtual bool onAdd();

   static void initPersistFields();

   DECLARE_CONOBJECT(SFXDescription);
};

#endif  // _H_AUDIODATABLOCK_
