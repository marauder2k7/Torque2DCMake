#ifndef _SFXSTREAMSOURCE_H_
#define _SFXSTREAMSOURCE_H_

#include "sfx/sfxDevice.h"

#include "sfx/sfxDescription.h"

#include "sfx/sfxEnvironment.h"

#ifndef _RESMANAGER_H_
#include "io/resource/resourceManager.h"
#endif

#define NUMBUFFERS 16

class SFXStreamSource
{
public:
   //need this because subclasses are deleted through base pointer
   virtual ~SFXStreamSource() {}
   virtual bool initStream() = 0;
   virtual bool updateBuffers() = 0;
   virtual void freeStream() = 0;
   virtual F32 getElapsedTime() = 0;
   virtual F32 getTotalTime() = 0;
   //void clear();

   AUDIOHANDLE             mHandle;
   ALuint				      mSource;

   SFXDescription          mDescription;
   SFXSampleEnvironment    *mEnvironment;

   Point3F                 mPosition;
   Point3F                 mDirection;
   F32                     mPitch;
   F32                     mScore;
   U32                     mCullTime;

   bool					      bFinishedPlaying;
   bool					      bIsValid;

#ifdef TORQUE_OS_LINUX
   void checkPosition();
#endif

protected:
   const char* mFilename;
};

#endif // _AUDIOSTREAMSOURCE_H_