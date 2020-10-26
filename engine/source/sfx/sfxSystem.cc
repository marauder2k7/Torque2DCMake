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

#include "platform/platform.h"
#include "sfx/sfxSystem.h"

#include "sfx/sfxProvider.h"
#include "sfx/sfxDevice.h"
//#include "sfx/sfxInternal.h"
#include "sfx/sfxSource.h"
#include "sfx/sfxProfile.h"
#include "sfx/sfxDescription.h"
#include "sfx/sfxTrack.h"
//#include "sfx/sfxPlayList.h"
#include "sfx/sfxSound.h"
//#include "sfx/sfxController.h"
//#include "sfx/sfxSoundscape.h"

#include "console/console.h"
#include "console/consoleTypes.h"
//#include "T3D/gameBase/processList.h"

#include "sfx/media/sfxWavStream.h"
#ifndef TORQUE_OS_IOS
#include "sfx/media/sfxVorbisStream.h"
#endif

SFXSystem* SFXSystem::smSingleton = NULL;





         // Constants.
         static const U32 sDeviceCapsReverb = SFXDevice::CAPS_Reverb;
         static const U32 sDeviceCapsVoiceManagement = SFXDevice::CAPS_VoiceManagement;
         static const U32 sDeviceCapsOcclusion = SFXDevice::CAPS_Occlusion;
         static const U32 sDeviceCapsDSPEffects = SFXDevice::CAPS_DSPEffects;
         static const U32 sDeviceCapsMultiListener = SFXDevice::CAPS_MultiListener;
         static const U32 sDeviceCapsFMODDesigner = SFXDevice::CAPS_FMODDesigner;

         static const U32 sDeviceInfoProvider = 0;
         static const U32 sDeviceInfoName = 1;
         static const U32 sDeviceInfoUseHardware = 2;
         static const U32 sDeviceInfoMaxBuffers = 3;


         //-----------------------------------------------------------------------------

         SFXSystem::SFXSystem()
            : mDevice(NULL),
            mLastSourceUpdateTime(0),
            mLastAmbientUpdateTime(0),
            mLastParameterUpdateTime(0),
            mStatNumSources(0),
            mStatNumSounds(0),
            mStatNumPlaying(0),
            mStatNumCulled(0),
            mStatNumVoices(0),
            mStatSourceUpdateTime(0),
            mStatParameterUpdateTime(0),
            mDistanceModel(SFXDistanceModelLinear),
            mStatAmbientUpdateTime(0),
            mDopplerFactor(0.5),
            mRolloffFactor(1.0)
         {
            VECTOR_SET_ASSOCIATION(mSounds);
            VECTOR_SET_ASSOCIATION(mPlayOnceSources);
            VECTOR_SET_ASSOCIATION(mPlugins);
            VECTOR_SET_ASSOCIATION(mListeners);

            // Always at least one listener.

            mListeners.increment();

            // Register stat variables.

            Con::addVariable("SFX::numSources", TypeS32, &mStatNumSources);
            Con::addVariable("SFX::numSounds", TypeS32, &mStatNumSounds);
            Con::addVariable("SFX::numPlaying", TypeS32, &mStatNumPlaying);
            Con::addVariable("SFX::numCulled", TypeS32, &mStatNumCulled);
            Con::addVariable("SFX::numVoices", TypeS32, &mStatNumVoices);
            Con::addVariable("SFX::sourceUpdateTime", TypeS32, &mStatSourceUpdateTime);
            Con::addVariable("SFX::parameterUpdateTime", TypeS32, &mStatParameterUpdateTime);
            Con::addVariable("SFX::ambientUpdateTime", TypeS32, &mStatAmbientUpdateTime);

            // Register constants.
         }

         //-----------------------------------------------------------------------------

         SFXSystem::~SFXSystem()
         {
            // Unregister stat variables.

            Con::removeVariable("SFX::numSources");
            Con::removeVariable("SFX::numSounds");
            Con::removeVariable("SFX::numPlaying");
            Con::removeVariable("SFX::numCulled");
            Con::removeVariable("SFX::numVoices");
            Con::removeVariable("SFX::sourceUpdateTime");
            Con::removeVariable("SFX::parameterUpdateTime");
            Con::removeVariable("SFX::ambientUpdateTime");

            // Cleanup any remaining sources.

            if (Sim::getSFXSourceSet())
               Sim::getSFXSourceSet()->clear();

            mSounds.clear();
            mPlayOnceSources.clear();
            mListeners.clear();

            deleteDevice();
         }

         //-----------------------------------------------------------------------------

         void SFXSystem::init()
         {
            AssertWarn(smSingleton == NULL, "SFX has already been initialized!");

            SFXProvider::initializeAllProviders();

            // Note: If you have provider specific file types you should
            // register them in the provider initialization.

            // Create the system.
            smSingleton = new SFXSystem();
         }

         //-----------------------------------------------------------------------------

         void SFXSystem::destroy()
         {
            AssertWarn(smSingleton != NULL, "SFX has not been initialized!");

            delete smSingleton;
            smSingleton = NULL;

         }

         //-----------------------------------------------------------------------------

         void SFXSystem::addPlugin(SFXSystemPlugin* plugin)
         {
            for (U32 i = 0; i < mPlugins.size(); ++i)
               AssertFatal(mPlugins[i] != plugin, "SFXSystem::addPlugin - plugin already added to the system!");

            mPlugins.push_back(plugin);
         }

         //-----------------------------------------------------------------------------

         void SFXSystem::removePlugin(SFXSystemPlugin* plugin)
         {
            for (U32 i = 0; i < mPlugins.size(); ++i)
               if (mPlugins[i] == plugin)
               {
                  mPlugins.erase_fast(i);
                  break;
               }
         }

         //-----------------------------------------------------------------------------

         bool SFXSystem::createDevice(const char& providerName, const StringTableEntry& deviceName, bool useHardware, S32 maxBuffers, bool changeDevice)
         {
            // Make sure we don't have a device already.

            if (mDevice && !changeDevice)
               return false;

            // Lookup the provider.

            SFXProvider* provider = SFXProvider::findProvider(providerName);
            if (!provider)
               return false;

            // If we have an existing device remove it.

            if (mDevice)
               deleteDevice();

            // Create the new device..

            mDevice = provider->createDevice(deviceName, useHardware, maxBuffers);
            if (!mDevice)
            {
               Con::errorf("SFXSystem::createDevice - failed creating %s device '%s'", providerName, deviceName);
               return false;
            }

            // Print capabilities.

            Con::printf("SFXSystem::createDevice - created %s device '%s'", providerName, deviceName);
            if (mDevice->getCaps() & SFXDevice::CAPS_Reverb)
               Con::printf("   CAPS_Reverb");
            if (mDevice->getCaps() & SFXDevice::CAPS_VoiceManagement)
               Con::printf("   CAPS_VoiceManagement");
            if (mDevice->getCaps() & SFXDevice::CAPS_Occlusion)
               Con::printf("\tCAPS_Occlusion");
            if (mDevice->getCaps() & SFXDevice::CAPS_MultiListener)
               Con::printf("\tCAPS_MultiListener");

            // Set defaults.

            mDevice->setNumListeners(getNumListeners());
            mDevice->setDistanceModel(mDistanceModel);
            mDevice->setDopplerFactor(mDopplerFactor);
            mDevice->setRolloffFactor(mRolloffFactor);
            //OpenAL requires slots for effects, this creates an empty function 
            //that will run when a sfxdevice is created.
            mDevice->openSlots();
            mDevice->setReverb(mReverb);

            return true;
         }

         //-----------------------------------------------------------------------------

         char SFXSystem::getDeviceInfoString()
         {
            char str;
            // Make sure we have a valid device.
            if (!mDevice)
               return str;

            dStrcpy((char*)str, (char*)mDevice->getProvider()->getName());
            dStrcat((char*)str, (char*)mDevice->getName());
            dStrcat((char*)str, (char*)mDevice->getUseHardware());
            dStrcat((char*)str, (char*)mDevice->getCaps());

            return (char)str;
         }

         //-----------------------------------------------------------------------------

         void SFXSystem::deleteDevice()
         {
            // Make sure we have a valid device.
            if (!mDevice)
               return;

            // Put all playing sounds into virtualized playback mode.  Where this fails,
            // stop the source.

            for (U32 i = 0; i < mSounds.size(); ++i)
            {
               SFXSound* sound = mSounds[i];
               if (sound->hasVoice() && !sound->_releaseVoice())
                  sound->stop();
            }

            // Free the device which should delete all
            // the active voices and buffers.
            delete mDevice;
            mDevice = NULL;
         }

         //-----------------------------------------------------------------------------

         SFXSource* SFXSystem::createSource(SFXTrack* track,
            const MatrixF* transform,
            const VectorF* velocity)
         {
            if (!track)
               return NULL;

            SFXSource* source = NULL;

            // Try creating through a plugin first so that they
            // always get the first shot and may override our
            // logic here.

            for (U32 i = 0; !source && i < mPlugins.size(); ++i)
               source = mPlugins[i]->createSource(track);

            // If that failed, try our own logic using the track
            // types that we know about.

            if (!source)
            {
               if (!mDevice)
               {
                  Con::errorf("SFXSystem::createSource() - no device initialized!");
                  return NULL;
               }
               else
               {
                  Con::errorf("SFXSystem::createSource - cannot create source for %i (%s) of type '%s'",
                     track->getId(), track->getName(), track->getClassName());
                  Con::errorf("SFXSystem::createSource - maybe you are using the wrong SFX provider for this type of track");
                  return NULL;
               }
            }

            if (source)
            {
               if (transform)
                  source->setTransform(*transform);
               if (velocity)
                  source->setVelocity(*velocity);
            }

            return source;
         }

         //-----------------------------------------------------------------------------

         SFXSound* SFXSystem::createSourceFromStream(const SFXStream* stream,
            SFXDescription* description)
         {
            AssertFatal(mDevice, "SFXSystem::createSourceFromStream() - no device initialized!");

            // We sometimes get null values from script... fail in that case.

            if (!description)
               return NULL;

            // Create the sound.

            SFXSound* sound = SFXSound::_create(mDevice, stream, description);
            if (!sound)
               return NULL;

            return sound;
         }

         //-----------------------------------------------------------------------------

         void SFXSystem::stopAndDeleteSource(SFXSource* source)
         {
            if (source->getFadeOutTime() > 0.f)
            {
               // Fade-out.  Stop and put on play-once list to
               // ensure deletion when the source actually stops.

               source->stop();
               deleteWhenStopped(source);
            }
            else
            {
               // No fade-out.  Just stop and delete the source.

               source->stop();
               SFX_DELETE(source);
            }
         }

         //-----------------------------------------------------------------------------

         void SFXSystem::deleteWhenStopped(SFXSource* source)
         {
            // If the source isn't already on the play-once source list,
            // put it there now.
            Vector< SFXSource* >::iterator iter = mPlayOnceSources.begin();
            for (; iter != mPlayOnceSources.end(); iter++)
            {
               if (iter == mPlayOnceSources.end())
                  mPlayOnceSources.push_back(source);
            }
         }

         //-----------------------------------------------------------------------------

         void SFXSystem::_onAddSource(SFXSource* source)
         {
            if (dynamic_cast<SFXSound*>(source))
            {
               SFXSound* sound = static_cast<SFXSound*>(source);
               mSounds.push_back(sound);

               mStatNumSounds = mSounds.size();
            }

            // Update the stats.
            mStatNumSources++;
         }

         //-----------------------------------------------------------------------------

         void SFXSystem::_onRemoveSource(SFXSource* source)
         {
            // Check if it was a play once source.
            Vector< SFXSource* >::iterator sourceIter = mPlayOnceSources.begin();
            for (; sourceIter != mPlayOnceSources.end(); sourceIter++)
            {
               if (sourceIter != mPlayOnceSources.end())
                  mPlayOnceSources.erase_fast(sourceIter);
            }
            // Update the stats.

            mStatNumSources--;

            if (dynamic_cast<SFXSound*>(source))
            {
               SFXSoundVector::iterator vectorIter = mSounds.begin();
               if (vectorIter != mSounds.end())
                  mSounds.erase_fast(vectorIter);

               mStatNumSounds = mSounds.size();
            }
         }

         //-----------------------------------------------------------------------------

         SFXBuffer* SFXSystem::_createBuffer(const SFXStream* stream, SFXDescription* description)
         {
            // The buffers are created by the active
            // device... without one we cannot do anything.
            if (!mDevice)
            {
               Con::errorf("SFXSystem::_createBuffer - No sound device initialized!!");
               return NULL;
            }

            // Some sanity checking for streaming.  If the stream isn't at least three packets
            // in size given the current settings in "description", then turn off streaming.
            // The device code *will* mess up if the stream input fails to match certain metrics.
            // Just disabling streaming when it doesn't make sense is easier than complicating the
            // device logic to account for bad metrics.

            bool streamFlag = description->mIsStreaming;
            if (description->mIsStreaming)
               description->mIsStreaming = false;

            SFXBuffer* buffer = mDevice->createBuffer(stream, description);

            description->mIsStreaming = streamFlag; // restore in case we stomped it
            return buffer;
         }

         //-----------------------------------------------------------------------------

         SFXBuffer* SFXSystem::_createBuffer(const char& filename, SFXDescription* description)
         {
            if (!mDevice)
            {
               Con::errorf("SFXSystem::_createBuffer - No sound device initialized!!");
               return NULL;
            }

            return mDevice->createBuffer(filename, description);
         }

         //-----------------------------------------------------------------------------

         SFXSource* SFXSystem::playOnce(SFXTrack* track,
            const MatrixF *transform,
            const VectorF *velocity,
            F32 fadeInTime)
         {
            // We sometimes get null profiles... nothing to play without a profile!
            if (!track)
               return NULL;

            SFXSource *source = createSource(track, transform, velocity);
            if (source)
            {
               mPlayOnceSources.push_back(source);
               source->play(fadeInTime);
            }

            return source;
         }

         //-----------------------------------------------------------------------------

         void SFXSystem::_update()
         {


            for (U32 i = 0; i < mPlugins.size(); ++i)
               mPlugins[i]->update();

            // The update of the sources can be a bit expensive
            // and it does not need to be updated every frame.
            const U32 currentTime = Platform::getRealMilliseconds();

               _updateSources();
               mLastSourceUpdateTime = currentTime;

            // If we have a device then update it.
            if (mDevice)
               mDevice->update();
         }

         //-----------------------------------------------------------------------------

         void SFXSystem::_updateSources()
         {
            PROFILE_SCOPE(SFXSystem_UpdateSources);

            SimSet* sources = Sim::getSFXSourceSet();
            if (!sources)
               return;

            // Check the status of the sources here once.
            // 
            // NOTE: We do not use iterators in this loop because
            // SFXControllers can add to the source list during the
            // loop.
            //
            mStatNumPlaying = 0;
            for (S32 i = 0; i < sources->size(); i++)
            {
               SFXSource *source = dynamic_cast<SFXSource*>(sources->at(i));
               if (source)
               {
                  source->update();
                  if (source->getStatus() == SFXStatusPlaying)
                     ++mStatNumPlaying;
               }
            }

            // First check to see if any play once sources have
            // finished playback... delete them.

            for (SFXSourceVector::iterator iter = mPlayOnceSources.begin(); iter != mPlayOnceSources.end(); )
            {
               SFXSource* source = *iter;

               if (source->getLastStatus() == SFXStatusStopped &&
                  source->getSavedStatus() != SFXStatusPlaying)
               {
                  S32 index = iter - mPlayOnceSources.begin();

                  // Erase it from the vector first, so that onRemoveSource
                  // doesn't do it during cleanup and screw up our loop here!
                  mPlayOnceSources.erase_fast(iter);
                  source->deleteObject();

                  iter = mPlayOnceSources.begin() + index;
                  continue;
               }

               ++iter;
            }


            if (mDevice)
            {
               // Reassign buffers to the sounds (if voices are managed by
               // us instead of by the device).

               if (!(mDevice->getCaps() & SFXDevice::CAPS_VoiceManagement))
                  _assignVoices();

               // Update the voice count stat.
               mStatNumVoices = mDevice->getVoiceCount();
            }
         }

         //-----------------------------------------------------------------------------

         void SFXSystem::_sortSounds(const SFXListenerProperties& listener)
         {
            PROFILE_SCOPE(SFXSystem_SortSounds);

            // Sort the source vector by the attenuated 
            // volume and priorities.  This leaves us
            // with the loudest and highest priority sounds 
            // at the front of the vector.

            dQsort((void*)mSounds.address(), mSounds.size(), sizeof(SFXSound*), SFXSound::qsortCompare);
         }

         //-----------------------------------------------------------------------------

         void SFXSystem::_assignVoices()
         {
            AssertFatal(getNumListeners() == 1, "SFXSystem::_assignVoices() - must only have a single listener");
            PROFILE_SCOPE(SFXSystem_AssignVoices);

            mStatNumVoices = 0;
            mStatNumCulled = 0;

            if (!mDevice)
               return;

            // Sort the sources in the SFX source set by priority.  This also
            // updates each soures effective volume first.

            _sortSounds(getListener());

            // We now make sure that the sources closest to the 
            // listener, the ones at the top of the source list,
            // have a device buffer to play thru.

            mStatNumCulled = 0;
            for (SFXSoundVector::iterator iter = mSounds.begin(); iter != mSounds.end(); ++iter)
            {
               SFXSound* sound = *iter;

               // Non playing sources (paused or stopped) are at the
               // end of the vector, so when i encounter one i know 
               // that nothing else in the vector needs buffer assignment.

               if (!sound->isPlaying())
                  break;

               // If the source is outside it's max range we can
               // skip it as well, so that we don't waste cycles
               // setting up a buffer for something we won't hear.

               if (sound->getAttenuatedVolume() <= 0.0f)
               {
                  ++mStatNumCulled;
                  continue;
               }

               // If the source has a voice then we can skip it.

               if (sound->hasVoice())
                  continue;

               // Ok let the device try to assign a new voice for 
               // this source... this may fail if we're out of voices.

               if (sound->_allocVoice(mDevice))
                  continue;

               // The device couldn't assign a new voice, so we go through
               // local priority sounds and try to steal a voice.

               for (SFXSoundVector::iterator hijack = mSounds.end() - 1; hijack != iter; --hijack)
               {
                  SFXSound* other = *hijack;

                  if (other->hasVoice())
                  {
                     // If the sound is a suitable candidate, try to steal
                     // its voice.  While the sound definitely is lower down the chain
                     // in the total priority ordering, we don't want to steal voices
                     // from sounds that are clearly audible as that results in noticable
                     // sound pops.

                     if ((other->getAttenuatedVolume() < 0.1     // Very quiet or maybe not even audible.
                        || !other->isPlaying()                    // Not playing so not audible anyways.
                        || other->getPosition() == 0)            // Not yet started playing.
                        && other->_releaseVoice())
                        break;
                  }
               }

               // Ok try to assign a voice once again!

               if (sound->_allocVoice(mDevice))
                  continue;

               // If the source still doesn't have a buffer... well
               // tough cookies.  It just cannot be heard yet, maybe
               // it can in the next update.

               mStatNumCulled++;
            }

            // Update the voice count stat.
            mStatNumVoices = mDevice->getVoiceCount();
         }

         //-----------------------------------------------------------------------------

         void SFXSystem::_assignVoice(SFXSound* sound)
         {
            if (!mDevice)
               return;

            // Make sure all properties are up-to-date.

            sound->_update();

            // If voices are managed by the device, just let the sound
            // allocate a voice on it.  Otherwise, do a voice allocation pass
            // on all our active sounds.

            if (mDevice->getCaps() & SFXDevice::CAPS_VoiceManagement)
               sound->_allocVoice(mDevice);
            else
               _assignVoices();

            // Update the voice count stat.
            mStatNumVoices = mDevice->getVoiceCount();
         }

         //-----------------------------------------------------------------------------

         void SFXSystem::setDistanceModel(SFXDistanceModel model)
         {
            const bool changed = (model != mDistanceModel);

            mDistanceModel = model;
            if (mDevice && changed)
               mDevice->setDistanceModel(model);
         }

         //-----------------------------------------------------------------------------

         void SFXSystem::setDopplerFactor(F32 factor)
         {
            const bool changed = (factor != mDopplerFactor);

            mDopplerFactor = factor;
            if (mDevice && changed)
               mDevice->setDopplerFactor(factor);
         }

         //-----------------------------------------------------------------------------

         void SFXSystem::setRolloffFactor(F32 factor)
         {
            const bool changed = (factor != mRolloffFactor);

            mRolloffFactor = factor;
            if (mDevice && changed)
               mDevice->setRolloffFactor(factor);
         }

         //-----------------------------------------------------------------------------

         void SFXSystem::setReverb(const SFXReverbProperties& reverb)
         {
            mReverb = reverb;

            // Allow the plugins to adjust the reverb.

            for (U32 i = 0; i < mPlugins.size(); ++i)
               mPlugins[i]->filterReverb(mReverb);

            // Pass it on to the device.

            if (mDevice)
               mDevice->setReverb(mReverb);
         }

         //-----------------------------------------------------------------------------

         void SFXSystem::setNumListeners(U32 num)
         {
            // If we are set to a single listener, just accept this as
            // we always support this no matter what.

            if (num == 1)
            {
               mListeners.setSize(1);
               if (mDevice)
                  mDevice->setNumListeners(1);
               return;
            }

            // If setting to multiple listeners, make sure that the device
            // both supports multiple listeners and implements its own voice
            // management (as our voice virtualization does not work with more
            // than a single listener).

            if (!mDevice || !(mDevice->getCaps() & SFXDevice::CAPS_MultiListener)
               || !(mDevice->getCaps() & SFXDevice::CAPS_VoiceManagement))
            {
               Con::errorf("SFXSystem::setNumListeners() - multiple listeners not supported on current configuration");
               return;
            }

            mListeners.setSize(num);
            if (mDevice)
               mDevice->setNumListeners(num);
         }

         //-----------------------------------------------------------------------------

         void SFXSystem::setListener(U32 index, const MatrixF& transform, const Point3F& velocity)
         {
            if (index >= mListeners.size())
               return;

            mListeners[index] = SFXListenerProperties(transform, velocity);

            if (mDevice)
               mDevice->setListener(index, mListeners[index]);
         }

         //-----------------------------------------------------------------------------

         void SFXSystem::notifyDescriptionChanged(SFXDescription* description)
         {
            SimSet* set = Sim::getSFXSourceSet();
            for (SimSet::iterator iter = set->begin(); iter != set->end(); ++iter)
            {
               SFXSource* source = dynamic_cast<SFXSource*>(*iter);
               if (source && source->getDescription() == description)
                  source->notifyDescriptionChanged();
            }
         }

         //-----------------------------------------------------------------------------

         void SFXSystem::notifyTrackChanged(SFXTrack* track)
         {
            SimSet* set = Sim::getSFXSourceSet();
            for (SimSet::iterator iter = set->begin(); iter != set->end(); ++iter)
            {
               SFXSource* source = dynamic_cast<SFXSource*>(*iter);
               if (source && source->getTrack() == track)
                  source->notifyTrackChanged();
            }
         }

         //-----------------------------------------------------------------------------