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

#include "sfx/sfxResource.h"
#include "sfx/sfxFileStream.h"
#include "io/resource/resourceManager.h"



// Ugly workaround to keep the constructor protected.
struct SFXResource::_NewHelper
{
   static SFXResource* New(const char* fileName, SFXStream* stream)
   {
      return new SFXResource(fileName, stream);
   }
};


template<>
void* Resource< SFXResource >::create(const char* path)
{

   // Try to open the stream.
   SFXStream* stream = SFXFileStream::create(path);
   if (!stream)
      return NULL;

   // We have a valid stream... create the resource.
   SFXResource* res = SFXResource::_NewHelper::New(path, stream);

   return res;
}

Resource< SFXResource > SFXResource::load(const char* filename)
{
   return ResourceManager->load(filename);
}

SFXResource::SFXResource(const char* fileName, SFXStream *stream)
   : mFileName((char)fileName),
   mFormat(stream->getFormat()),
   mDuration(stream->getDuration())
{
}

bool SFXResource::exists(const char* filename)
{
   return SFXFileStream::exists(filename);
}

SFXStream* SFXResource::openStream()
{
   return SFXFileStream::create((const char*)mFileName);
}
