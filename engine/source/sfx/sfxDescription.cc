
#include "platform/platform.h"
#include "sfx/sfxDescription.h"


IMPLEMENT_CO_DATABLOCK_V1(SFXDescription);

SFXDescription::SFXDescription()
{
   mVolume = 1.f;  // 0-1    1=loudest volume
   mVolumeChannel = 1;
   mIsLooping = false;
   mIsStreaming = false;

   mIs3D = true; //The default class is used with SceneObject.Playsound

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
   addField("VolumeChannel", TypeS32, Offset(mVolumeChannel, SFXDescription));
   addField("isLooping", TypeBool, Offset(mIsLooping, SFXDescription));
   addField("isStreaming", TypeBool, Offset(mIsStreaming, SFXDescription));

   addField("is3D", TypeBool, Offset(mIs3D, SFXDescription));
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
   if (!Parent::onAdd())
      return false;

   Sim::getSFXDescriptionSet()->addObject(this);
   return true;
}