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

#ifndef _SFXWAVSTREAMSOURCE_H_
#define _SFXWAVSTREAMSOURCE_H_

#ifndef _SFXSTREAMSOURCE_H_
#include "sfx/sfxStreamSource.h"
#endif

#ifndef _LOADOAL_H_
#include "sfx/loadOal.h"
#endif // !_LOADAL_H_


class SFXWavStreamSource : public SFXStreamSource
{
public:
   SFXWavStreamSource(const OPENALFNTABLE &oalft,const char *filename);
   virtual ~SFXWavStreamSource();

   virtual bool initStream();
   virtual bool updateBuffers();
   virtual void freeStream();
   virtual F32 getElapsedTime();
   virtual F32 getTotalTime();

private:
   const OPENALFNTABLE &mOpenAL;
   ALuint      mBufferList[NUMBUFFERS];
   S32			mNumBuffers;
   S32		   mBufferSize;
   Stream	   *stream;

   bool		   bReady;
   bool			bFinished;

   ALenum      format;
   ALsizei     size;
   ALsizei     freq;

   ALuint		DataSize;
   ALuint		DataLeft;
   ALuint		dataStart;
   ALuint		buffersinqueue;

   bool			bBuffersAllocated;

   void clear();
   void resetStream();
};

#endif // _SFXWAVSTREAMSOURCE_H_
