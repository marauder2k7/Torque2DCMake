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

#ifndef _SFXBUFFER_H_
#define _SFXBUFFER_H_

#ifndef _LOADOAL_H_
#include "sfx/loadOal.h"
#endif // !_LOADOAL_H_

#ifndef _RESMANAGER_H_
#include "io/resource/resourceManager.h"
#endif

#ifndef _SFXCOMMON_H_
#include "sfx/sfxCommon.h"
#endif

//--------------------------------------------------------------------------

class SFXBuffer : public ResourceInstance
{
   friend class AudioThread;
protected:
   StringTableEntry  mFilename;
   bool              mLoading;
   ALuint            malBuffer;

   SFXBuffer(const OPENALFNTABLE &oalft,
             StringTableEntry filename);

   const OPENALFNTABLE &mOpenAL;

   bool readRIFFchunk(Stream &s, const char *seekLabel, U32 *size);
   bool readWAV(ResourceObject *obj);

#ifndef TORQUE_OS_IOS
   bool readOgg(ResourceObject *obj);
   long oggRead(struct OggVorbis_File* vf, char *buffer, int length, int bigendianp, int *bitstream);
#endif

public:
   static SFXBuffer* create(const OPENALFNTABLE &oalft,
                            StringTableEntry filename);
   ~SFXBuffer();
   ALuint getALBuffer();
   bool isLoading() { return(mLoading); }

   static Resource<SFXBuffer> find(const OPENALFNTABLE &oalft, const char *filename);
   static ResourceInstance* construct(Stream& stream);

};


#endif  // _SFXBUFFER_H_
