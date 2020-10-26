#include "console/console.h"
#include "platform/platform.h"
#include "sfx/sfxDevice.h"
#include "sfxVoice.h"

SFXVoice * SFXVoice::create(SFXDevice* device, SFXBuffer * buffer)
{
   AssertFatal(buffer, "SFXALVoice::create() - Got null buffer!");

   ALuint sourceName;
   device->mOpenAL.alGenSources(1, &sourceName);
   AssertFatal(device->mOpenAL.alIsSource(sourceName), "AL Source Sanity Check Failed!");

   device->mOpenAL.alSourcei(sourceName, AL_SOURCE_RELATIVE, (buffer->mIs3d ? AL_FALSE : AL_TRUE));

   if (buffer->mIs3d)
      device->mOpenAL.alSourcef(sourceName, AL_ROLLOFF_FACTOR, device->mRolloffFactor);

   SFXVoice *voice = new SFXVoice(device->mOpenAL,
      buffer,
      sourceName);

   return voice;
}

SFXVoice::SFXVoice(const OPENALFNTABLE & oalft, 
   SFXBuffer * buffer, 
   ALuint sourceName)
   :  mSourceName(sourceName),
      mResumeAtSampleOffset(-1.0f),
      mSampleOffset(0),
      mOpenAL(oalft),
      mBuffer( buffer ),
      mStatus(SFXStatusNull),
      mOffset(0)
{
}

void SFXVoice::_lateBindStaticBufferIfNecessary()
{
}

SFXStatus SFXVoice::_status() const
{
   ALint state;
   mOpenAL.alGetSourcei(mSourceName, AL_SOURCE_STATE, &state);

   switch (state)
   {
   case AL_PLAYING:  return SFXStatusPlaying;
   case AL_PAUSED:   return SFXStatusPaused;
   default:          return SFXStatusStopped;
   }
}

void SFXVoice::_play()
{
   _lateBindStaticBufferIfNecessary();

#if defined(AL_ALEXT_PROTOTYPES)
   //send every voice that plays to the alauxiliary slot that has the reverb
   mOpenAL.alSource3i(mSourceName, AL_AUXILIARY_SEND_FILTER, 1, 0, AL_FILTER_NULL);
#endif
   mOpenAL.alSourcePlay(mSourceName);

   //WORKAROUND: Adjust play cursor for buggy OAL when resuming playback.  Do this after alSourcePlay
   // as it is the play function that will cause the cursor to jump.

   if (mResumeAtSampleOffset != -1.0f)
   {
      mOpenAL.alSourcef(mSourceName, AL_SAMPLE_OFFSET, mResumeAtSampleOffset);
      mResumeAtSampleOffset = -1.0f;
   }
}

void SFXVoice::_pause()
{
#ifdef DEBUG_SPEW
   Platform::outputDebugString("[SFXALVoice] Pausing playback");
#endif

   mOpenAL.alSourcePause(mSourceName);

   //WORKAROUND: Another workaround for buggy OAL.  Resuming playback of a paused source will cause the 
   // play cursor to jump.  Save the cursor so we can manually move it into position in _play().  Sigh.

   mOpenAL.alGetSourcef(mSourceName, AL_SAMPLE_OFFSET, &mResumeAtSampleOffset);
}

void SFXVoice::_stop()
{
#ifdef DEBUG_SPEW
   Platform::outputDebugString("[SFXALVoice] Stopping playback");
#endif

   mOpenAL.alSourceStop(mSourceName);
   mSampleOffset = 0;

   mResumeAtSampleOffset = -1.0f;
}

void SFXVoice::_seek(U32 sample)
{
   _lateBindStaticBufferIfNecessary();
   mOpenAL.alSourcei(mSourceName, AL_SAMPLE_OFFSET, sample);

   mResumeAtSampleOffset = -1.0f;
}

U32 SFXVoice::_tell() const
{

   ALint pos;
   mOpenAL.alGetSourcei(mSourceName, AL_SAMPLE_OFFSET, &pos);
   return (pos + mSampleOffset);
}

void SFXVoice::_resetStream(U32 sampleStartPos, bool triggerUpdate)
{
   mOpenAL.alSourceStop(mSourceName);
   mSampleOffset = 0;

   mResumeAtSampleOffset = -1.0f;
   _play();
}

SFXVoice::~SFXVoice()
{
   mOpenAL.alDeleteSources(1, &mSourceName);
}

const SFXFormat & SFXVoice::getFormat()
{
   return mBuffer->getFormat();
}

U32 SFXVoice::getPosition() const
{
   // When stopped, always return 0.

   if (getStatus() == SFXStatusStopped)
      return 0;

   // It depends on the device if and when it will return a count of the total samples
   // played so far.  With streaming buffers, all devices will do that.  With non-streaming
   // buffers, some may do for looping voices thus returning a number that exceeds the actual
   // source stream size.  So, clamp things into range here and also take care of any offsetting
   // resulting from a setPosition() call.

   U32 pos = _tell() + mOffset;
   const U32 numStreamSamples = mBuffer->getFormat().getSampleCount(mBuffer->getDuration());

   if (mBuffer->mIsLooping)
      pos %= numStreamSamples;
   else if (pos > numStreamSamples)
   {
      // Ensure we never report out-of-range positions even if the device does.

      pos = numStreamSamples;
   }

   return pos;
}

void SFXVoice::setPosition(U32 inSample)
{
   // Clamp to sample range.
   const U32 sample = inSample % (mBuffer->getFormat().getSampleCount(mBuffer->getDuration()) - 1);

   // Don't perform a seek when we already are at the
   // given position.  Especially avoids a costly stream
   // clone when seeking on a streamed voice.

   if (getPosition() == sample)
      return;

   if (!mBuffer->isStreaming())
   {
      // Non-streaming sound.  Just seek in the device buffer.

      _seek(sample);
   }
   else
   {
      // Streaming sound.  Reset the stream and playback.
      //
      // Unfortunately, the logic here is still prone to subtle timing dependencies
      // in relation to the buffer updates.  In retrospect, I feel that solving all issues
      // of asynchronous operation on a per-voice/buffer level has greatly complicated
      // the system.  It seems now that it would have been a lot simpler to have a single
      // asynchronous buffer/voice manager that manages the updates of all voices and buffers
      // currently in the system in one spot.  Packet reads could still be pushed out to
      // the thread pool but queue updates would all be handled centrally in one spot.  This
      // would do away with problems like those (mostly) solved by the multi-step procedure
      // here.

      // Go into transition.

      SFXStatus oldStatus;
      while (true)
      {
         oldStatus = mStatus;
         if (oldStatus != SFXStatusTransition)
            break;
      }

      // Switch the stream.

      _resetStream(sample, false);

      // Come out of transition.

      SFXStatus newStatus = oldStatus;
      if (oldStatus == SFXStatusPlaying)
         newStatus = SFXStatusBlocked;
   }
}

void SFXVoice::setMinMaxDistance(F32 min, F32 max)
{
   mOpenAL.alSourcef(mSourceName, AL_REFERENCE_DISTANCE, min);
   mOpenAL.alSourcef(mSourceName, AL_MAX_DISTANCE, max);
}

SFXStatus SFXVoice::getStatus() const
{
   // Detect when the device has finished playback.  Only for
   // non-streaming voices.  For streaming voices, we rely on
   // the buffer to send us a STATUS_AtEnd signal when it is
   // done playing.

   if (mStatus == SFXStatusPlaying &&
      !mBuffer->isStreaming() &&
      _status() == SFXStatusStopped)
      mStatus = SFXStatusStopped;

   return mStatus;
}

void SFXVoice::play(bool looping)
{
   mOpenAL.alSourceStop(mSourceName);
   if (!mBuffer->isStreaming())
      mOpenAL.alSourcei(mSourceName, AL_LOOPING, (looping ? AL_TRUE : AL_FALSE));

      //For streaming, check whether we have played previously.
      // If so, reset the buffer's stream.

      if (mBuffer->isStreaming() &&
         mStatus == SFXStatusStopped)
         _resetStream(0);

   // Now switch state.

   while (mStatus != SFXStatusPlaying &&
      mStatus != SFXStatusBlocked)
   {

      if ((mStatus = SFXStatusNull) ||
         ((mStatus, SFXStatusStopped) ||
         ((mStatus, SFXStatusPaused))))
      {
         _play();

         break;
      }
   }
}

void SFXVoice::stop()
{
   while (mStatus != SFXStatusStopped &&
      mStatus != SFXStatusNull)
   {
      _stop();
      break;
   }
}

void SFXVoice::pause()
{
   while (mStatus != SFXStatusPaused &&
      mStatus != SFXStatusNull &&
      mStatus != SFXStatusStopped)
   {
      _pause();
      break;
   }
}

void SFXVoice::setVelocity(const ALfloat* velocity)
{
   
   mOpenAL.alSourcefv(mSourceName, AL_VELOCITY, velocity);
}

void SFXVoice::setTransform(const MatrixF & transform)
{
   Point3F pos, dir;
   transform.getColumn(3, &pos);
   transform.getColumn(1, &dir);

   mOpenAL.alSourcefv(mSourceName, AL_POSITION, pos);
   mOpenAL.alSourcefv(mSourceName, AL_DIRECTION, dir);
}

void SFXVoice::setVolume(F32 volume)
{
   mOpenAL.alSourcef(mSourceName, AL_GAIN, volume);
}

void SFXVoice::setPitch(F32 pitch)
{
   mOpenAL.alSourcef(mSourceName, AL_PITCH, pitch);
}

void SFXVoice::setCone(F32 innerAngle, F32 outerAngle, F32 outerVolume)
{
   mOpenAL.alSourcef(mSourceName, AL_CONE_INNER_ANGLE, innerAngle);
   mOpenAL.alSourcef(mSourceName, AL_CONE_OUTER_ANGLE, outerAngle);
   mOpenAL.alSourcef(mSourceName, AL_CONE_OUTER_GAIN, outerVolume);
}

void SFXVoice::setRolloffFactor(F32 factor)
{
   mOpenAL.alSourcef(mSourceName, AL_ROLLOFF_FACTOR, factor);
}
