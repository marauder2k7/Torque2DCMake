#ifndef _SFXCOMMON_H_
#define _SFXCOMMON_H_

#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif
#ifndef _SFXDEVICE_H_
#include "sfx/sfxDevice.h"
#endif // !_SFXDEVICE_H_

#ifdef _VECTOR_H_
#include "collection/vector.h"
#endif // !_VECTOR_H_
#ifndef _TYPETRAITS_H_
#include "platform/typetraits.h"
#endif

#include "math/mMath.h"

class SFXEnvironment;

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


#endif