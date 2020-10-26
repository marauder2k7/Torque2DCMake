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

#ifndef _SFXALBUFFER_H_
#define _SFXALBUFFER_H_

#ifndef _LOADOAL_H
#include "sfx/LoadOAL.h"
#endif
#include "sfx/sfxVoice.h"
#include "sfx/sfxDescription.h"
#include "collection/refBase.h"
#include "sfx/sfxStream.h"

#ifndef _TVECTOR_H_
#include "collection/vector.h"
#endif
#ifndef _RESMANAGER_H_
#include "io/resource/resourceManager.h"
#endif

#include "io/tStream.h"

class SFXVoice;


class SFXBuffer : public ResourceInstance
{
public:

   friend class SFXDevice;
   friend class SFXVoice;

   /// Status indicators for sound buffers.
   enum Status
   {
      STATUS_Null,      ///< Initial state.
      STATUS_Loading,   ///< Buffer has requested data and is waiting for queue to fill up.
      STATUS_Ready,     ///< Playback queue is fed and ready (non-stream buffers will stop at this state).
      STATUS_Blocked,   ///< Queue is starved and playback thus held until further data is available (streaming buffers only).
      STATUS_AtEnd,     ///< Buffer has read all its streaming data (streaming buffers only).
   };

protected:

   StringTableEntry  mFilename;
   bool              mLoading;
   ALuint            malBuffer;

   Vector<ALuint> mFreeBuffers;

   SFXBuffer(  const OPENALFNTABLE &oalft,
               const SFXStream &stream,
               SFXDescription* description,
               bool useHardware);

   bool mIs3d;

   bool mUseHardware;

   /// The buffer readiness status.
   Status mStatus;

   /// The sound sample format used by the buffer.
   SFXFormat mFormat;

   /// Total playback time of the associated sound stream in milliseconds.
   /// @note For streaming buffers, this will not correspond to the actual
   ///   playtime of the device buffer.
   U32 mDuration;

   /// If true, this is a continuously streaming buffer.
   bool mIsStreaming;

   /// For streaming buffers, tells whether the source stream loops.
   bool mIsLooping;

   /// If true, this buffer can only have a single SFXVoice attached.
   bool mIsUnique;

   /// If true, the buffer is dead and will be deleted.  Can't be in status
   /// for synchronization reasons.
   bool mIsDead;

   const OPENALFNTABLE &mOpenAL;

   static ALenum _sfxFormatToALFormat(const SFXFormat& format)
   {
      if (format.getChannels() == 2)
      {
         const U32 bps = format.getBitsPerSample();
         if (bps == 16)
            return AL_FORMAT_STEREO8;
         else if (bps == 32)
            return AL_FORMAT_STEREO16;
      }
      else if (format.getChannels() == 1)
      {
         const U32 bps = format.getBitsPerSample();
         if (bps == 8)
            return AL_FORMAT_MONO8;
         else if (bps == 16)
            return AL_FORMAT_MONO16;
      }
      return 0;
   }

   StrongWeakRefPtr< SFXVoice > mUniqueVoice;
   ///
   SFXVoice* _getUniqueVoice() const
   {
      return (SFXVoice*)mUniqueVoice.getPointer();
   }

   /// Set the buffer status and trigger mOnStatusChange if the status changes.
      /// @note Called on both the SFX update thread and the main thread.
   void _setStatus(Status status);

   void _flush();

   bool readRIFFchunk(Stream &s, const char *seekLabel, U32 *size);
   bool readWAV(ResourceObject *obj);

#ifndef TORQUE_OS_IOS
   bool readOgg(ResourceObject *obj);
   long oggRead(struct OggVorbis_File* vf, char *buffer, int length, int bigendianp, int *bitstream);
#endif

public:

   /// @return The current buffer loading/queue status.
   Status getStatus() const { return mStatus; }

   /// @return The sound sample format used by the buffer.
   const SFXFormat& getFormat() const { return mFormat; }

   /// @return The total playback time of the buffer in milliseconds.
   U32 getDuration() const { return mDuration; }

   /// @return The total number of samples in the buffer.
   U32 getNumSamples() const { return getFormat().getSampleCount(mDuration); }

   /// @return The number of bytes consumed by this sound buffer.
   virtual U32 getMemoryUsed() const { return 0; }

   /// @return True if the buffer does continuous sound streaming.
   bool isStreaming() const { return mIsStreaming; }

   /// @return True if the buffer is pending deletion.
   bool isDead() const { return mIsDead; }

   /// @return True if the buffer's packet queue is loaded and ready for playback.
   bool isReady() const { return (getStatus() == STATUS_Ready); }

   /// @return True if the buffer's packet queue is still loading.
   bool isLoading() const { return (getStatus() == STATUS_Loading); }

   /// @return True if the buffer's packet queue has been starved and is waiting for data.
   bool isBlocked() const { return (getStatus() == STATUS_Blocked); }

   /// @return True if the buffer has exhausted its source stream
   bool isAtEnd() const { return (getStatus() == STATUS_AtEnd); }

   /// @return True if the buffer can only have a single SFXVoice attached to it.
   bool isUnique() const { return mIsUnique; }


   static SFXBuffer* create(const OPENALFNTABLE &oalft,
      const SFXStream &stream,
      SFXDescription* description,
      bool useHardware);

   ~SFXBuffer();
   ALuint getALBuffer();
   bool isLoading() { return(mLoading); }

};

#endif // _SFXALBUFFER_H_