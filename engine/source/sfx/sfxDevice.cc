#include "collection/vector.h"
#include "console/console.h"
#include "console/consoleTypes.h"
#include "game/gameConnection.h"
#include "io/fileStream.h"
#include "sfx/sfxBuffer.h"
#include "sfx/sfxDescription.h"
#include "sfx/sfxDevice.h"
#include "sfx/sfxStreamSourceFactory.h"
#include "sfx/sfxCommon.h"
#include "sfx/sfxEnvironment.h"
#include "sfx/sfxProvider.h"

//-----------------------------------------------------
// INITIALIZATION
//-----------------------------------------------------

void SFXDevice::init()
{
   Con::printSeparator();
   Con::printf("OpenAL Initialization:");
   SFXProvider* dumPro = new SFXProvider;
   dumPro->init();
}

//-----------------------------------------------------
// DEVICE CREATION
//-----------------------------------------------------

SFXDevice::SFXDevice(const OPENALFNTABLE & openal)
   : mOpenAL(openal)
{
   Con::printf("SFXDevice: creating Device");
   mDevice = mOpenAL.alcOpenDevice((const ALCchar*)NULL);

   if (mDevice == (ALCvoid*)NULL)
   {
      Con::printf("Failed to create SFXDevice");
      return;
   }
   Con::printf("SFXDevice: creating Context");
   mContext = mOpenAL.alcCreateContext(mDevice, NULL);
   if (mContext == (ALCvoid*)NULL)
   {
      Con::printf("Failed to create context for SFXDevice");
      return;
   }
   Con::printf("SFXDevice: making Context current");
   mOpenAL.alcMakeContextCurrent(mContext);

   //Find out max sources supported and assign it.
   maxSources = GetMaxNumSources();

   mOpenAL.alEnable(AL_DISTANCE_MODEL);
   mOpenAL.alDistanceModel(AL_INVERSE_DISTANCE);
   mOpenAL.alDopplerFactor(1.0f);

   mOpenAL.alListenerf(AL_GAIN, 1.0f);

   F32 listenerPos[] = { 0.f, 0.f, 0.f };
   F32 listenerVel[] = { 0.f, 0.f, 0.f };
   F32 listenerOrientation[] = { 0.f, 0.f, -1.0f, 0.f, 1.f, 0.f };

   mOpenAL.alListenerfv(AL_POSITION, listenerPos);
   mOpenAL.alListenerfv(AL_VELOCITY, listenerVel);
   mOpenAL.alListenerfv(AL_ORIENTATION, listenerOrientation);
   Con::printf("SFXDevice created:");
}

SFXDevice::~SFXDevice()
{
   
   
}

unsigned int SFXDevice::GetMaxNumSources()
{
   ALuint uiSources[256];
   U32 iSourceCount = 0;

   // Clear AL Error Code
   mOpenAL.alGetError();

   // Generate up to 256 Sources, checking for any errors
   for (iSourceCount = 0; iSourceCount < 256; iSourceCount++)
   {
      mOpenAL.alGenSources(1, &uiSources[iSourceCount]);
      if (mOpenAL.alGetError() != AL_NO_ERROR)
         break;
   }

   // Release the Sources
   mOpenAL.alDeleteSources(iSourceCount, uiSources);
   if (mOpenAL.alGetError() != AL_NO_ERROR)
   {
      for (U32 i = 0; i < 256; i++)
      {
         mOpenAL.alDeleteSources(1, &uiSources[i]);
      }
   }

   Con::printf("SFXDevice supports: %i Sources", iSourceCount);

   return iSourceCount;
}