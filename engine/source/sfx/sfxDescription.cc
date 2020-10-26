#include "sfx/sfxDescription.h"
#include "console/consoleTypes.h"

IMPLEMENT_CO_DATABLOCK_V1(SFXDescription);

SFXDescription::SFXDescription()
{
   mVolume = 1.0f;  // 0-1    1=loudest volume
   mPitch = 1.0f;
   mVolumeChannel = 1;
   mIsLooping = false;
   mIsStreaming = false;

   mIs3D = true; //The default class is used with SceneObject.Playsound
   mUseReverb = false;
   mReferenceDistance = 25.f;
   mRollOffFactor = 5.0f;
   mMaxDistance = 50.f;

   mConeInsideAngle = 360;
   mConeOutsideAngle = 360;
   mConeOutsideVolume = 1.0f;
   mConeVector = { 0.f, 0.f, 1.f };

   // environment info
   mEnvironmentLevel = 0.f;
}

void SFXDescription::initPersistFields()
{
   Parent::initPersistFields();

   addField("Volume", TypeF32, Offset(mVolume, SFXDescription));
   addField("Pitch", TypeF32, Offset(mPitch, SFXDescription));
   addField("VolumeChannel", TypeS32, Offset(mVolumeChannel, SFXDescription));
   addField("isLooping", TypeBool, Offset(mIsLooping, SFXDescription));
   addField("isStreaming", TypeBool, Offset(mIsStreaming, SFXDescription));
   addField("is3D", TypeBool, Offset(mIs3D, SFXDescription));
   addField("Reverb", TypeBool, Offset(mUseReverb, SFXDescription));
   addField("ReferenceDistance", TypeF32, Offset(mReferenceDistance, SFXDescription));
   addField("RolloffFactor", TypeF32, Offset(mRollOffFactor, SFXDescription));
   addField("MaxDistance", TypeF32, Offset(mMaxDistance, SFXDescription));
   addField("ConeInsideAngle", TypeS32, Offset(mConeInsideAngle, SFXDescription));
   addField("ConeOutsideAngle", TypeS32, Offset(mConeOutsideAngle, SFXDescription));
   addField("ConeOutsideVolume", TypeF32, Offset(mConeOutsideVolume, SFXDescription));
   addField("ConeVector", TypeF32Vector, Offset(mConeVector, SFXDescription));
   addField("EnvironmentLevel", TypeF32, Offset(mEnvironmentLevel, SFXDescription));
}

bool SFXDescription::onAdd()
{
   if(!Parent::onAdd() )
      return false;

   return true;
}

void SFXDescription::packData(BitStream * stream)
{
   Parent::packData(stream);

   stream->writeFloat(mVolume, 6);
   stream->write(mPitch);
   stream->writeFlag(mIsLooping);
   stream->writeFlag(mIsStreaming);
   stream->writeFlag(mIs3D);
   stream->writeFloat(mReferenceDistance, 6);
   stream->write(mMaxDistance);
   stream->write(mRollOffFactor);
   stream->writeFlag(mUseReverb);
   stream->writeInt(mConeInsideAngle, 9);
   stream->writeInt(mConeOutsideAngle, 9);
   stream->writeFloat(mConeOutsideVolume, 6);

   if (mUseReverb)
   {
      stream->write(mReverb.flDensity);
      stream->write(mReverb.flDiffusion);
      stream->write(mReverb.flGain);
      stream->write(mReverb.flGainHF);
      stream->write(mReverb.flGainLF);
      stream->write(mReverb.flDecayTime);
      stream->write(mReverb.flDecayHFRatio);
      stream->write(mReverb.flDecayLFRatio);
      stream->write(mReverb.flReflectionsGain);
      stream->write(mReverb.flReflectionsDelay);
      stream->write(mReverb.flLateReverbGain);
      stream->write(mReverb.flLateReverbDelay);
      stream->write(mReverb.flEchoTime);
      stream->write(mReverb.flEchoDepth);
      stream->write(mReverb.flModulationTime);
      stream->write(mReverb.flModulationDepth);
      stream->write(mReverb.flAirAbsorptionGainHF);
      stream->write(mReverb.flHFReference);
      stream->write(mReverb.flLFReference);
      stream->write(mReverb.flRoomRolloffFactor);
      stream->write(mReverb.iDecayHFLimit);
   }
   stream->writeFloat(mEnvironmentLevel, 6);

}

void SFXDescription::unpackData(BitStream * stream)
{
   Parent::unpackData(stream);

   mVolume = stream->readFloat(6);
   stream->read(&mPitch);
   mIsLooping = stream->readFlag();
   mIsStreaming = stream->readFlag();
   mIs3D = stream->readFlag();
   stream->read(&mReferenceDistance);
   stream->read(&mMaxDistance);
   stream->read(&mRollOffFactor);
   mUseReverb = stream->readFlag();
   mConeInsideAngle = stream->readInt(9);
   mConeOutsideAngle = stream->readInt(9);
   mConeOutsideVolume = stream->readFloat(6);

   if (mUseReverb)
   {
      stream->read(&mReverb.flDensity);
      stream->read(&mReverb.flDiffusion);
      stream->read(&mReverb.flGain);
      stream->read(&mReverb.flGainHF);
      stream->read(&mReverb.flGainLF);
      stream->read(&mReverb.flDecayTime);
      stream->read(&mReverb.flDecayHFRatio);
      stream->read(&mReverb.flDecayLFRatio);
      stream->read(&mReverb.flReflectionsGain);
      stream->read(&mReverb.flReflectionsDelay);
      stream->read(&mReverb.flLateReverbGain);
      stream->read(&mReverb.flLateReverbDelay);
      stream->read(&mReverb.flEchoTime);
      stream->read(&mReverb.flEchoDepth);
      stream->read(&mReverb.flModulationTime);
      stream->read(&mReverb.flModulationDepth);
      stream->read(&mReverb.flAirAbsorptionGainHF);
      stream->read(&mReverb.flHFReference);
      stream->read(&mReverb.flLFReference);
      stream->read(&mReverb.flRoomRolloffFactor);
      stream->read(&mReverb.iDecayHFLimit);
   }

   mEnvironmentLevel = stream->readFloat(6);
}