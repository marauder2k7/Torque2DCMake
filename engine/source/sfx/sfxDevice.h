#ifndef _SFXDEVICE_H_
#define _SFXDEVICE_H_

#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif

#ifdef _SFXPROVIDER_H_
#include "sfx/sfxProvider.h"
#endif

#ifndef _SFXALBUFFER_H_
#include "sfx/sfxBuffer.h"
#endif

#ifndef _SFXALVOICE_H_
#include "sfx/sfxVoice.h"
#endif

#ifndef _OPENALFNTABLE
#include "sfx/LoadOAL.h"
#endif

#ifdef _SFXSTREAM_H_
#include "sfx/sfxStream.h"
#endif // !_SFXSTREAM_H_

#ifndef _SFXCOMMON_H_
#include "sfx/sfxCommon.h"
#endif // !_SFXCOMMON_H_

#ifndef _CONSOLE_H_
#include "console/console.h"
#endif // !_CONSOLE_H_

class SFXProvider;
class SFXListener;
class SFXBuffer;
class SFXVoice;
class SFXStream;
class SFXDescription;

class SFXDevice
{
public:

   typedef void Parent;

   friend class SFXVoice;

   SFXDevice(  SFXProvider *provider,
               const OPENALFNTABLE &openal,
               StringTableEntry name,
               bool useHardware,
               S32 maxBuffers);

   ~SFXDevice();

protected:

   typedef Vector< SFXBuffer* > BufferVector;
   typedef Vector< SFXVoice* > VoiceVector;

   typedef BufferVector::iterator BufferIterator;
   typedef VoiceVector::iterator VoiceIterator;

   SFXProvider* mProvider;
   OPENALFNTABLE mOpenAL;
   StringTableEntry mName;
   ALCcontext *mContext;
   ALCdevice  *mDevice;
   SFXDistanceModel mDistanceModel;
   F32 mDistanceFactor;
   F32 mRolloffFactor;
   F32 mUserRolloffFactor;

   void _setRolloffFactor(F32 factor);

   /// Should the device try to use hardware processing.
   bool mUseHardware;

   /// The maximum playback buffers this device will use.
   S32 mMaxBuffers;

   /// Current set of sound buffers.
   BufferVector mBuffers;

   /// Current set of voices.
   VoiceVector mVoices;

   /// Device capabilities.
   U32 mCaps;

   /// Current number of buffers.  Reflected in $SFX::Device::numBuffers.
   U32 mStatNumBuffers;

   /// Current number of voices.  Reflected in $SFX::Device::numVoices.
   U32 mStatNumVoices;

   /// Current total memory size of sound buffers.  Reflected in $SFX::Device::numBufferBytes.
   U32 mStatNumBufferBytes;

   /// Register a buffer with the device.
   /// This also triggers the buffer's stream packet request chain.
   void _addBuffer(SFXBuffer* buffer);

   /// Unregister the given buffer.
   void _removeBuffer(SFXBuffer* buffer);

   /// Register a voice with the device.
   void _addVoice(SFXVoice* voice);

   /// Unregister the given voice.
   virtual void _removeVoice(SFXVoice* buffer);

public:

   // SFXDevice.
   SFXBuffer* createBuffer(const SFXStream& stream, SFXDescription* description);
   SFXVoice* createVoice(bool is3D, SFXBuffer *buffer);
   void setListener(U32 index, const SFXListenerProperties& listener);
   void setDistanceModel(SFXDistanceModel model);
   void setDopplerFactor(F32 factor);
   void setRolloffFactor(F32 factor);

   #if defined(AL_ALEXT_PROTOTYPES)
   //function for openAL to open slots
   void openSlots();
   //slots
   ALuint	effectSlot[4] = { 0 };
   ALuint	effect[2] = { 0 };
   ALuint   uLoop;
   //get values from sfxreverbproperties and pass it to openal device
   void setReverb(const SFXReverbProperties& reverb);
   #endif

};

#endif
