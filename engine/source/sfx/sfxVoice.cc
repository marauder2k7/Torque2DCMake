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

void SFXVoice::setMinMaxDistance(F32 min, F32 max)
{
   mOpenAL.alSourcef(mSourceName, AL_REFERENCE_DISTANCE, min);
   mOpenAL.alSourcef(mSourceName, AL_MAX_DISTANCE, max);
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
