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

SFXDevice* SFXDevice::smDevice = NULL;

void SFXDevice::init()
{
   Con::printSeparator();
   Con::printf("OpenAL Initialization:");
   SFXProvider* dumPro = new SFXProvider;
   dumPro->init();
}

void SFXDevice::shutdown()
{
   delete smDevice;
}

//-----------------------------------------------------
// DEVICE CREATION
//-----------------------------------------------------

SFXDevice::SFXDevice(SFXProvider* provider, const OPENALFNTABLE & openal)
   :  mProvider(provider),
      mOpenAL(openal)
{
   smDevice = this;

   Con::printf("SFXDevice: creating Device");
   mDevice = mOpenAL.alcOpenDevice((const ALCchar*)NULL);

   if (mDevice == (ALCvoid*)NULL)
   {
      Con::printf("Failed to create SFXDevice");
      return;
   }

   mContext = mOpenAL.alcCreateContext(mDevice, NULL);
   if (mContext == (ALCvoid*)NULL)
   {
      Con::printf("Failed to create context for SFXDevice");
      return;
   }

   mOpenAL.alcMakeContextCurrent(mContext);
   ///---Get device info and capabilities
   mDeviceName = mOpenAL.alcGetString(mDevice, ALC_DEVICE_SPECIFIER);
   mOpenAL.alcGetIntegerv(mDevice, ALC_MAJOR_VERSION, sizeof(U32), &mMajorVersion);
   mOpenAL.alcGetIntegerv(mDevice, ALC_MINOR_VERSION, sizeof(U32), &mMinorVersion);

   Con::printf("SFXDevice: %s v%i.%i", mDeviceName, mMajorVersion, mMinorVersion);

   mCaps = 0;
   // Check for ALC Extensions
   if (mOpenAL.alcIsExtensionPresent(mDevice, "ALC_EXT_CAPTURE") == AL_TRUE)
      mCaps |= SFXCapture;
   if (mOpenAL.alcIsExtensionPresent(mDevice, "ALC_EXT_EFX") == AL_TRUE)
      mCaps |= SFXEFX;

   // Check for AL Extensions
   if (mOpenAL.alIsExtensionPresent("AL_EXT_OFFSET") == AL_TRUE)
      mCaps |= SFXOffset;

   if (mOpenAL.alIsExtensionPresent("AL_EXT_LINEAR_DISTANCE") == AL_TRUE)
      mCaps |= SFXLinearDistance;
   if (mOpenAL.alIsExtensionPresent("AL_EXT_EXPONENT_DISTANCE") == AL_TRUE)
      mCaps |= SFXExponentDistance;

   if (mOpenAL.alIsExtensionPresent("EAX2.0") == AL_TRUE)
      mCaps |= SFXEAX2;
   if (mOpenAL.alIsExtensionPresent("EAX3.0") == AL_TRUE)
      mCaps |= SFXEAX3;
   if (mOpenAL.alIsExtensionPresent("EAX4.0") == AL_TRUE)
      mCaps |= SFXEAX4;
   if (mOpenAL.alIsExtensionPresent("EAX5.0") == AL_TRUE)
      mCaps |= SFXEAX5;

   if (mOpenAL.alIsExtensionPresent("EAX-RAM") == AL_TRUE)
      mCaps |= SFXEAXRAM;

   Con::printf("SFXDevice: Caps: %i", mCaps);

   //Find out max sources supported and assign it.
   maxSources = GetMaxNumSources();
   ///---Get device info and capabilities END

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
   Con::printf("SFXDevice created.");
}

SFXDevice::~SFXDevice()
{
   Con::printf("SFXDevice: Shutting down!");
   mOpenAL.alcMakeContextCurrent(NULL);
   mOpenAL.alcDestroyContext(mContext);
   mOpenAL.alcCloseDevice(mDevice);

   delete mProvider;
}

U32 SFXDevice::GetMaxNumSources()
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