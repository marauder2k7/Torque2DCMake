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

//--------------------------------------
// audioStreamSource.cc
// implementation of streaming audio source
//
// Kurtis Seebaldt
//--------------------------------------

#include "sfx/sfxWavStreamSource.h"

#define BUFFERSIZE 32768

typedef struct
{
   ALubyte		riff[4];		// 'RIFF'
   ALsizei		riffSize;
   ALubyte		wave[4];		// 'WAVE'
   ALubyte		fmt[4];			// 'fmt '
   ALuint		fmtSize;
   ALushort	Format;
   ALushort	Channels;
   ALuint		SamplesPerSec;
   ALuint		BytesPerSec;
   ALushort	BlockAlign;
   ALushort	BitsPerSample;
   ALubyte		data[4];		// 'data'
   ALuint		dataSize;
} WAVE_Struct;

/// WAV File-header
struct WAVFileHdr
{
   ALubyte  id[4];
   ALsizei  size;
   ALubyte  type[4];
};

//// WAV Fmt-header
struct WAVFmtHdr
{
   ALushort format;
   ALushort channels;
   ALuint   samplesPerSec;
   ALuint   bytesPerSec;
   ALushort blockAlign;
   ALushort bitsPerSample;
};

/// WAV FmtEx-header
struct WAVFmtExHdr
{
   ALushort size;
   ALushort samplesPerBlock;
};

/// WAV Smpl-header
struct WAVSmplHdr
{
   ALuint   manufacturer;
   ALuint   product;
   ALuint   samplePeriod;
   ALuint   note;
   ALuint   fineTune;
   ALuint   SMPTEFormat;
   ALuint   SMPTEOffest;
   ALuint   loops;
   ALuint   samplerData;
   struct
   {
      ALuint identifier;
      ALuint type;
      ALuint start;
      ALuint end;
      ALuint fraction;
      ALuint count;
   }      loop[1];
};

/// WAV Chunk-header
struct WAVChunkHdr
{
   ALubyte  id[4];
   ALuint   size;
};



SFXWavStreamSource::SFXWavStreamSource(
   const OPENALFNTABLE &oalft,
   const char *filename)  
   : mOpenAL(oalft)
{
   stream = NULL;
   bIsValid = false;
   bBuffersAllocated = false;
   mBufferList[0] = 0;
   clear();

   mFilename = filename;
   mPosition = Point3F(0.f, 0.f, 0.f);
}

SFXWavStreamSource::~SFXWavStreamSource() {
   if (bReady && bIsValid)
      freeStream();
}

void SFXWavStreamSource::clear()
{
   if (stream)
      freeStream();

   //mHandle = NULL_AUDIOHANDLE;
   mSource = 0;

   if (mBufferList[0] != 0)
      mOpenAL.alDeleteBuffers(NUMBUFFERS, mBufferList);
   for (int i = 0; i < NUMBUFFERS; i++)
      mBufferList[i] = 0;

   dMemset(&mDescription, 0, sizeof(SFXDevice::DefDescription));
   mEnvironment = 0;
   mPosition.set(0.f, 0.f, 0.f);
   mDirection.set(0.f, 1.f, 0.f);
   mPitch = 1.f;
   mScore = 0.f;
   mCullTime = 0;

   bReady = false;
   bFinishedPlaying = false;
   bIsValid = false;
   bBuffersAllocated = false;
}

bool SFXWavStreamSource::initStream() {
   WAVChunkHdr chunkHdr;
   WAVFileHdr  fileHdr;
   WAVFmtHdr   fmtHdr;

   ALint			error;

   bFinished = false;

   char   data[BUFFERSIZE];

   mOpenAL.alSourceStop(mSource);
   mOpenAL.alSourcei(mSource, AL_BUFFER, 0);

   stream = ResourceManager->openStream(mFilename);
   if (stream != NULL) {
      stream->read(4, &fileHdr.id[0]);
      stream->read(&fileHdr.size);
      stream->read(4, &fileHdr.type[0]);

      stream->read(4, &chunkHdr.id[0]);
      stream->read(&chunkHdr.size);

      // WAV Format header
      stream->read(&fmtHdr.format);
      stream->read(&fmtHdr.channels);
      stream->read(&fmtHdr.samplesPerSec);
      stream->read(&fmtHdr.bytesPerSec);
      stream->read(&fmtHdr.blockAlign);
      stream->read(&fmtHdr.bitsPerSample);

      format = (fmtHdr.channels == 1 ?
         (fmtHdr.bitsPerSample == 8 ? AL_FORMAT_MONO8 : AL_FORMAT_MONO16) :
         (fmtHdr.bitsPerSample == 8 ? AL_FORMAT_STEREO8 : AL_FORMAT_STEREO16));
      freq = fmtHdr.samplesPerSec;

      stream->read(4, &chunkHdr.id[0]);
      stream->read(&chunkHdr.size);

      DataSize = chunkHdr.size;
      DataLeft = DataSize;
      dataStart = stream->getPosition();

      // Clear Error Code
      mOpenAL.alGetError();

      mOpenAL.alGenBuffers(NUMBUFFERS, mBufferList);
      if ((error = mOpenAL.alGetError()) != AL_NO_ERROR) {
         return false;
      }

      bBuffersAllocated = true;

      int numBuffers = 0;
      for (int loop = 0; loop < NUMBUFFERS; loop++)
      {
         ALuint DataToRead = (DataLeft > BUFFERSIZE) ? BUFFERSIZE : DataLeft;

         if (DataToRead == DataLeft)
            bFinished = AL_TRUE;
         stream->read(DataToRead, data);
         DataLeft -= DataToRead;
         mOpenAL.alBufferData(mBufferList[loop], format, data, DataToRead, freq);
         if ((error = mOpenAL.alGetError()) != AL_NO_ERROR) {
            return false;
         }
         ++numBuffers;
         if (bFinished)
            break;
      }

      // Queue the buffers on the source
      mOpenAL.alSourceQueueBuffers(mSource, numBuffers, mBufferList);
      if ((error = mOpenAL.alGetError()) != AL_NO_ERROR) {
         return false;
      }

      buffersinqueue = numBuffers;
      mOpenAL.alSourcei(mSource, AL_LOOPING, AL_FALSE);
      bReady = true;
   }
   else {
      return false;
   }

   bIsValid = true;

   return true;
}

bool SFXWavStreamSource::updateBuffers() {

   ALint			processed;
   ALuint			BufferID;
   ALint			error;
   char data[BUFFERSIZE];

   // don't do anything if buffer isn't initialized
   if (!bIsValid)
      return false;

   if (bFinished && mDescription.mIsLooping) {
      resetStream();
   }

   // reset AL error code
   mOpenAL.alGetError();

   // Get status
   mOpenAL.alGetSourcei(mSource, AL_BUFFERS_PROCESSED, &processed);

   // If some buffers have been played, unqueue them and load new audio into them, then add them to the queue
   if (processed > 0)
   {

      while (processed)
      {
         mOpenAL.alSourceUnqueueBuffers(mSource, 1, &BufferID);
         if ((error = mOpenAL.alGetError()) != AL_NO_ERROR)
            return false;

         if (!bFinished)
         {
            ALuint DataToRead = (DataLeft > BUFFERSIZE) ? BUFFERSIZE : DataLeft;

            if (DataToRead == DataLeft) {
               bFinished = AL_TRUE;
            }

            stream->read(DataToRead, data);
            DataLeft -= DataToRead;

            mOpenAL.alBufferData(BufferID, format, data, DataToRead, freq);
            if ((error = mOpenAL.alGetError()) != AL_NO_ERROR)
               return false;

            // Queue buffer
            mOpenAL.alSourceQueueBuffers(mSource, 1, &BufferID);
            if ((error = mOpenAL.alGetError()) != AL_NO_ERROR)
               return false;

            processed--;

            if (bFinished && mDescription.mIsLooping) {
               resetStream();
            }
         }
         else
         {
            buffersinqueue--;
            processed--;

            if (buffersinqueue == 0)
            {
               bFinishedPlaying = AL_TRUE;
               return AL_FALSE;
            }
         }
      }
   }

   return AL_TRUE;
}

void SFXWavStreamSource::freeStream() {
   bReady = false;


   if (stream != NULL)
      ResourceManager->closeStream(stream);
   stream = NULL;

   if (bBuffersAllocated) {
      if (mBufferList[0] != 0)
         mOpenAL.alDeleteBuffers(NUMBUFFERS, mBufferList);
      for (int i = 0; i < NUMBUFFERS; i++)
         mBufferList[i] = 0;

      bBuffersAllocated = false;
   }
}

void SFXWavStreamSource::resetStream() {
   stream->setPosition(dataStart);
   DataLeft = DataSize;
   bFinished = AL_FALSE;
}

#include "console/console.h"

F32 SFXWavStreamSource::getElapsedTime()
{
   Con::warnf("GetElapsedTime not implemented in WaveStreams yet");
   return -1.f;
}

F32 SFXWavStreamSource::getTotalTime()
{
   Con::warnf("GetTotalTime not implemented in WaveStreams yet");
   return -1.f;
}
