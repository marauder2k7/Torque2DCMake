
#include "sfx/sfxDevice.h"
#include "sfx/sfxBuffer.h"
#include "sfx/sfxVoice.h"
#include "console/console.h"
#include "console/consoleTypes.h"

SFXDevice::SFXDevice(SFXProvider *provider,
                     const OPENALFNTABLE &openal,
                     StringTableEntry name,
                     bool useHardware,
                     S32 maxBuffers)
   :  mProvider(provider),
      mOpenAL(openal),
      mName(name),
      mUseHardware(useHardware),
      mContext(NULL),
      mDevice(NULL),
      mDistanceModel(SFXDistanceModelLinear),
      mDistanceFactor(1.0f),
      mRolloffFactor(1.0f),
      mUserRolloffFactor(1.0f)
{
   mMaxBuffers = getMax(maxBuffers, 8);

   // TODO: The OpenAL device doesn't set the primary buffer
   // $pref::SFX::frequency or $pref::SFX::bitrate!
   //check auxiliary device sends 4 and add them to the device
   ALint attribs[4] = { 0 };
#if defined(AL_ALEXT_PROTOTYPES)
   ALCint iSends = 0;
   attribs[0] = ALC_MAX_AUXILIARY_SENDS;
#endif
   attribs[1] = 4;

   mDevice = mOpenAL.alcOpenDevice(name);
   mOpenAL.alcGetError(mDevice);
   if (mDevice)
   {
      mContext = mOpenAL.alcCreateContext(mDevice, attribs);

      if (mContext)
         mOpenAL.alcMakeContextCurrent(mContext);
#if defined(AL_ALEXT_PROTOTYPES)
      mOpenAL.alcGetIntegerv(mDevice, ALC_MAX_AUXILIARY_SENDS, 1, &iSends);
#endif
      U32 err = mOpenAL.alcGetError(mDevice);

      if (err != ALC_NO_ERROR)
         Con::errorf("SFXALDevice - Initialization Error: %s", mOpenAL.alcGetString(mDevice, err));
   }

   AssertFatal(mDevice != NULL && mContext != NULL, "Failed to create OpenAL device and/or context!");

   // Start the update thread.
   // TODO AsyncPeriodicUpdateThread support for Linux/Mac
#ifdef TORQUE_OS_WIN
   if (!Con::getBoolVariable("$_forceAllMainThread"))
   {
      SFXInternal::gUpdateThread = new AsyncPeriodicUpdateThread
      ("OpenAL Update Thread", SFXInternal::gBufferUpdateList,
         Con::getIntVariable("$pref::SFX::updateInterval", SFXInternal::DEFAULT_UPDATE_INTERVAL));
      SFXInternal::gUpdateThread->start();
   }
#endif

#if defined(AL_ALEXT_PROTOTYPES)
   dMemset(effectSlot, 0, sizeof(effectSlot));
   dMemset(effect, 0, sizeof(effect));
   uLoop = 0;
#endif
}

SFXDevice::~SFXDevice()
{
#if defined(AL_ALEXT_PROTOTYPES)
   mOpenAL.alDeleteAuxiliaryEffectSlots(4, effectSlot);
   mOpenAL.alDeleteEffects(2, effect);
#endif
   ///cleanup of effects ends
   mOpenAL.alcMakeContextCurrent(NULL);
   mOpenAL.alcDestroyContext(mContext);
   mOpenAL.alcCloseDevice(mDevice);
}

void SFXDevice::setReverb(const SFXReverbProperties & reverb)
{
   Platform::outputDebugString("Updated");
   ///load an efxeaxreverb default and add our values from
   ///sfxreverbproperties to it
   EFXEAXREVERBPROPERTIES prop = EFX_REVERB_PRESET_GENERIC;

   prop.flDensity = reverb.flDensity;
   prop.flDiffusion = reverb.flDiffusion;
   prop.flGain = reverb.flGain;
   prop.flGainHF = reverb.flGainHF;
   prop.flGainLF = reverb.flGainLF;
   prop.flDecayTime = reverb.flDecayTime;
   prop.flDecayHFRatio = reverb.flDecayHFRatio;
   prop.flDecayLFRatio = reverb.flDecayLFRatio;
   prop.flReflectionsGain = reverb.flReflectionsGain;
   prop.flReflectionsDelay = reverb.flReflectionsDelay;
   prop.flLateReverbGain = reverb.flLateReverbGain;
   prop.flLateReverbDelay = reverb.flLateReverbDelay;
   prop.flEchoTime = reverb.flEchoTime;
   prop.flEchoDepth = reverb.flEchoDepth;
   prop.flModulationTime = reverb.flModulationTime;
   prop.flModulationDepth = reverb.flModulationDepth;
   prop.flAirAbsorptionGainHF = reverb.flAirAbsorptionGainHF;
   prop.flHFReference = reverb.flHFReference;
   prop.flLFReference = reverb.flLFReference;
   prop.flRoomRolloffFactor = reverb.flRoomRolloffFactor;
   prop.iDecayHFLimit = reverb.iDecayHFLimit;

   if (mOpenAL.alGetEnumValue("AL_EFFECT_EAXREVERB") != 0)
   {

      /// EAX Reverb is available. Set the EAX effect type

      mOpenAL.alEffecti(effect[0], AL_EFFECT_TYPE, AL_EFFECT_EAXREVERB);

      ///add our values to the setup of the reverb

      mOpenAL.alEffectf(effect[0], AL_EAXREVERB_DENSITY, prop.flDensity);
      mOpenAL.alEffectf(effect[0], AL_EAXREVERB_DIFFUSION, prop.flDiffusion);
      mOpenAL.alEffectf(effect[0], AL_EAXREVERB_GAIN, prop.flGain);
      mOpenAL.alEffectf(effect[0], AL_EAXREVERB_GAINHF, prop.flGainHF);
      mOpenAL.alEffectf(effect[0], AL_EAXREVERB_GAINLF, prop.flGainLF);
      mOpenAL.alEffectf(effect[0], AL_EAXREVERB_DECAY_TIME, prop.flDecayTime);
      mOpenAL.alEffectf(effect[0], AL_EAXREVERB_DECAY_HFRATIO, prop.flDecayHFRatio);
      mOpenAL.alEffectf(effect[0], AL_EAXREVERB_DECAY_LFRATIO, prop.flDecayLFRatio);
      mOpenAL.alEffectf(effect[0], AL_EAXREVERB_REFLECTIONS_GAIN, prop.flReflectionsGain);
      mOpenAL.alEffectf(effect[0], AL_EAXREVERB_REFLECTIONS_DELAY, prop.flReflectionsDelay);
      mOpenAL.alEffectf(effect[0], AL_EAXREVERB_LATE_REVERB_GAIN, prop.flLateReverbGain);
      mOpenAL.alEffectf(effect[0], AL_EAXREVERB_LATE_REVERB_DELAY, prop.flLateReverbDelay);
      mOpenAL.alEffectf(effect[0], AL_EAXREVERB_ECHO_TIME, prop.flEchoTime);
      mOpenAL.alEffectf(effect[0], AL_EAXREVERB_ECHO_DEPTH, prop.flEchoDepth);
      mOpenAL.alEffectf(effect[0], AL_EAXREVERB_MODULATION_TIME, prop.flModulationTime);
      mOpenAL.alEffectf(effect[0], AL_EAXREVERB_MODULATION_DEPTH, prop.flModulationDepth);
      mOpenAL.alEffectf(effect[0], AL_EAXREVERB_AIR_ABSORPTION_GAINHF, prop.flAirAbsorptionGainHF);
      mOpenAL.alEffectf(effect[0], AL_EAXREVERB_HFREFERENCE, prop.flHFReference);
      mOpenAL.alEffectf(effect[0], AL_EAXREVERB_LFREFERENCE, prop.flLFReference);
      mOpenAL.alEffectf(effect[0], AL_EAXREVERB_ROOM_ROLLOFF_FACTOR, prop.flRoomRolloffFactor);
      mOpenAL.alEffecti(effect[0], AL_EAXREVERB_DECAY_HFLIMIT, prop.iDecayHFLimit);
      mOpenAL.alAuxiliaryEffectSloti(1, AL_EFFECTSLOT_EFFECT, effect[0]);
      Platform::outputDebugString("eax reverb properties set");

   }
   else
   {

      /// No EAX Reverb. Set the standard reverb effect
      mOpenAL.alEffecti(effect[0], AL_EFFECT_TYPE, AL_EFFECT_REVERB);

      mOpenAL.alEffectf(effect[0], AL_REVERB_DENSITY, prop.flDensity);
      mOpenAL.alEffectf(effect[0], AL_REVERB_DIFFUSION, prop.flDiffusion);
      mOpenAL.alEffectf(effect[0], AL_REVERB_GAIN, prop.flGain);
      mOpenAL.alEffectf(effect[0], AL_REVERB_GAINHF, prop.flGainHF);
      mOpenAL.alEffectf(effect[0], AL_REVERB_DECAY_TIME, prop.flDecayTime);
      mOpenAL.alEffectf(effect[0], AL_REVERB_DECAY_HFRATIO, prop.flDecayHFRatio);
      mOpenAL.alEffectf(effect[0], AL_REVERB_REFLECTIONS_GAIN, prop.flReflectionsGain);
      mOpenAL.alEffectf(effect[0], AL_REVERB_REFLECTIONS_DELAY, prop.flReflectionsDelay);
      mOpenAL.alEffectf(effect[0], AL_REVERB_LATE_REVERB_GAIN, prop.flLateReverbGain);
      mOpenAL.alEffectf(effect[0], AL_REVERB_LATE_REVERB_DELAY, prop.flLateReverbDelay);
      mOpenAL.alEffectf(effect[0], AL_REVERB_AIR_ABSORPTION_GAINHF, prop.flAirAbsorptionGainHF);
      mOpenAL.alEffectf(effect[0], AL_REVERB_ROOM_ROLLOFF_FACTOR, prop.flRoomRolloffFactor);
      mOpenAL.alEffecti(effect[0], AL_REVERB_DECAY_HFLIMIT, prop.iDecayHFLimit);
      mOpenAL.alAuxiliaryEffectSloti(1, AL_EFFECTSLOT_EFFECT, effect[0]);

   }
}

void SFXDevice::update()
{

}

SFXBuffer * SFXDevice::createBuffer(const SFXStream* stream, SFXDescription * description)
{
   SFXBuffer* buffer = SFXBuffer::create(mOpenAL,
                                          stream,
                                          description,
                                          mUseHardware);
                                       if (!buffer)
                                          return NULL;

   _addBuffer(buffer);
   return buffer;
}

SFXVoice * SFXDevice::createVoice(bool is3D, SFXBuffer * buffer)
{
   // Don't bother going any further if we've 
// exceeded the maximum voices.
   if (mVoices.size() >= mMaxBuffers)
      return NULL;

   AssertFatal(buffer, "SFXALDevice::createVoice() - Got null buffer!");

   SFXBuffer* alBuffer = dynamic_cast<SFXBuffer*>(buffer);
   AssertFatal(alBuffer, "SFXALDevice::createVoice() - Got bad buffer!");

   SFXVoice* voice = SFXVoice::create(this, alBuffer);
   if (!voice)
      return NULL;

   _addVoice(voice);
   return voice;
}

void SFXDevice::setListener(U32 index, const SFXListenerProperties & listener)
{
   if (index != 0)
      return;

   // Torque and OpenAL are both right handed 
   // systems, so no coordinate flipping is needed.

   const MatrixF &transform = listener.getTransform();
   Point3F pos, tupple[2];
   transform.getColumn(3, &pos);
   transform.getColumn(1, &tupple[0]);
   transform.getColumn(2, &tupple[1]);

   const VectorF &velocity = listener.getVelocity();

   mOpenAL.alListenerfv(AL_POSITION, pos);
   mOpenAL.alListenerfv(AL_VELOCITY, velocity);
   mOpenAL.alListenerfv(AL_ORIENTATION, (const F32 *)&tupple[0]);
   ///Pass a unit size to openal, 1.0 assumes 1 meter to 1 game unit.
   ///Crucial for air absorbtion calculations.
#if defined(AL_ALEXT_PROTOTYPES)
   mOpenAL.alListenerf(AL_METERS_PER_UNIT, 1.0f);
#endif
}

void SFXDevice::setDistanceModel(SFXDistanceModel model)
{
   switch (model)
   {
   case SFXDistanceModelLinear:
      mOpenAL.alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);
      if (mRolloffFactor != 1.0f)
         _setRolloffFactor(1.0f); // No rolloff on linear.
      break;

   case SFXDistanceModelLogarithmic:
      mOpenAL.alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);
      if (mUserRolloffFactor != mRolloffFactor)
         _setRolloffFactor(mUserRolloffFactor);
      break;
      /// create a case for our exponential distance model
   case SFXDistanceModelExponent:
      mOpenAL.alDistanceModel(AL_EXPONENT_DISTANCE_CLAMPED);
      if (mUserRolloffFactor != mRolloffFactor)
         _setRolloffFactor(mUserRolloffFactor);
      break;

   default:
      AssertWarn(false, "SFXDevice::setDistanceModel - distance model not implemented");
   }

   mDistanceModel = model;
}

void SFXDevice::setDopplerFactor(F32 factor)
{
   mOpenAL.alDopplerFactor(factor);
}

void SFXDevice::_setRolloffFactor(F32 factor)
{
   mRolloffFactor = factor;

   for (U32 i = 0, num = mVoices.size(); i < num; ++i)
      mOpenAL.alSourcef(((SFXVoice*)mVoices[i])->mSourceName, AL_ROLLOFF_FACTOR, factor);
}

void SFXDevice::setRolloffFactor(F32 factor)
{
   if (mDistanceModel == SFXDistanceModelLinear && factor != 1.0f)
      Con::errorf("SFXALDevice::setRolloffFactor - rolloff factor <> 1.0f ignored in linear distance model");
   else
      _setRolloffFactor(factor);

   mUserRolloffFactor = factor;
}

void SFXDevice::openSlots()
{
   for (uLoop = 0; uLoop < 4; uLoop++)
   {
      mOpenAL.alGenAuxiliaryEffectSlots(1, &effectSlot[uLoop]);
   }

   for (uLoop = 0; uLoop < 2; uLoop++)
   {
      mOpenAL.alGenEffects(1, &effect[uLoop]);
   }
   ///debug string output so we know our slots are open
   Platform::outputDebugString("Slots Open");
}
