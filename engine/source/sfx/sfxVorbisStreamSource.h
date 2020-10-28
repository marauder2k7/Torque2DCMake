//--------------------------------------------
// vorbisStreamSource.h
// header for streaming audio source for Ogg Vorbis
//
// Kurtis Seebaldt
//--------------------------------------------

#ifndef _SFXVORBISSTREAMSOURCE_H_
#define _SFXVORBISSTREAMSOURCE_H_

#ifndef _SFXSTREAMSOURCE_H_
#include "sfx/sfxStreamSource.h"
#endif

#ifndef TORQUE_OS_IOS
#include "vorbis/vorbisfile.h"

class SFXVorbisStreamSource : public SFXStreamSource
{
public:
   SFXVorbisStreamSource(const OPENALFNTABLE &oalft,const char *filename);
   virtual ~SFXVorbisStreamSource();

   virtual bool initStream();
   virtual bool updateBuffers();
   virtual void freeStream();
   virtual F32 getElapsedTime();
   virtual F32 getTotalTime();

private:
   const OPENALFNTABLE &mOpenAL;
   ALuint				    mBufferList[NUMBUFFERS];
   S32						mNumBuffers;
   S32						mBufferSize;
   Stream				   *stream;

   bool					bReady;
   bool					bFinished;

   ALenum  format;
   ALsizei size;
   ALsizei freq;

   ALuint			DataSize;
   ALuint			DataLeft;
   ALuint			buffersinqueue;

   bool			bBuffersAllocated;
   bool			bVorbisFileInitialized;

   int current_section;
   OggVorbis_File vf;

   void clear();
   long oggRead(char *buffer, int length, int bigendianp, int *bitstream);
   void resetStream();
   void setNewFile(const char * file);
};
#endif

#endif // _VORBISSTREAMSOURCE_H_
