//-----------------------------------------------------------------------------
// Copyright (c) 2013 GarageGames, LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------

#ifndef _SFXASSET_H_
#include "sfxAsset.h"
#endif

#ifndef _ASSET_PTR_H_
#include "assets/assetPtr.h"
#endif

#ifndef _CONSOLETYPES_H_
#include "console/consoleTypes.h"
#endif
#include "sfxDevice.h"

//-----------------------------------------------------------------------------

ConsoleType(sfxAssetPtr, TypeSFXAssetPtr, sizeof(AssetPtr<SFXAsset>), ASSET_ID_FIELD_PREFIX)

//-----------------------------------------------------------------------------

ConsoleGetType(TypeSFXAssetPtr)
{
   // Fetch asset Id.
   return (*((AssetPtr<SFXAsset>*)dptr)).getAssetId();
}

//-----------------------------------------------------------------------------

ConsoleSetType(TypeSFXAssetPtr)
{
   // Was a single argument specified?
   if (argc == 1)
   {
      // Yes, so fetch field value.
      const char* pFieldValue = argv[0];

      // Fetch asset pointer.
      AssetPtr<SFXAsset>* pAssetPtr = dynamic_cast<AssetPtr<SFXAsset>*>((AssetPtrBase*)(dptr));

      // Is the asset pointer the correct type?
      if (pAssetPtr == NULL)
      {
         // No, so fail.
         Con::warnf("(TypeSFXAssetPtr) - Failed to set asset Id '%d'.", pFieldValue);
         return;
      }

      // Set asset.
      pAssetPtr->setAssetId(pFieldValue);

      return;
   }

   // Warn.
   Con::warnf("(TypeSFXAssetPtr) - Cannot set multiple args to a single asset.");
}

//--------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(SFXAsset);

//--------------------------------------------------------------------------

SFXAsset::SFXAsset()
{
   mAudioFile = StringTable->EmptyString;
   mDescription.mVolume = 1.0f;
   mDescription.mVolumeChannel = 0;
   mDescription.mIsLooping = false;
   mDescription.mIsStreaming = false;

   mDescription.mIs3D = false;
   mDescription.mReferenceDistance = 1.0f;
   mDescription.mMaxDistance = 100.0f;
   mDescription.mEnvironmentLevel = 0.0f;
   mDescription.mConeInsideAngle = 360;
   mDescription.mConeOutsideAngle = 360;
   mDescription.mConeOutsideVolume = 1.0f;
   mDescription.mConeVector.set(0, 0, 1);

}

//--------------------------------------------------------------------------

void SFXAsset::initPersistFields()
{
   Parent::initPersistFields();

   addProtectedField("AudioFile", TypeAssetLooseFilePath, Offset(mAudioFile, SFXAsset), &setAudioFile, &getAudioFile, &defaultProtectedWriteFn, "");
   addProtectedField("Volume", TypeF32, Offset(mDescription.mVolume, SFXAsset), &setVolume, &defaultProtectedGetFn, &writeVolume, "");
   addProtectedField("VolumeChannel", TypeS32, Offset(mDescription.mVolumeChannel, SFXAsset), &setVolumeChannel, &defaultProtectedGetFn, &writeVolumeChannel, "");
   addProtectedField("Looping", TypeBool, Offset(mDescription.mIsLooping, SFXAsset), &setLooping, &defaultProtectedGetFn, &writeLooping, "");
   addProtectedField("Streaming", TypeBool, Offset(mDescription.mIsStreaming, SFXAsset), &setStreaming, &defaultProtectedGetFn, &writeStreaming, "");

   //addField("is3D",              TypeBool,    Offset(mDescription.mIs3D, AudioAsset));
   //addField("referenceDistance", TypeF32,     Offset(mDescription.mReferenceDistance, AudioAsset));
   //addField("maxDistance",       TypeF32,     Offset(mDescription.mMaxDistance, AudioAsset));
   //addField("coneInsideAngle",   TypeS32,     Offset(mDescription.mConeInsideAngle, AudioAsset));
   //addField("coneOutsideAngle",  TypeS32,     Offset(mDescription.mConeOutsideAngle, AudioAsset));
   //addField("coneOutsideVolume", TypeF32,     Offset(mDescription.mConeOutsideVolume, AudioAsset));
   //addField("coneVector",        TypePoint3F, Offset(mDescription.mConeVector, AudioAsset));
   //addField("environmentLevel",  TypeF32,     Offset(mDescription.mEnvironmentLevel, AudioAsset));
}

//------------------------------------------------------------------------------

void SFXAsset::copyTo(SimObject* object)
{
   // Call to parent.
   Parent::copyTo(object);

   // Cast to asset.
   SFXAsset* pAsset = static_cast<SFXAsset*>(object);

   // Sanity!
   AssertFatal(pAsset != NULL, "AudioAsset::copyTo() - Object is not the correct type.");

   // Copy state.
   pAsset->setAudioFile(getAudioFile());
   pAsset->setVolume(getVolume());
   pAsset->setVolumeChannel(getVolumeChannel());
   pAsset->setLooping(getLooping());
   pAsset->setStreaming(getStreaming());
}

//--------------------------------------------------------------------------

void SFXAsset::initializeAsset(void)
{
   // Call parent.
   Parent::initializeAsset();

   // Ensure the audio-file is expanded.
   mAudioFile = expandAssetFilePath(mAudioFile);

   // Asset should never auto-unload.
   setAssetAutoUnload(false);

   // Clamp these for now.
   if (mDescription.mIs3D)
   {
      mDescription.mReferenceDistance = mClampF(mDescription.mReferenceDistance, 0.0f, mDescription.mReferenceDistance);
      mDescription.mMaxDistance = mDescription.mMaxDistance > mDescription.mReferenceDistance ? mDescription.mMaxDistance : (mDescription.mReferenceDistance + 0.01f);
      mDescription.mEnvironmentLevel = mClampF(mDescription.mEnvironmentLevel, 0.0f, 1.0f);
      mDescription.mConeInsideAngle = mClamp(mDescription.mConeInsideAngle, 0, 360);
      mDescription.mConeOutsideAngle = mClamp(mDescription.mConeOutsideAngle, mDescription.mConeInsideAngle, 360);
      mDescription.mConeOutsideVolume = mClampF(mDescription.mConeOutsideVolume, 0.0f, 1.0f);
      mDescription.mConeVector.normalize();
   }
}

//--------------------------------------------------------------------------

void SFXAsset::setAudioFile(const char* pAudioFile)
{
   // Sanity!
   AssertFatal(pAudioFile != NULL, "Cannot use a NULL audio filename.");

   // Fetch audio filename.
   pAudioFile = StringTable->insert(pAudioFile);

   // Ignore no change,
   if (pAudioFile == mAudioFile)
      return;

   // Update.
   mAudioFile = getOwned() ? expandAssetFilePath(pAudioFile) : pAudioFile;

   // Refresh the asset.
   refreshAsset();
}

//--------------------------------------------------------------------------

void SFXAsset::setVolume(const F32 volume)
{
   // Ignore no change.
   if (mIsEqual(volume, mDescription.mVolume))
      return;

   // Update.
   mDescription.mVolume = mClampF(volume, 0.0f, 1.0f);;

   // Refresh the asset.
   refreshAsset();
}

//--------------------------------------------------------------------------

void SFXAsset::setVolumeChannel(const S32 volumeChannel)
{
   // Ignore no change.
   if (volumeChannel == mDescription.mVolumeChannel)
      return;

   // Update.
   mDescription.mVolumeChannel = volumeChannel;

   // Refresh the asset.
   refreshAsset();
}

//--------------------------------------------------------------------------

void SFXAsset::setLooping(const bool looping)
{
   // Ignore no change.
   if (looping == mDescription.mIsLooping)
      return;

   // Update.
   mDescription.mIsLooping = looping;

   // Refresh the asset.
   refreshAsset();
}


//--------------------------------------------------------------------------

void SFXAsset::setStreaming(const bool streaming)
{
   // Ignore no change.
   if (streaming == mDescription.mIsStreaming)
      return;

   // UPdate.
   mDescription.mIsStreaming = streaming;

   // Refresh the asset.
   refreshAsset();
}

//--------------------------------------------------------------------------

void SFXAsset::setDescription(const SFXDevice::DefDescription& audioDescription)
{
   // Update.
   mDescription = audioDescription;

   // Refresh the asset.
   refreshAsset();
}

//-----------------------------------------------------------------------------

void SFXAsset::onTamlPreWrite(void)
{
   // Call parent.
   Parent::onTamlPreWrite();

   // Ensure the audio-file is collapsed.
   mAudioFile = collapseAssetFilePath(mAudioFile);
}

//-----------------------------------------------------------------------------

void SFXAsset::onTamlPostWrite(void)
{
   // Call parent.
   Parent::onTamlPostWrite();

   // Ensure the audio-file is expanded.
   mAudioFile = expandAssetFilePath(mAudioFile);
}


