#ifndef _SFXDEVICE_H_
#define _SFXDEVICE_H_

#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif

#ifndef _MMATH_H_
#include "math/mMath.h"
#endif

#ifndef _OPENALFNTABLE
#include "sfx/LoadOAL.h"
#endif

class SFXProvider;

enum SFXCaps
{
   SFXCapture = 0,
   SFXEFX,
   SFXOffset,
   SFXLinearDistance,
   SFXExponentDistance,
   SFXEAX2,
   SFXEAX3,
   SFXEAX4,
   SFXEAX5,
   SFXEAXRAM
};

class SFXDevice
{

public:
   SFXDevice(SFXProvider* provider,const OPENALFNTABLE &openal);
   ~SFXDevice();

protected:

   static SFXDevice* smDevice;

   SFXProvider*   mProvider;

   OPENALFNTABLE  mOpenAL;

   ALCcontext     *mContext;

   ALCdevice      *mDevice;

   ///--- device identifier
   const char     *mDeviceName;
   S32            mMajorVersion;
   S32            mMinorVersion;
   S32            mCaps;
   U32            maxSources;
   ///---


public:
   static void init();
   static void shutdown();

   unsigned int GetMaxNumSources();

   // sound property description
   struct DefDescription
   {
      F32  mVolume;    // 0-1    1=loudest volume
      S32  mVolumeChannel;
      bool mIsLooping;
      bool mIsStreaming;

      bool mIs3D;
      F32  mReferenceDistance;
      F32  mMaxDistance;
      U32  mConeInsideAngle;
      U32  mConeOutsideAngle;
      F32  mConeOutsideVolume;
      Point3F mConeVector;

      // environment info
      F32 mEnvironmentLevel;
   };

};


#endif // !_SFXDEVICE_H_
