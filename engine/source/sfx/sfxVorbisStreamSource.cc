//--------------------------------------
// vorbisStreamSource.cc
// implementation of streaming audio source
//
// Kurtis Seebaldt
//--------------------------------------

#include "console/console.h"
#include "sfx/sfxCommon.h"

#include "sfx/sfxVorbisStreamSource.h"

#define BUFFERSIZE 32768
#define CHUNKSIZE 4096

#if defined(TORQUE_BIG_ENDIAN)
#define ENDIAN 1
#else
#define ENDIAN 0
#endif


static size_t _ov_read_func(void *ptr, size_t size, size_t nmemb, void *datasource)
{
   Stream *stream = reinterpret_cast<Stream*>(datasource);

   // Stream::read() returns true if any data was
   // read, so we must track the read bytes ourselves.
   U32 startByte = stream->getPosition();
   stream->read(size * nmemb, ptr);
   U32 endByte = stream->getPosition();

   // How many did we actually read?
   U32 readBytes = (endByte - startByte);
   U32 readItems = readBytes / size;

   return readItems;
}

static int _ov_seek_func(void *datasource, ogg_int64_t offset, int whence)
{
   Stream *stream = reinterpret_cast<Stream*>(datasource);

   U32 newPos = 0;
   if (whence == SEEK_CUR)
      newPos = stream->getPosition() + (U32)offset;
   else if (whence == SEEK_END)
      newPos = stream->getStreamSize() - (U32)offset;
   else
      newPos = (U32)offset;

   return stream->setPosition(newPos) ? 0 : -1;
}

static long _ov_tell_func(void *datasource)
{
   Stream *stream = reinterpret_cast<Stream*>(datasource);
   return stream->getPosition();
}


SFXVorbisStreamSource::SFXVorbisStreamSource(const OPENALFNTABLE &oalft,
   const char *filename)
   : mOpenAL(oalft)
{
   stream = NULL;
   bIsValid = false;
   bBuffersAllocated = false;
   bVorbisFileInitialized = false;
   mBufferList[0] = 0;
   clear();

   mFilename = filename;
   mPosition = Point3F(0.f, 0.f, 0.f);
}

SFXVorbisStreamSource::~SFXVorbisStreamSource()
{
   if (bReady && bIsValid)
      freeStream();
}

void SFXVorbisStreamSource::clear()
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
   bVorbisFileInitialized = false;
}

bool SFXVorbisStreamSource::initStream()
{
   vorbis_info *vi;

   ALint error;

   bFinished = false;

   // CSH 11/1/15 - Page 404's, not sure what bug this was working around??
   // JMQ: changed buffer to static and doubled size.  workaround for
   // https://206.163.64.242/mantis/view_bug_page.php?f_id=0000242
   static char data[BUFFERSIZE * 2];

   mOpenAL.alSourceStop(mSource);
   mOpenAL.alSourcei(mSource, AL_BUFFER, 0);

   stream = ResourceManager->openStream(mFilename);
   if (stream != NULL)
   {
      const bool canSeek = stream->hasCapability(Stream::StreamPosition);

      ov_callbacks cb;
      cb.read_func = _ov_read_func;
      cb.seek_func = canSeek ? _ov_seek_func : NULL;
      cb.close_func = NULL;
      cb.tell_func = canSeek ? _ov_tell_func : NULL;

      S32 ret = ov_open_callbacks(stream, &vf, NULL, 0, cb);
      if (ret < 0)
      {
         const char* detail = "";
         if (ret == OV_ENOTVORBIS)
            detail = "; not a Vorbis file";
         else if (ret == OV_EVERSION)
            detail = "; bad version";
         else if (ret == OV_EBADHEADER)
            detail = "; bad header";

         Con::errorf("Could not open '%s' for streaming%s", mFilename, detail);

         return false;
      }

      bVorbisFileInitialized = true;

      //Read Vorbis File Info
      vi = ov_info(&vf, -1);
      freq = vi->rate;

      long samples = (long)ov_pcm_total(&vf, -1);

      if (vi->channels == 1)
      {
         format = AL_FORMAT_MONO16;
         DataSize = 2 * samples;
      }
      else
      {
         format = AL_FORMAT_STEREO16;
         DataSize = 4 * samples;
      }
      DataLeft = DataSize;

      // Clear Error Code
      mOpenAL.alGetError();

      mOpenAL.alGenBuffers(NUMBUFFERS, mBufferList);
      if ((error = mOpenAL.alGetError()) != AL_NO_ERROR)
         return false;

      bBuffersAllocated = true;

      int numBuffers = 0;
      for (int loop = 0; loop < NUMBUFFERS; loop++)
      {
         ALuint DataToRead = (DataLeft > BUFFERSIZE) ? BUFFERSIZE : DataLeft;
         if (DataToRead == DataLeft)
            bFinished = AL_TRUE;

         long ret = oggRead(data, BUFFERSIZE, ENDIAN, &current_section);
         if (ret <= 0)
         {
            bFinished = AL_TRUE;
            break;
         }

         DataLeft -= ret;
         mOpenAL.alBufferData(mBufferList[loop], format, data, ret, freq);
         ++numBuffers;

         if ((error = mOpenAL.alGetError()) != AL_NO_ERROR)
            return false;
         if (bFinished)
            break;
      }

      // Queue the buffers on the source
      mOpenAL.alSourceQueueBuffers(mSource, NUMBUFFERS, mBufferList);
      if ((error = mOpenAL.alGetError()) != AL_NO_ERROR)
         return false;

      mOpenAL.alSourcei(mSource, AL_LOOPING, AL_FALSE);
      bReady = true;
   }
   else
   {
      return false;
   }
   bIsValid = true;

   return true;
}

bool SFXVorbisStreamSource::updateBuffers()
{
   ALint         processed;
   ALuint         BufferID;
   ALint         error;

   // CSH 11/1/15 - Page 404's, not sure what bug this was working around??
   // JMQ: changed buffer to static and doubled size.  workaround for
   // https://206.163.64.242/mantis/view_bug_page.php?f_id=0000242
   static char data[BUFFERSIZE * 2];

   // don't do anything if stream not loaded properly
   if (!bIsValid)
      return false;

   if (bFinished && mDescription.mIsLooping)
      resetStream();

   // reset AL error code
   mOpenAL.alGetError();

#if 1 //def TORQUE_OS_LINUX
   // JMQ: this doesn't really help on linux.  it may make things worse.
   // if it doesn't help on mac/win either, could disable it.
   ALint state;
   mOpenAL.alGetSourcei(mSource, AL_SOURCE_STATE, &state);
   if (state == AL_STOPPED)
   {
      // um, yeah.  you should be playing
      // restart
      mOpenAL.alSourcePlay(mSource);
      //#ifdef TORQUE_DEBUG
      //Con::errorf(">><<>><< THE MUSIC STOPPED >><<>><<");
      //#endif
      return true;
   }
#endif


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

            if (DataToRead == DataLeft)
               bFinished = AL_TRUE;

            long ret = oggRead(data, BUFFERSIZE, ENDIAN, &current_section);
            if (ret > 0)
            {
               DataLeft -= ret;

               mOpenAL.alBufferData(BufferID, format, data, ret, freq);
               if ((error = mOpenAL.alGetError()) != AL_NO_ERROR)
                  return false;

               // Queue buffer
               mOpenAL.alSourceQueueBuffers(mSource, 1, &BufferID);
               if ((error = mOpenAL.alGetError()) != AL_NO_ERROR)
                  return false;
            }

            processed--;

            if (bFinished && mDescription.mIsLooping)
            {
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

   return true;
}

void SFXVorbisStreamSource::freeStream()
{
   bReady = false;

   if (stream != NULL)
      ResourceManager->closeStream(stream);

   stream = NULL;

   if (bBuffersAllocated)
   {
      if (mBufferList[0] != 0)
         mOpenAL.alDeleteBuffers(NUMBUFFERS, mBufferList);
      for (int i = 0; i < NUMBUFFERS; i++)
         mBufferList[i] = 0;

      bBuffersAllocated = false;
   }

   if (bVorbisFileInitialized)
   {
      ov_clear(&vf);
      bVorbisFileInitialized = false;
   }
}

void SFXVorbisStreamSource::resetStream()
{
   ov_pcm_seek(&vf, 0);
   DataLeft = DataSize;
   bFinished = AL_FALSE;
}

void SFXVorbisStreamSource::setNewFile(const char * file)
{
   //---------------------
   // close down old file...
   //---------------------

   if (stream != NULL)
   {
      ResourceManager->closeStream(stream);
      stream = NULL;
   }

   if (bVorbisFileInitialized)
   {
      ov_clear(&vf);
      bVorbisFileInitialized = false;
   }

   //---------------------
   // open up new file...
   //---------------------

   mFilename = file;
   stream = ResourceManager->openStream(mFilename);
   if (stream != NULL)
   {
      const bool canSeek = stream->hasCapability(Stream::StreamPosition);

      ov_callbacks cb;
      cb.read_func = _ov_read_func;
      cb.seek_func = canSeek ? _ov_seek_func : NULL;
      cb.close_func = NULL;
      cb.tell_func = canSeek ? _ov_tell_func : NULL;

      S32 ret = ov_open_callbacks(stream, &vf, NULL, 0, cb);

      if (ret < 0)
      {
         bFinished = AL_TRUE;
         bVorbisFileInitialized = false;
         bIsValid = false;
         return;
      }

      //Read Vorbis File Info
      vorbis_info * vi = ov_info(&vf, -1);
      freq = vi->rate;

      long samples = (long)ov_pcm_total(&vf, -1);

      if (vi->channels == 1)
      {
         format = AL_FORMAT_MONO16;
         DataSize = 2 * samples;
      }
      else
      {
         format = AL_FORMAT_STEREO16;
         DataSize = 4 * samples;
      }
      DataLeft = DataSize;

      // Clear Error Code
      mOpenAL.alGetError();

      bFinished = AL_FALSE;
      bVorbisFileInitialized = true;
      bIsValid = true;
   }
}

// ov_read() only returns a maximum of one page worth of data
// this helper function will repeat the read until buffer is full
long SFXVorbisStreamSource::oggRead(char *buffer, int length,
   int bigendianp, int *bitstream)
{
   long bytesRead = 0;
   long totalBytes = 0;
   long offset = 0;
   long bytesToRead = 0;

   while ((offset) < length)
   {
      if ((length - offset) < CHUNKSIZE)
         bytesToRead = length - offset;
      else
         bytesToRead = CHUNKSIZE;

      bytesRead = ov_read(&vf, (char*)buffer, bytesToRead, bigendianp, 2, 1, bitstream);
      //#ifdef TORQUE_OS_LINUX
#if 1 // Might fix mac audio issue and possibly others...based on references, this looks like correct action
      // linux ver will hang on exit after a stream loop if we don't
      // do this
      if (bytesRead == OV_HOLE)
         // retry, see:
         // http://www.xiph.org/archives/vorbis-dev/200102/0163.html
         // http://www.mit.edu/afs/sipb/user/xiphmont/ogg-sandbox/vorbis/doc/vorbis-errors.txt
         continue;
#endif

      if (bytesRead <= 0)
         break;
      offset += bytesRead;
      buffer += bytesRead;
   }
   return offset;
}

F32 SFXVorbisStreamSource::getElapsedTime()
{
   return ov_time_tell(&vf);
}

F32 SFXVorbisStreamSource::getTotalTime()
{
   return ov_time_total(&vf, -1);
}
