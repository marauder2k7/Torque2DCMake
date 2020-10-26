
#ifndef _SFXCOMMON_H_
#define _SFXCOMMON_H_

#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif

#ifndef _MMATHFN_H_
#include "math/mMathFn.h"
#endif // !_MMATHFN_H_

class SFXEnvironment;

//-----------------------------------------------------------------------------
//    SFXListenerProperties.
//-----------------------------------------------------------------------------


///
class SFXListenerProperties
{
public:

   typedef void Parent;

   /// Position and orientation of the listener.
   MatrixF mTransform;

   ///
   Point3F mVelocity;

   SFXListenerProperties()
      : mTransform(true),
      mVelocity(0.0f, 0.0f, 0.0f) {}

   SFXListenerProperties(const MatrixF& transform, const Point3F& velocity)
      : mTransform(transform),
      mVelocity(velocity) {}

   ///
   const MatrixF& getTransform() const { return mTransform; }
   MatrixF& getTransform() { return mTransform; }

   ///
   const Point3F& getVelocity() const { return mVelocity; }
   Point3F& getVelocity() { return mVelocity; }
};

//-----------------------------------------------------------------------------
//    SFXStatus.
//-----------------------------------------------------------------------------


/// The sound playback state.
enum SFXStatus
{
   /// Initial state; no operation yet performed on sound.
   SFXStatusNull,

   /// Sound is playing.
   SFXStatusPlaying,

   /// Sound has been stopped.
   SFXStatusStopped,

   /// Sound is paused.
   SFXStatusPaused,

   /// Sound stream is starved and playback blocked.
   SFXStatusBlocked,

   /// Temporary state while transitioning to another state.  This is used when multiple
   /// threads concurrently maintain a status and need to perform a sequence of actions before
   /// being able to fully go from a previous to a new current state.  In this case, the
   /// transition state marks the status as being under update on another thread.
   ///
   /// @note Not all places that use SFXStatus actually use this state.
   SFXStatusTransition,
};

enum SFXDistanceModel
{
   SFXDistanceModelLinear,             ///< Volume decreases linearly from min to max where it reaches zero.
   SFXDistanceModelLogarithmic,        ///< Volume halves every min distance steps starting from min distance; attenuation stops at max distance.
   SFXDistanceModelExponent,           /// exponential falloff for distance attenuation.
};

//EnumTable(SFXStatus);

class SFXFormat
{
protected:

   /// The number of sound channels in the data.
   U8 mChannels;

   /// The number of bits per sound sample.
   U8 mBitsPerSample;

   /// The frequency in samples per second.
   U32 mSamplesPerSecond;

public:

   SFXFormat(U8 channels = 0,
      U8 bitsPerSample = 0,
      U32 samplesPerSecond = 0)
      : mChannels(channels),
      mBitsPerSample(bitsPerSample),
      mSamplesPerSecond(samplesPerSecond)
   {}

   /// Copy constructor.
   SFXFormat(const SFXFormat &format)
      : mChannels(format.mChannels),
      mBitsPerSample(format.mBitsPerSample),
      mSamplesPerSecond(format.mSamplesPerSecond)
   {}

public:

   /// Sets the format.
   void set(U8 channels,
      U8 bitsPerSample,
      U32 samplesPerSecond)
   {
      mChannels = channels;
      mBitsPerSample = bitsPerSample;
      mSamplesPerSecond = samplesPerSecond;
   }

   /// Comparision between formats.
   bool operator == (const SFXFormat& format) const
   {
      return   mChannels == format.mChannels &&
         mBitsPerSample == format.mBitsPerSample &&
         mSamplesPerSecond == format.mSamplesPerSecond;
   }

   /// Returns the number of sound channels.
   U8 getChannels() const { return mChannels; }

   /// Returns true if there is a single sound channel.
   bool isMono() const { return mChannels == 1; }

   /// Is true if there are two sound channels.
   bool isStereo() const { return mChannels == 2; }

   /// Is true if there are more than two sound channels.
   bool isMultiChannel() const { return mChannels > 2; }

   /// 
   U32 getSamplesPerSecond() const { return mSamplesPerSecond; }

   /// The bits of data per channel.
   U8 getBitsPerChannel() const { return mBitsPerSample / mChannels; }

   /// The number of bytes of data per channel.
   U8 getBytesPerChannel() const { return getBitsPerChannel() / 8; }

   /// The number of bits per sound sample.
   U8 getBitsPerSample() const { return mBitsPerSample; }

   /// The number of bytes of data per sample.
   /// @note Be aware that this comprises all channels.
   U8 getBytesPerSample() const { return mBitsPerSample / 8; }

   /// Returns the duration from the sample count.
   U32 getDuration(U32 samples) const
   {
      // Use 64bit types to avoid overflow during division. 
      return ((U64)samples * (U64)1000) / (U64)mSamplesPerSecond;
   }

   ///
   U32 getSampleCount(U32 ms) const
   {
      return U64(mSamplesPerSecond) * U64(ms) / U64(1000);
   }

   /// Returns the data length in bytes.
   U32 getDataLength(U32 ms) const
   {
      U32 bytes = (((U64)ms * (U64)mSamplesPerSecond) * (U64)getBytesPerSample()) / (U64)1000;
      return bytes;
   }
};

//-----------------------------------------------------------------------------
//    SFXReverb.
//-----------------------------------------------------------------------------


/// Reverb environment properties.
///
/// @note A given device may not implement all properties.
///restructure our reverbproperties to match openal

class SFXReverbProperties
{
public:

   struct Parent;

   float flDensity;
   float flDiffusion;
   float flGain;
   float flGainHF;
   float flGainLF;
   float flDecayTime;
   float flDecayHFRatio;
   float flDecayLFRatio;
   float flReflectionsGain;
   float flReflectionsDelay;
   float flReflectionsPan[3];
   float flLateReverbGain;
   float flLateReverbDelay;
   float flLateReverbPan[3];
   float flEchoTime;
   float flEchoDepth;
   float flModulationTime;
   float flModulationDepth;
   float flAirAbsorptionGainHF;
   float flHFReference;
   float flLFReference;
   float flRoomRolloffFactor;
   int   iDecayHFLimit;

   ///set our defaults to be the same as no reverb otherwise our reverb
   ///effects menu sounds
   SFXReverbProperties()
   {
      flDensity = 0.0f;
      flDiffusion = 0.0f;
      flGain = 0.0f;
      flGainHF = 0.0f;
      flGainLF = 0.0000f;
      flDecayTime = 0.0f;
      flDecayHFRatio = 0.0f;
      flDecayLFRatio = 0.0f;
      flReflectionsGain = 0.0f;
      flReflectionsDelay = 0.0f;
      flReflectionsPan[3] = 0.0f;
      flLateReverbGain = 0.0f;
      flLateReverbDelay = 0.0f;
      flLateReverbPan[3] = 0.0f;
      flEchoTime = 0.0f;
      flEchoDepth = 0.0f;
      flModulationTime = 0.0f;
      flModulationDepth = 0.0f;
      flAirAbsorptionGainHF = 0.0f;
      flHFReference = 0.0f;
      flLFReference = 0.0f;
      flRoomRolloffFactor = 0.0f;
      iDecayHFLimit = 0;
   }

   void validate()
   {
      flDensity = mClampF(flDensity, 0.0f, 1.0f);
      flDiffusion = mClampF(flDiffusion, 0.0f, 1.0f);
      flGain = mClampF(flGain, 0.0f, 1.0f);
      flGainHF = mClampF(flGainHF, 0.0f, 1.0f);
      flGainLF = mClampF(flGainLF, 0.0f, 1.0f);
      flDecayTime = mClampF(flDecayTime, 0.1f, 20.0f);
      flDecayHFRatio = mClampF(flDecayHFRatio, 0.1f, 2.0f);
      flDecayLFRatio = mClampF(flDecayLFRatio, 0.1f, 2.0f);
      flReflectionsGain = mClampF(flReflectionsGain, 0.0f, 3.16f);
      flReflectionsDelay = mClampF(flReflectionsDelay, 0.0f, 0.3f);
      flReflectionsPan[0] = mClampF(flReflectionsPan[0], -1.0f, 1.0f);
      flReflectionsPan[1] = mClampF(flReflectionsPan[1], -1.0f, 1.0f);
      flReflectionsPan[2] = mClampF(flReflectionsPan[2], -1.0f, 1.0f);
      flLateReverbGain = mClampF(flLateReverbGain, 0.0f, 10.0f);
      flLateReverbDelay = mClampF(flLateReverbDelay, 0.0f, 0.1f);
      flLateReverbPan[0] = mClampF(flLateReverbPan[0], -1.0f, 1.0f);
      flLateReverbPan[1] = mClampF(flLateReverbPan[1], -1.0f, 1.0f);
      flLateReverbPan[2] = mClampF(flLateReverbPan[2], -1.0f, 1.0f);
      flEchoTime = mClampF(flEchoTime, 0.075f, 0.25f);
      flEchoDepth = mClampF(flEchoDepth, 0.0f, 1.0f);
      flModulationTime = mClampF(flModulationTime, 0.04f, 4.0f);
      flModulationDepth = mClampF(flModulationDepth, 0.0f, 1.0f);
      flAirAbsorptionGainHF = mClampF(flAirAbsorptionGainHF, 0.892f, 1.0f);
      flHFReference = mClampF(flHFReference, 1000.0f, 20000.0f);
      flLFReference = mClampF(flLFReference, 20.0f, 1000.0f);
      flRoomRolloffFactor = mClampF(flRoomRolloffFactor, 0.0f, 10.0f);
      iDecayHFLimit = mClampF(iDecayHFLimit, 0, 1);
   }
};


//-----------------------------------------------------------------------------
//    SFXSoundReverbProperties.
//-----------------------------------------------------------------------------


/// Sound reverb properties.
///
/// @note A given SFX device may not implement all properties.
///not in use by openal yet if u are going to use ambient reverb zones its 
///probably best to not have reverb on the sound effect itself.
class SFXSoundReverbProperties
{
public:

   typedef void Parent;

   float flDensity;
   float flDiffusion;
   float flGain;
   float flGainHF;
   float flGainLF;
   float flDecayTime;
   float flDecayHFRatio;
   float flDecayLFRatio;
   float flReflectionsGain;
   float flReflectionsDelay;
   float flReflectionsPan[3];
   float flLateReverbGain;
   float flLateReverbDelay;
   float flLateReverbPan[3];
   float flEchoTime;
   float flEchoDepth;
   float flModulationTime;
   float flModulationDepth;
   float flAirAbsorptionGainHF;
   float flHFReference;
   float flLFReference;
   float flRoomRolloffFactor;
   int   iDecayHFLimit;


   ///Set our defaults to have no reverb
   ///if you are going to use zone reverbs its
   ///probably best not to use per-voice reverb
   SFXSoundReverbProperties()
   {
      flDensity = 0.0f;
      flDiffusion = 0.0f;
      flGain = 0.0f;
      flGainHF = 0.0f;
      flGainLF = 0.0000f;
      flDecayTime = 0.0f;
      flDecayHFRatio = 0.0f;
      flDecayLFRatio = 0.0f;
      flReflectionsGain = 0.0f;
      flReflectionsDelay = 0.0f;
      flReflectionsPan[3] = 0.0f;
      flLateReverbGain = 0.0f;
      flLateReverbDelay = 0.0f;
      flLateReverbPan[3] = 0.0f;
      flEchoTime = 0.0f;
      flEchoDepth = 0.0f;
      flModulationTime = 0.0f;
      flModulationDepth = 0.0f;
      flAirAbsorptionGainHF = 0.0f;
      flHFReference = 0.0f;
      flLFReference = 0.0f;
      flRoomRolloffFactor = 0.0f;
      iDecayHFLimit = 0;
   }

   void validate()
   {
      flDensity = mClampF(flDensity, 0.0f, 1.0f);
      flDiffusion = mClampF(flDiffusion, 0.0f, 1.0f);
      flGain = mClampF(flGain, 0.0f, 1.0f);
      flGainHF = mClampF(flGainHF, 0.0f, 1.0f);
      flGainLF = mClampF(flGainLF, 0.0f, 1.0f);
      flDecayTime = mClampF(flDecayTime, 0.1f, 20.0f);
      flDecayHFRatio = mClampF(flDecayHFRatio, 0.1f, 2.0f);
      flDecayLFRatio = mClampF(flDecayLFRatio, 0.1f, 2.0f);
      flReflectionsGain = mClampF(flReflectionsGain, 0.0f, 3.16f);
      flReflectionsDelay = mClampF(flReflectionsDelay, 0.0f, 0.3f);
      flReflectionsPan[0] = mClampF(flReflectionsPan[0], -1.0f, 1.0f);
      flReflectionsPan[1] = mClampF(flReflectionsPan[1], -1.0f, 1.0f);
      flReflectionsPan[2] = mClampF(flReflectionsPan[2], -1.0f, 1.0f);
      flLateReverbGain = mClampF(flLateReverbGain, 0.0f, 10.0f);
      flLateReverbDelay = mClampF(flLateReverbDelay, 0.0f, 0.1f);
      flLateReverbPan[0] = mClampF(flLateReverbPan[0], -1.0f, 1.0f);
      flLateReverbPan[1] = mClampF(flLateReverbPan[1], -1.0f, 1.0f);
      flLateReverbPan[2] = mClampF(flLateReverbPan[2], -1.0f, 1.0f);
      flEchoTime = mClampF(flEchoTime, 0.075f, 0.25f);
      flEchoDepth = mClampF(flEchoDepth, 0.0f, 1.0f);
      flModulationTime = mClampF(flModulationTime, 0.04f, 4.0f);
      flModulationDepth = mClampF(flModulationDepth, 0.0f, 1.0f);
      flAirAbsorptionGainHF = mClampF(flAirAbsorptionGainHF, 0.892f, 1.0f);
      flHFReference = mClampF(flHFReference, 1000.0f, 20000.0f);
      flLFReference = mClampF(flLFReference, 20.0f, 1000.0f);
      flRoomRolloffFactor = mClampF(flRoomRolloffFactor, 0.0f, 10.0f);
      iDecayHFLimit = mClampF(iDecayHFLimit, 0, 1);
   }
};

#endif // !_SFXCOMMON_H_
