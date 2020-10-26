#include "sfx/sfxDevice.h"
#include "sfx/sfxVoice.h"
#include "sfx/sfxDescription.h"
#include "console/console.h"

#include "sfx/sfxBuffer.h"

SFXBuffer::SFXBuffer(const OPENALFNTABLE & oalft, const SFXStream * stream, SFXDescription * description, bool useHardware) :
                                                                                             mIs3d(description->mIs3D),
                                                                                             mUseHardware(useHardware),
                                                                                             mOpenAL(oalft)
{
}

void SFXBuffer::_flush()
{
   _getUniqueVoice()->_stop();

   MutexHandle mutex;
   mutex.lock(&_getUniqueVoice()->mMutex, true);

   ALuint source = _getUniqueVoice()->mSourceName;

   ALint numQueued;
   mOpenAL.alGetSourcei(source, AL_BUFFERS_QUEUED, &numQueued);

   for (U32 i = 0; i < numQueued; ++i)
   {
      ALuint buffer;
      mOpenAL.alSourceUnqueueBuffers(source, 1, &buffer);
      mFreeBuffers.push_back(buffer);
   }

   _getUniqueVoice()->mSampleOffset = 0;
}

SFXBuffer * SFXBuffer::create(const OPENALFNTABLE & oalft, const SFXStream * stream, SFXDescription * description, bool useHardware)
{
   SFXBuffer *buffer = new SFXBuffer(oalft,
                           stream,
                           description,
                           useHardware);
                        return buffer;
}

SFXBuffer::~SFXBuffer()
{
   // Release buffers.
   if (mOpenAL.alIsBuffer(malBuffer))
      mOpenAL.alDeleteBuffers(1, &malBuffer);

   while (mFreeBuffers.size())
   {
      ALuint buffer = mFreeBuffers.last();
      mOpenAL.alDeleteBuffers(1, &buffer);
      mFreeBuffers.pop_back();
   }
}