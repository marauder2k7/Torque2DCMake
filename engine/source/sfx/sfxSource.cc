//-----------------------------------------------------------------------------
// Copyright (c) 2012 GarageGames, LLC
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

#include "sfx/sfxSource.h"
#include "sfx/sfxSystem.h"
#include "sfx/sfxTrack.h"
#include "sfx/sfxTypes.h"
#include "sfx/sfxDescription.h"
//#include "sfx/sfxModifier.h"
#include "console/consoleTypes.h"
#include "math/mRandom.h"
#include "math/mMathFn.h"



//#define DEBUG_SPEW


IMPLEMENT_CONOBJECT(SFXSource);


//-----------------------------------------------------------------------------

SFXSource::SFXSource()
   : mStatus(SFXStatusStopped),
   mSavedStatus(SFXStatusNull),
   mStatusCallback(NULL),
   mDescription(NULL),
   mVolume(1.f),
   mPreFadeVolume(1.f),
   mFadedVolume(1.f),
   mModulativeVolume(1.f),
   mPreAttenuatedVolume(1.f),
   mAttenuatedVolume(1.f),
   mPriority(0),
   mModulativePriority(1.f),
   mEffectivePriority(0),
   mPitch(1.f),
   mModulativePitch(1.f),
   mEffectivePitch(1.f),
   mTransform(true),
   mVelocity(0, 0, 0),
   mMinDistance(1),
   mMaxDistance(100),
   mConeInsideAngle(360),
   mConeOutsideAngle(360),
   mConeOutsideVolume(1),
   mDistToListener(0.f),
   mTransformScattered(false),
   mFadeInTime(0.f),
   mFadeOutTime(0.f),
   mFadeInPoint(-1.f),
   mFadeOutPoint(-1.f),
   mFadeSegmentType(FadeSegmentNone),
   mFadeSegmentStartPoint(0.f),
   mFadeSegmentEndPoint(0.f),
   mSavedFadeTime(-1.f),
   mPlayStartTick(0)
{
}

//-----------------------------------------------------------------------------

SFXSource::SFXSource(SFXTrack* track, SFXDescription* description)
   : mStatus(SFXStatusStopped),
   mSavedStatus(SFXStatusNull),
   mStatusCallback(NULL),
   mTrack(track),
   mDescription(description),
   mVolume(1.f),
   mPreFadeVolume(1.f),
   mFadedVolume(1.f),
   mModulativeVolume(1.f),
   mPreAttenuatedVolume(1.f),
   mAttenuatedVolume(1.f),
   mPriority(0),
   mModulativePriority(1.f),
   mEffectivePriority(0),
   mPitch(1.f),
   mModulativePitch(1.f),
   mEffectivePitch(1.f),
   mTransform(true),
   mVelocity(0, 0, 0),
   mMinDistance(1),
   mMaxDistance(100),
   mConeInsideAngle(360),
   mConeOutsideAngle(360),
   mConeOutsideVolume(1),
   mDistToListener(0.f),
   mTransformScattered(false),
   mFadeInTime(0.f),
   mFadeOutTime(0.f),
   mFadeInPoint(-1.f),
   mFadeOutPoint(-1.f),
   mFadeSegmentType(FadeSegmentNone),
   mFadeSegmentStartPoint(0.f),
   mFadeSegmentEndPoint(0.f),
   mSavedFadeTime(-1.f),
   mPlayStartTick(0)
{

   if (!description && track)
      mDescription = track->getDescription();

   // Make sure we get notified when our datablocks go away
   // so we can kill this source.

   if (mTrack != NULL)
      deleteNotify(mTrack);
   deleteNotify(mDescription);
}

//-----------------------------------------------------------------------------

SFXSource::~SFXSource()
{
   // Disconnect from any remaining parameters.
}

//-----------------------------------------------------------------------------

void SFXSource::initPersistFields()
{
   addGroup("Sound");

   addProtectedField("Description", TypeSFXDescriptionName, Offset(mDescription, SFXSource), &_setDescription, &_getDescription, " ");

   addField("statusCallback", TypeString, Offset(mStatusCallback, SFXSource),
      "Name of function to call when the status of the source changes.\n\n"
      "The source that had its status changed is passed as the first argument to the function and the "
      "new status of the source is passed as the second argument.");

   endGroup("Sound");

   Parent::initPersistFields();
}

//-----------------------------------------------------------------------------

bool SFXSource::processArguments(S32 argc, const char **argv)
{
   // Don't allow subclasses of this to be created via script.  Force
   // usage of the SFXSystem functions.

   if (typeid(*this) != typeid(SFXSource))
   {
      Con::errorf(ConsoleLogEntry::Script, "Use sfxCreateSource, sfxPlay, or sfxPlayOnce!");
      return false;
   }
   else
      return Parent::processArguments(argc, argv);
}

//-----------------------------------------------------------------------------

bool SFXSource::onAdd()
{
   if (!Parent::onAdd())
      return false;

   // Make sure we have a description.

   if (!mDescription)
   {
      Con::errorf("SFXSource::onAdd() - no description set on source %i (%s)", getId(), getName());
      return false;
   }

   // Set our initial properties.

   _setVolume(mDescription->mVolume);
   _setPitch(mDescription->mPitch);
   _setFadeTimes(mDescription->mFadeInTime, mDescription->mFadeOutTime);

   _setMinMaxDistance(mDescription->mReferenceDistance, mDescription->mMaxDistance);
   _setCone(F32(mDescription->mConeInsideAngle),
      F32(mDescription->mConeOutsideAngle),
      mDescription->mConeOutsideVolume);

   // Add the parameters from the track.


   // Add to source set.

   if (Sim::getSFXSourceSet())
      Sim::getSFXSourceSet()->addObject(this);

   // Register with SFX system.

   SFX->_onAddSource(this);

   return true;
}

//-----------------------------------------------------------------------------

void SFXSource::onRemove()
{
   stop();

   // Let the system know.
   SFX->_onRemoveSource(this);

#ifdef DEBUG_SPEW
   Platform::outputDebugString("[SFXSource] removed source '%i'", getId());
#endif

   Parent::onRemove();
}

//-----------------------------------------------------------------------------

void SFXSource::onDeleteNotify(SimObject* object)
{
   if (object == mTrack)
   {
      deleteObject();
      return;
   }

   Parent::onDeleteNotify(object);
}

//-----------------------------------------------------------------------------

bool SFXSource::acceptsAsChild(SimObject* object)
{
   return (dynamic_cast<SFXSource*>(object) != NULL);
}

//-----------------------------------------------------------------------------

void SFXSource::onGroupAdd()
{
   Parent::onGroupAdd();

   SFXSource* source = dynamic_cast<SFXSource*>(getGroup());
   if (source)
   {
      if (source != mDescription->mSourceGroup)
         mFlags.set(CustomGroupFlag);
      else
         mFlags.clear(CustomGroupFlag);
   }

   //DRTODO: sync up playback state
}

//-----------------------------------------------------------------------------

SFXSource* SFXSource::getSourceGroup() const
{
   return dynamic_cast<SFXSource*>(getGroup());
}

//-----------------------------------------------------------------------------

F32 SFXSource::getElapsedPlayTime() const
{
   return F32(mPlayTimer.getPosition()) / 1000.f;
}

//-----------------------------------------------------------------------------

F32 SFXSource::getElapsedPlayTimeCurrentCycle() const
{
   // In this base class, we can't make assumptions about total playtimes
   // and thus cannot clamp the playtimer into range for the current cycle.
   // This needs to be done by subclasses.

   return F32(mPlayTimer.getPosition()) / 1000.f;
}

//-----------------------------------------------------------------------------

F32 SFXSource::getTotalPlayTime() const
{
   return mPlayStartTick - Platform::getRealMilliseconds();
}

//-----------------------------------------------------------------------------

void SFXSource::play(F32 fadeInTime)
{
   SFXStatus status = getStatus();

   // Return if the source is already playing.

   if (status == SFXStatusPlaying)
   {
      // Revert fade-out if there is one.

      if (mFadeSegmentType != FadeSegmentNone && mFadeSegmentType != FadeSegmentPlay)
      {
         // Let easing curve remain in place.  Otherwise we would have to
         // search the fade-in's easing curve for the point matching the
         // current fade volume in order to prevent volume pops.

         mFadeSegmentType = FadeSegmentPlay;
      }

      return;
   }

   // First check the parent source.  If it is not playing,
   // only save our non-inherited state and return.

   SFXSource* sourceGroup = getSourceGroup();
   if (sourceGroup != NULL && !sourceGroup->isPlaying())
   {
      mSavedStatus = SFXStatusPlaying;
      mSavedFadeTime = fadeInTime;

      return;
   }

   // Add fades, if required.

   if (fadeInTime == -1.f)
      fadeInTime = mFadeInTime;

   if (status == SFXStatusPaused && fadeInTime > 0.f)
   {
      // Source is paused.  Set up temporary fade-in segment
      mFadeSegmentType = FadeSegmentPlay;
      mFadeSegmentStartPoint = getElapsedPlayTimeCurrentCycle();
      mFadeSegmentEndPoint = mFadeSegmentStartPoint + fadeInTime;
   }
   else if (fadeInTime > 0.f)
   {
      mFadeInPoint = fadeInTime;

      // Add fade-out if play-time of the source is finite and
      // if it is either not looping or has fading enabled on loops.

      F32 totalPlayTime = getTotalPlayTime();
      if (mDescription->mFadeOutTime > 0.f && (!mDescription->mIsLooping))
      {
         mFadeOutPoint = totalPlayTime - mDescription->mFadeOutTime;

         // If there's an intersection between fade-in and fade-out, move them
         // to the midpoint.

         if (mFadeOutPoint < mFadeInPoint)
         {
            F32 midPoint = mFadeOutPoint + (mFadeInPoint - mFadeOutPoint / 2);

            mFadeInPoint = midPoint;
            mFadeOutPoint = midPoint;
         }
      }
   }
   else
   {
      mFadeInPoint = -1.f;
      mFadeOutPoint = -1.f;
   }

   // Start playback.

   if (status != SFXStatusPaused)
   {
      mPlayStartTick = Platform::getVirtualMilliseconds();
      mPlayTimer.reset();
   }

   _setStatus(SFXStatusPlaying);
   _play();

#ifdef DEBUG_SPEW
   Platform::outputDebugString("[SFXSource] Started playback of source '%i'", getId());
#endif
}

//-----------------------------------------------------------------------------

void SFXSource::pause(F32 fadeOutTime)
{
   SFXStatus status = getStatus();
   if (status != SFXStatusPlaying)
      return;

   // Pause playback.

   if (fadeOutTime == -1.f)
      fadeOutTime = mFadeOutTime;

   if (fadeOutTime > 0.f)
      _setupFadeOutSegment(FadeSegmentPause, fadeOutTime);
   else
   {
      _setStatus(SFXStatusPaused);
      _pause();

      mFadeSegmentType = FadeSegmentNone;
      mPreFadeVolume = mVolume;

#ifdef DEBUG_SPEW
      Platform::outputDebugString("[SFXSource] Paused playback of source '%i'", getId());
#endif
   }
}

//-----------------------------------------------------------------------------

void SFXSource::stop(F32 fadeOutTime)
{
   SFXStatus status = getStatus();
   if (status == SFXStatusStopped)
      return;

   if (status == SFXStatusPaused)
   {
      _setStatus(SFXStatusStopped);
      _stop();
      return;
   }

   if (fadeOutTime == -1.f)
      fadeOutTime = mFadeOutTime;

   if (fadeOutTime > 0.f)
      _setupFadeOutSegment(FadeSegmentStop, fadeOutTime);
   else
   {
      _setStatus(SFXStatusStopped);
      _stop();

      mFadeSegmentType = FadeSegmentNone;
      mPreFadeVolume = mVolume;

#ifdef DEBUG_SPEW
      Platform::outputDebugString("[SFXSource] Stopped playback of source '%i'", getId());
#endif
   }
}

//-----------------------------------------------------------------------------

void SFXSource::update()
{
   if (!isPlaying())
      return;

   _update();
}

//-----------------------------------------------------------------------------

void SFXSource::_play()
{
   mPlayTimer.start();

   // Resume playback of children that want to be in
   // playing state.

   for (iterator iter = begin(); iter != end(); ++iter)
   {
      SFXSource* source = dynamic_cast<SFXSource*>(*iter);
      if (source && source->mSavedStatus == SFXStatusPlaying)
         source->play(source->mSavedFadeTime);
   }
}

//-----------------------------------------------------------------------------

void SFXSource::_pause()
{
   // Pause playing child sources.

   for (iterator iter = begin(); iter != end(); ++iter)
   {
      SFXSource* source = dynamic_cast<SFXSource*>(*iter);
      if (source && source->isPlaying())
      {
         source->pause(0.f);

         // Save info for resuming playback.

         source->mSavedStatus = SFXStatusPlaying;
         source->mSavedFadeTime = 0.f;
      }
   }

   mPlayTimer.pause();
}

//-----------------------------------------------------------------------------

void SFXSource::_stop()
{
   // Stop all child sources.

   for (iterator iter = begin(); iter != end(); ++iter)
   {
      SFXSource* source = dynamic_cast<SFXSource*>(*iter);
      if (source)
         source->stop(0.f);
   }

   mPlayTimer.stop();
}

//-----------------------------------------------------------------------------

void SFXSource::_update()
{
   /// Update our modulated properties.

   _updateVolume(SFX->getListener().getTransform());
   _updatePitch();
   _updatePriority();
}

//-----------------------------------------------------------------------------

bool SFXSource::_setDescription(void* obj, const char* data)
{
   SFXSource* source = reinterpret_cast<SFXSource*>(obj);

   source->mDescription = reinterpret_cast<SFXDescription*>(Sim::findObject(data));

   if (!source->mDescription)
   {
      Con::errorf("SFXSource::_setDescription - No SFXDescription '%s'", data);
      return false;
   }

   source->notifyDescriptionChanged();

   return false;
}

//-----------------------------------------------------------------------------

const char* SFXSource::_getDescription(void* obj, const char* data)
{
   SFXSource* source = reinterpret_cast<SFXSource*>(obj);
   SFXDescription* description = source->mDescription;

   if (!description)
      return "";

   return description->getName();
}

//-----------------------------------------------------------------------------

void SFXSource::_setStatus(SFXStatus status)
{
   if (mStatus == status)
      return;

   mStatus = status;

   // Clear saved status.

   mSavedStatus = SFXStatusNull;

   // Do the callback if we have it.

   if (mStatusCallback && mStatusCallback[0])
   {
      const char* statusString = SFXStatusToString(mStatus);
      Con::executef(this, 1,statusString);
   }
}

//-----------------------------------------------------------------------------

void SFXSource::_updateVolume(const MatrixF& listener)
{
   // Handle fades (compute mFadedVolume).

   mFadedVolume = mPreFadeVolume;
   if (mFadeSegmentType != FadeSegmentNone)
   {
      // We have a temporary fade segment.

      F32 elapsed;
      if (mDescription->mIsLooping)
         elapsed = getElapsedPlayTime();
      else
         elapsed = getElapsedPlayTimeCurrentCycle();

      if (elapsed < mFadeSegmentEndPoint)
      {
         const F32 duration = mFadeSegmentEndPoint - mFadeSegmentStartPoint;

         if (mFadeSegmentType == FadeSegmentPlay)
         {
            const F32 time = elapsed - mFadeSegmentStartPoint;
            (time, 0.f, mPreFadeVolume, duration);
         }
         else
         {
            // If the end-point to the ease functions is 0,
            // we'll always get out the start point.  Thus do the whole
            // thing backwards here.

            const F32 time = mFadeSegmentEndPoint - elapsed;
            (time, 0.f, mPreFadeVolume, duration);
         }
      }
      else
      {
         // The fade segment has played.  Remove it.

         switch (mFadeSegmentType)
         {
         case FadeSegmentStop:
            stop(0.f);
            break;

         case FadeSegmentPause:
            pause(0.f);
            break;

         case FadeSegmentPlay: // Nothing to do.               
         default:
            break;
         }

         mFadeSegmentType = FadeSegmentNone;
         mPreFadeVolume = mVolume;
      }
   }
   else if (mFadeInPoint != -1 || mFadeOutPoint != -1)
   {
      F32 elapsed;
      if (mDescription->mIsLooping )
         elapsed = getElapsedPlayTime();
      else
         elapsed = getElapsedPlayTimeCurrentCycle();

      // Check for fade-in.

      if (mFadeInPoint != -1)
      {
            // Deactivate fade-in so we don't see it on further loops.
            mFadeInPoint = -1;
         
      }

      // Check for fade-out.

      if (mFadeOutPoint != -1 && elapsed > mFadeOutPoint)
      {
         const F32 totalPlayTime = getTotalPlayTime();
         const F32 duration = totalPlayTime - mFadeOutPoint;
         const F32 time = totalPlayTime - elapsed;

      }
   }

   // Compute the pre-attenuated volume.

   mPreAttenuatedVolume =
      mFadedVolume
      * mModulativeVolume;

   SFXSource* group = getSourceGroup();
   if (group)
      mPreAttenuatedVolume *= group->getAttenuatedVolume();

   if (!is3d())
   {
      mDistToListener = 0.0f;
      mAttenuatedVolume = mPreAttenuatedVolume;
   }
   else
   {
      // For 3D sounds, compute distance attenuation.

      Point3F pos, lpos;
      mTransform.getColumn(3, &pos);
      listener.getColumn(3, &lpos);

      mDistToListener = (pos - lpos).len();
      mAttenuatedVolume = SFXDistanceAttenuation(
         SFX->getDistanceModel(),
         mMinDistance,
         mMaxDistance,
         mDistToListener,
         mPreAttenuatedVolume,
         SFX->getRolloffFactor());
   }
}

//-----------------------------------------------------------------------------

void SFXSource::_updatePitch()
{
   mEffectivePitch = mModulativePitch * mPitch;
}

//-----------------------------------------------------------------------------

void SFXSource::_updatePriority()
{
   mEffectivePriority = mPriority * mModulativePriority;

   SFXSource* group = getSourceGroup();
   if (group)
      mEffectivePriority *= group->getEffectivePriority();
}

//-----------------------------------------------------------------------------

void SFXSource::setTransform(const MatrixF& transform)
{
   mTransform = transform;
}

//-----------------------------------------------------------------------------

void SFXSource::setVelocity(const VectorF& velocity)
{
   mVelocity = velocity;
}

//-----------------------------------------------------------------------------

void SFXSource::setFadeTimes(F32 fadeInTime, F32 fadeOutTime)
{
   _setFadeTimes(fadeInTime, fadeOutTime);

   if (mFadeInTime >= 0.f || mFadeOutTime >= 0.f)
      mFlags.set(CustomFadeFlag);
}

//-----------------------------------------------------------------------------

void SFXSource::_setFadeTimes(F32 fadeInTime, F32 fadeOutTime)
{
   if (fadeInTime >= 0.f)
      mFadeInTime = getMax(0.f, fadeInTime);
   else
      mFadeInTime = mDescription->mFadeInTime;

   if (fadeOutTime >= 0.f)
      mFadeOutTime = getMax(0.f, fadeOutTime);
   else
      mFadeOutTime = mDescription->mFadeOutTime;
}

//-----------------------------------------------------------------------------

void SFXSource::_setupFadeOutSegment(FadeSegmentType type, F32 fadeOutTime)
{
   // Get the current faded volume as the start volume so
   // that we correctly fade when starting in a fade.

   _updateVolume(SFX->getListener().getTransform());
   mPreFadeVolume = mFadedVolume;

   mFadeSegmentType = type;

   if (mDescription->mIsLooping)
   {
      mFadeSegmentStartPoint = getElapsedPlayTimeCurrentCycle();
      mFadeSegmentEndPoint = getMin(mFadeSegmentStartPoint + fadeOutTime, getTotalPlayTime());
   }
   else
   {
      mFadeSegmentStartPoint = getElapsedPlayTime();
      mFadeSegmentEndPoint = mFadeSegmentStartPoint + fadeOutTime;
   }
}

//-----------------------------------------------------------------------------

void SFXSource::_setMinMaxDistance(F32 min, F32 max)
{
   mMinDistance = getMax(0.0f, min);
   mMaxDistance = getMax(mMinDistance, max);
}

//-----------------------------------------------------------------------------

void SFXSource::_setCone(F32 innerAngle, F32 outerAngle, F32 outerVolume)
{
   mConeInsideAngle = mClampF(innerAngle, 0.0, 360.0);
   mConeOutsideAngle = mClampF(outerAngle, mConeInsideAngle, 360.0);
   mConeOutsideVolume = mClampF(outerVolume, 0.0, 1.0);
}

//-----------------------------------------------------------------------------

void SFXSource::_setVolume(F32 volume)
{
   mVolume = mClampF(volume, 0.f, 1.f);
   mPreFadeVolume = mVolume;
   _updateVolume(SFX->getListener(0).getTransform());
}

//-----------------------------------------------------------------------------

void SFXSource::setModulativeVolume(F32 value)
{
   mModulativeVolume = mClampF(value, 0.f, 1.f);
   _updateVolume(SFX->getListener(0).getTransform());
}

//-----------------------------------------------------------------------------

void SFXSource::_setPitch(F32 pitch)
{
   mPitch = mClampF(pitch, 0.001f, 2.0f);
   _updatePitch();
}

//-----------------------------------------------------------------------------

void SFXSource::setModulativePitch(F32 value)
{
   mModulativePitch = mClampF(value, 0.001f, 2.0f);
   _updatePitch();
}

//-----------------------------------------------------------------------------

void SFXSource::_setPriority(F32 priority)
{
   mPriority = priority;
   _updatePriority();
}

//-----------------------------------------------------------------------------

void SFXSource::setModulativePriority(F32 value)
{
   mModulativePriority = value;
   _updatePriority();
}

//-----------------------------------------------------------------------------

SFXTrack* SFXSource::getTrack() const
{
   return mTrack;
}

//-----------------------------------------------------------------------------

void SFXSource::notifyDescriptionChanged()
{
   if (!mFlags.test(CustomVolumeFlag))
      _setVolume(mDescription->mVolume);
   if (!mFlags.test(CustomPitchFlag))
      _setPitch(mDescription->mPitch);
   if (!mFlags.test(CustomRadiusFlag))
      _setMinMaxDistance(mDescription->mReferenceDistance, mDescription->mMaxDistance);
   if (!mFlags.test(CustomConeFlag))
      _setCone(mDescription->mConeInsideAngle, mDescription->mConeOutsideAngle, mDescription->mConeOutsideVolume);
   if (!mFlags.test(CustomFadeFlag))
      _setFadeTimes(mDescription->mFadeInTime, mDescription->mFadeOutTime);
   if (!mFlags.test(CustomGroupFlag) && mDescription->mSourceGroup != NULL && getGroup() != mDescription->mSourceGroup)
      mDescription->mSourceGroup->addObject(this);
}

//-----------------------------------------------------------------------------

void SFXSource::notifyTrackChanged()
{
   //RDTODO
}
