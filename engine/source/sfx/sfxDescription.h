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
#endif

class SFXDescription : public SimDataBlock
{
   typedef SimDataBlock Parent;

public:

   /// The 0 to 1 volume scale.
   F32 mVolume;
   S32  mVolumeChannel;
   /// The pitch scale.
   F32 mPitch;

   /// If true the sound will loop.
   bool mIsLooping;
   bool mIsStreaming;
   /// If true the sound will be 3D positional.
   bool mIs3D;

   F32  mReferenceDistance;
   F32  mRollOffFactor;

   /// If true the sound uses custom reverb properties.
   bool mUseReverb;

   /// The distance from the emitter at which the
      /// sound volume becomes zero.
      ///
      /// This is only valid for 3D sounds.
   F32 mMaxDistance;

   /// The angle in degrees of the inner part of
   /// the cone.  It must be within 0 to 360.
   ///
   /// This is only valid for 3D sounds.
   U32 mConeInsideAngle;

   /// The angle in degrees of the outer part of
   /// the cone.  It must be greater than mConeInsideAngle
   /// and less than to 360.
   ///
   /// This is only valid for 3D sounds.
   U32 mConeOutsideAngle;

   /// The volume scalar for on/beyond the outside angle.
   ///
   /// This is only valid for 3D sounds.
   F32 mConeOutsideVolume;

   Point3F mConeVector;

   SFXSoundReverbProperties mReverb;

   // environment info
   F32 mEnvironmentLevel;

   SFXDescription();
   DECLARE_CONOBJECT(SFXDescription);
   static void initPersistFields();

   // SimDataBlock.
   virtual bool onAdd();
   virtual void packData(BitStream* stream);
   virtual void unpackData(BitStream* stream);

};


#endif // _SFXDESCRIPTION_H_