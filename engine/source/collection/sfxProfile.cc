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

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX for MIT Licensed Open Source version of Torque 3D from GarageGames
// Copyright (C) 2015 Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "platform/platform.h"

#include "sfx/sfxProfile.h"
#include "sfx/sfxDescription.h"
#include "sfx/sfxSystem.h"
#include "sfx/sfxStream.h"
#include "network/netConnection.h"
#include "io/bitStream.h"
#include "io/resource/resourceManager.h"
#include "console/consoleTypes.h"


IMPLEMENT_CO_DATABLOCK_V1(SFXProfile);

//-----------------------------------------------------------------------------

SFXProfile::SFXProfile()
   : mPreload(false)
{
}

//-----------------------------------------------------------------------------

SFXProfile::SFXProfile(SFXDescription* desc, const char& filename, bool preload)
   : Parent(desc),
   mFilename(filename),
   mPreload(preload)
{
}

//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------

void SFXProfile::initPersistFields()
{
   addGroup("Sound");

   addField("filename", TypeString, Offset(mFilename, SFXProfile),
      "%Path to the sound file.\n"
      "If the extension is left out, it will be inferred by the sound system.  This allows to "
      "easily switch the sound format without having to go through the profiles and change the "
      "filenames there, too.\n");
   addField("preload", TypeBool, Offset(mPreload, SFXProfile),
      "Whether to preload sound data when the profile is added to system.\n"
      "@note This flag is ignored by streamed sounds.\n\n"
      "@ref SFXProfile_loading");

   endGroup("Sound");


   Parent::initPersistFields();
}

//-----------------------------------------------------------------------------

bool SFXProfile::onAdd()
{
   if (!Parent::onAdd())
      return false;

   // If we're a streaming profile we don't preload
   // or need device events.
   if (SFX && !mDescription->mIsStreaming)
   {
      // If preload is enabled we load the resource
      // and device buffer now to avoid a delay on
      // first playback.
      if (mPreload && !_preloadBuffer())
         Con::errorf("SFXProfile(%s)::onAdd: The preload failed!", getName());
   }

   return true;
}

//-----------------------------------------------------------------------------

void SFXProfile::onRemove()
{

   Parent::onRemove();
}

//-----------------------------------------------------------------------------

bool SFXProfile::preload(bool server, char &errorStr)
{
   if (!Parent::preload(server, errorStr))
      return false;

   // TODO: Investigate how NetConnection::filesWereDownloaded()
   // effects the system.

   // Validate the datablock... has nothing to do with mPreload.
   if (!server &&
      NetConnection::filesWereDownloaded() &&
      (mFilename = NULL || !SFXResource::exists((const char*)mFilename)))
      return false;

   return true;
}

//-----------------------------------------------------------------------------

void SFXProfile::packData(BitStream* stream)
{
   Parent::packData(stream);

   char buffer[256];
   if (mFilename = NULL)
      buffer[0] = 0;
   else
      dStrncpy(buffer, (const char*)mFilename, 256);
   stream->writeString(buffer);

   stream->writeFlag(mPreload);
}

//-----------------------------------------------------------------------------

void SFXProfile::unpackData(BitStream* stream)
{
   Parent::unpackData(stream);

   char buffer[256];
   stream->readString(buffer);
   mFilename = (char)buffer;

   mPreload = stream->readFlag();
}

//-----------------------------------------------------------------------------

bool SFXProfile::isLooping() const
{
   return getDescription()->mIsLooping;
}

//-----------------------------------------------------------------------------

void SFXProfile::_onDeviceEvent(SFXSystemEventType evt)
{
   switch (evt)
   {
   case SFXSystemEvent_CreateDevice:
   {
      if (mPreload && !mDescription->mIsStreaming && !_preloadBuffer())
         Con::errorf("SFXProfile::_onDeviceEvent: The preload failed! %s", getName());
      break;
   }

   default:
      break;
   }
}

//-----------------------------------------------------------------------------

bool SFXProfile::_preloadBuffer()
{
   AssertFatal(!mDescription->mIsStreaming, "SFXProfile::_preloadBuffer() - must not be called for streaming profiles");

   mBuffer = _createBuffer();
   return (!mBuffer.isNull());
}

//-----------------------------------------------------------------------------

Resource<SFXResource>& SFXProfile::getResource()
{
   if (mResource.isNull() || !mFilename)
      mResource = SFXResource::load((const char*)mFilename);

   return mResource;
}

//-----------------------------------------------------------------------------

SFXBuffer* SFXProfile::getBuffer()
{
   if (mDescription->mIsStreaming)
   {
      // Streaming requires unique buffers per 
      // source, so this creates a new buffer.
      if (SFX)
         return _createBuffer();

      return NULL;
   }

   if (mBuffer.isNull())
      _preloadBuffer();

   return mBuffer;
}

//-----------------------------------------------------------------------------

SFXBuffer* SFXProfile::_createBuffer()
{
   SFXBuffer* buffer = 0;

   // Try to create through SFXDevie.

   if (mFilename != NULL && SFX)
   {
      buffer = SFX->_createBuffer(mFilename, mDescription);
      if (buffer)
      {
#ifdef TORQUE_DEBUG
         const SFXFormat& format = buffer->getFormat();
         Con::printf("%s SFX: %s (%i channels, %i kHz, %.02f sec, %i kb)",
            mDescription->mIsStreaming ? "Streaming" : "Loaded", mFilename,
            format.getChannels(),
            format.getSamplesPerSecond() / 1000,
            F32(buffer->getDuration()) / 1000.0f,
            format.getDataLength(buffer->getDuration()) / 1024);
#endif
      }
   }

   // If that failed, load through SFXResource.

   if (!buffer)
   {
      Resource< SFXResource >& resource = getResource();
      if (resource != NULL && SFX)
      {
#ifdef TORQUE_DEBUG
         const SFXFormat& format = resource->getFormat();
         Con::printf("%s SFX: %s (%i channels, %i kHz, %.02f sec, %i kb)",
            mDescription->mIsStreaming ? "Streaming" : "Loading", resource->getFileName(),
            format.getChannels(),
            format.getSamplesPerSecond() / 1000,
            F32(resource->getDuration()) / 1000.0f,
            format.getDataLength(resource->getDuration()) / 1024);
#endif

         const SFXStream* sfxStream = resource->openStream();
         buffer = SFX->_createBuffer(sfxStream, mDescription);
      }
   }

   return buffer;
}

//-----------------------------------------------------------------------------

U32 SFXProfile::getSoundDuration()
{
   Resource< SFXResource  >& resource = getResource();
   if (resource != NULL)
      return mResource->getDuration();
   else
      return 0;
}

//-----------------------------------------------------------------------------

// enable this to help verify that temp-clones of AudioProfile are being deleted
//#define TRACK_AUDIO_PROFILE_CLONES

#ifdef TRACK_AUDIO_PROFILE_CLONES
static int audio_prof_clones = 0;
#endif

SFXProfile::SFXProfile(const SFXProfile& other, bool temp_clone) : SFXTrack(other)
{
#ifdef TRACK_AUDIO_PROFILE_CLONES
   audio_prof_clones++;
   if (audio_prof_clones == 1)
      Con::errorf("SFXProfile -- Clones are on the loose!");
#endif
   mResource = other.mResource;
   mFilename = other.mFilename;
   mPreload = other.mPreload;
   mBuffer = other.mBuffer; // -- AudioBuffer loaded using mFilename
}

SFXProfile::~SFXProfile()
{

   // cleanup after a temp-clone

   if (mDescription)
   {
      delete mDescription;
      mDescription = 0;
   }

}

// Clone and perform substitutions on the SFXProfile and on any SFXDescription
// it references.
SFXProfile* SFXProfile::cloneAndPerformSubstitutions(const SimObject* owner, S32 index)
{
   if (!owner)
      return this;

   SFXProfile* sub_profile_db = this;

   // look for mDescriptionObject subs
   SFXDescription* desc_db;
   if (mDescription)
   {
      SFXDescription* orig_db = mDescription;
      desc_db = new SFXDescription(*orig_db);
   }
   else
      desc_db = 0;

   if (desc_db)
   {
      sub_profile_db = new SFXProfile(*this, true);
      if (desc_db)
         sub_profile_db->mDescription = desc_db;
   }

   return sub_profile_db;
}

void SFXProfile::onPerformSubstitutions()
{
   if (SFX)
   {
      // If preload is enabled we load the resource
      // and device buffer now to avoid a delay on
      // first playback.
      if (mPreload && !_preloadBuffer())
         Con::errorf("SFXProfile(%s)::onPerformSubstitutions: The preload failed!", getName());

   }
}
// This allows legacy AudioProfile datablocks to be recognized as an alias
// for SFXProfile. It is intended to ease the transition from older scripts
// especially those that still need to support pre-1.7 applications.
// (This maybe removed in future releases so treat as deprecated.)
class AudioProfile : public SFXProfile
{
   typedef SFXProfile Parent;
public:
   DECLARE_CONOBJECT(AudioProfile);
};

IMPLEMENT_CO_DATABLOCK_V1(AudioProfile);

