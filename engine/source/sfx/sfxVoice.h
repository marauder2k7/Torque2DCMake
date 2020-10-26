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

#ifndef _SFXVOICE_H_
#define _SFXVOICE_H_

#ifndef _OPENALFNTABLE
#include "sfx/LoadOAL.h"
#endif
#ifndef _PLATFORM_THREADS_MUTEX_H_
#include "platform/threads/mutex.h"
#endif

#include "collection/refBase.h"

#include "2d/core/Vector2.h"

#include "sfx/sfxCommon.h"

class SFXBuffer;
class SFXDevice;

class SFXVoice
{
public:

   friend class SFXDevice;
   friend class SFXBuffer;

protected:

   SFXVoice(const OPENALFNTABLE &oalft,
      SFXBuffer *buffer,
      ALuint sourceName);

   ALuint mSourceName;

   /// Buggy OAL jumps around when pausing.  Save playback cursor here.
   F32 mResumeAtSampleOffset;
   mutable volatile SFXStatus mStatus;
   /// Amount by which OAL's reported sample position is offset.
   ///
   /// OAL's sample position is relative to the current queue state,
   /// so we manually need to keep track of how far into the total
   /// queue we are.
   U32 mSampleOffset;

   WeakRefPtr< SFXBuffer > mBuffer;

   Mutex mMutex;

   /// For streaming voices, this keeps track of play start offset
   /// after seeking.  Expressed in number of samples.
   U32 mOffset;

   const OPENALFNTABLE &mOpenAL;

   ///
   SFXBuffer* _getBuffer() const
   {
      return (SFXBuffer*)mBuffer.getPointer();
   }

   /// For non-streaming buffers, late-bind the audio buffer
   /// to the source as OAL will not accept writes to buffers
   /// already bound.
   void _lateBindStaticBufferIfNecessary();

   // SFXVoice.
   SFXStatus _status() const;
   void _play();
   void _pause();
   void _stop();
   void _seek(U32 sample);
   U32 _tell() const;
   void _resetStream(U32 sampleStartPos, bool triggerUpdate = true);

public:

   static Vector< void(SFXVoice*) > smVoiceCreatedSignal;
   static Vector< void(SFXVoice*) > smVoiceDestroyedSignal;

   static SFXVoice* create(SFXDevice* device,
      SFXBuffer *buffer);

   ~SFXVoice();

   /// SFXVoice
   void setMinMaxDistance(F32 min, F32 max);
   void play(bool looping);
   void setVelocity(const ALfloat* velocity);
   void setTransform(const MatrixF& transform);
   void setVolume(F32 volume);
   void setPitch(F32 pitch);
   void setCone(F32 innerAngle, F32 outerAngle, F32 outerVolume);
   void setRolloffFactor(F32 factor);
};

#endif // _SFXALVOICE_H_