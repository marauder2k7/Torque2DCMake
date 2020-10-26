#include "sfx/sfxDevice.h"
#include "sfx/sfxVoice.h"
#include "sfx/sfxDescription.h"
#include "console/console.h"

#include "sfx/sfxBuffer.h"

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

bool SFXBuffer::readWAV(ResourceObject * obj)
{
   return false;
}

SFXBuffer * SFXBuffer::create(const OPENALFNTABLE &oalft,
                              StringTableEntry filename, 
                              SFXDescription* description,
                              bool useHardware)
{
   SFXBuffer *buffer = new SFXBuffer(  oalft,
                                       filename,
                                       description,
                                       useHardware);
      return buffer;
}

SFXBuffer::SFXBuffer(const OPENALFNTABLE &oalft,
                     StringTableEntry filename,
                     SFXDescription* description,
                     bool useHardware) : mIs3d(description->mIs3D),
                     mUseHardware(useHardware),
                     mOpenAL(oalft)
{
   AssertFatal(StringTable->lookup(filename), "AudioBuffer:: filename is not a string table entry");
   mFilename = filename;
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