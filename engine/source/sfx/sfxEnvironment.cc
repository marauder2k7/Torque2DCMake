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

#include "sfx/sfxEnvironment.h"
#include "network/netConnection.h"
#include "io/bitStream.h"


IMPLEMENT_CO_DATABLOCK_V1(SFXEnvironment);


// Reverb flags.
static const U32 sReverbFlagDecayTimeScale = 0x001;
static const U32 sReverbFlagReflectionsScale = 0x002;
//static const U32 sReverbFlagReflectionsDelayScale  = 0x004; unused, but kept for doc purposes -BJR
static const U32 sReverbFlagReverbScale = 0x008;
static const U32 sReverbFlagReverbDelayScale = 0x010;
static const U32 sReverbFlagDecayHFLimit = 0x020;
static const U32 sReverbFlagEchoTimeScale = 0x040;
static const U32 sReverbFlagModulationTimeScale = 0x080;
static const U32 sReverbFlagCore0 = 0x100;
static const U32 sReverbFlagCore1 = 0x200;
static const U32 sReverbFlagHighQualityReverb = 0x400;
static const U32 sReverbFlagHighQualityDPL2Reverb = 0x800;


//-----------------------------------------------------------------------------

SFXEnvironment::SFXEnvironment()
{
}

//-----------------------------------------------------------------------------

void SFXEnvironment::initPersistFields()
{
   addGroup("Reverb");

   addField("reverbDensity", TypeF32, Offset(mReverb.flDensity, SFXEnvironment),
      "Density of reverb environment.");
   addField("reverbDiffusion", TypeF32, Offset(mReverb.flDiffusion, SFXEnvironment),
      "Environment diffusion.");
   addField("reverbGain", TypeF32, Offset(mReverb.flGain, SFXEnvironment),
      "Reverb Gain Level.");
   addField("reverbGainHF", TypeF32, Offset(mReverb.flGainHF, SFXEnvironment),
      "Reverb Gain to high frequencies");
   addField("reverbGainLF", TypeF32, Offset(mReverb.flGainLF, SFXEnvironment),
      "Reverb Gain to high frequencies");
   addField("reverbDecayTime", TypeF32, Offset(mReverb.flDecayTime, SFXEnvironment),
      "Decay time for the reverb.");
   addField("reverbDecayHFRatio", TypeF32, Offset(mReverb.flDecayHFRatio, SFXEnvironment),
      "High frequency decay time ratio.");
   addField("reverbDecayLFRatio", TypeF32, Offset(mReverb.flDecayLFRatio, SFXEnvironment),
      "High frequency decay time ratio.");
   addField("reflectionsGain", TypeF32, Offset(mReverb.flReflectionsGain, SFXEnvironment),
      "Reflection Gain.");
   addField("reflectionDelay", TypeF32, Offset(mReverb.flReflectionsDelay, SFXEnvironment),
      "How long to delay reflections.");
   addField("reflectionsPan", TypeF32, Offset(mReverb.flReflectionsPan, SFXEnvironment),
      "Reflection reverberation panning vector.");
   addField("lateReverbGain", TypeF32, Offset(mReverb.flLateReverbGain, SFXEnvironment),
      "Late reverb gain amount.");
   addField("lateReverbDelay", TypeF32, Offset(mReverb.flLateReverbDelay, SFXEnvironment),
      "Late reverb delay time.");
   addField("lateReverbPan", TypeF32, Offset(mReverb.flLateReverbPan, SFXEnvironment),
      "Late reverberation panning vector.");
   addField("reverbEchoTime", TypeF32, Offset(mReverb.flEchoTime, SFXEnvironment),
      "Reverb echo time.");
   addField("reverbEchoDepth", TypeF32, Offset(mReverb.flEchoDepth, SFXEnvironment),
      "Reverb echo depth.");
   addField("reverbModTime", TypeF32, Offset(mReverb.flModulationTime, SFXEnvironment),
      "Reverb Modulation time.");
   addField("reverbModDepth", TypeF32, Offset(mReverb.flModulationDepth, SFXEnvironment),
      "Reverb Modulation time.");
   addField("airAbsorbtionGainHF", TypeF32, Offset(mReverb.flAirAbsorptionGainHF, SFXEnvironment),
      "High Frequency air absorbtion");
   addField("reverbHFRef", TypeF32, Offset(mReverb.flHFReference, SFXEnvironment),
      "Reverb High Frequency Reference.");
   addField("reverbLFRef", TypeF32, Offset(mReverb.flLFReference, SFXEnvironment),
      "Reverb Low Frequency Reference.");
   addField("roomRolloffFactor", TypeF32, Offset(mReverb.flRoomRolloffFactor, SFXEnvironment),
      "Rolloff factor for reverb.");
   addField("decayHFLimit", TypeS32, Offset(mReverb.iDecayHFLimit, SFXEnvironment),
      "High Frequency decay limit.");
   endGroup("Reverb");

   Parent::initPersistFields();
}

//-----------------------------------------------------------------------------

bool SFXEnvironment::onAdd()
{
   if (!Parent::onAdd())
      return false;

   return true;
}


//-----------------------------------------------------------------------------

void SFXEnvironment::inspectPostApply()
{
   Parent::inspectPostApply();
   validate();
}

//-----------------------------------------------------------------------------

void SFXEnvironment::validate()
{
   mReverb.validate();
}

//-----------------------------------------------------------------------------

void SFXEnvironment::packData(BitStream* stream)
{
   Parent::packData(stream);

   stream->write(mReverb.flDensity);
   stream->write(mReverb.flDiffusion);
   stream->write(mReverb.flGain);
   stream->write(mReverb.flGainHF);
   stream->write(mReverb.flGainLF);
   stream->write(mReverb.flDecayTime);
   stream->write(mReverb.flDecayHFRatio);
   stream->write(mReverb.flDecayLFRatio);
   stream->write(mReverb.flReflectionsGain);
   stream->write(mReverb.flReflectionsDelay);
   stream->write(mReverb.flLateReverbGain);
   stream->write(mReverb.flLateReverbDelay);
   stream->write(mReverb.flEchoTime);
   stream->write(mReverb.flEchoDepth);
   stream->write(mReverb.flModulationTime);
   stream->write(mReverb.flModulationDepth);
   stream->write(mReverb.flAirAbsorptionGainHF);
   stream->write(mReverb.flHFReference);
   stream->write(mReverb.flLFReference);
   stream->write(mReverb.flRoomRolloffFactor);
   stream->write(mReverb.iDecayHFLimit);
}

//-----------------------------------------------------------------------------

void SFXEnvironment::unpackData(BitStream* stream)
{
   Parent::unpackData(stream);

   stream->read(&mReverb.flDensity);
   stream->read(&mReverb.flDiffusion);
   stream->read(&mReverb.flGain);
   stream->read(&mReverb.flGainHF);
   stream->read(&mReverb.flGainLF);
   stream->read(&mReverb.flDecayTime);
   stream->read(&mReverb.flDecayHFRatio);
   stream->read(&mReverb.flDecayLFRatio);
   stream->read(&mReverb.flReflectionsGain);
   stream->read(&mReverb.flReflectionsDelay);
   stream->read(&mReverb.flLateReverbGain);
   stream->read(&mReverb.flLateReverbDelay);
   stream->read(&mReverb.flEchoTime);
   stream->read(&mReverb.flEchoDepth);
   stream->read(&mReverb.flModulationTime);
   stream->read(&mReverb.flModulationDepth);
   stream->read(&mReverb.flAirAbsorptionGainHF);
   stream->read(&mReverb.flHFReference);
   stream->read(&mReverb.flLFReference);
   stream->read(&mReverb.flRoomRolloffFactor);
   stream->read(&mReverb.iDecayHFLimit);
}

//--------------------------------------------------------------------------
// Class AudioEnvironmentProfile:
//--------------------------------------------------------------------------
IMPLEMENT_CO_DATABLOCK_V1(SFXSampleEnvironment);

SFXSampleEnvironment::SFXSampleEnvironment()
{
   mDirect = 0;
   mDirectHF = 0;
   mRoom = 0;
   mRoomHF = 0;
   mObstruction = 0.f;
   mObstructionLFRatio = 0.f;
   mOcclusion = 0.f;
   mOcclusionLFRatio = 0.f;
   mOcclusionRoomRatio = 0.f;
   mRoomRolloff = 0.f;
   mAirAbsorption = 0.f;
   mOutsideVolumeHF = 0;
   mFlags = 0;
}

//--------------------------------------------------------------------------

void SFXSampleEnvironment::initPersistFields()
{
   Parent::initPersistFields();

   addField("direct", TypeS32, Offset(mDirect, SFXSampleEnvironment));
   addField("directHF", TypeS32, Offset(mDirectHF, SFXSampleEnvironment));
   addField("room", TypeS32, Offset(mRoom, SFXSampleEnvironment));
   addField("obstruction", TypeF32, Offset(mObstruction, SFXSampleEnvironment));
   addField("obstructionLFRatio", TypeF32, Offset(mObstructionLFRatio, SFXSampleEnvironment));
   addField("occlusion", TypeF32, Offset(mOcclusion, SFXSampleEnvironment));
   addField("occlusionLFRatio", TypeF32, Offset(mOcclusionLFRatio, SFXSampleEnvironment));
   addField("occlusionRoomRatio", TypeF32, Offset(mOcclusionRoomRatio, SFXSampleEnvironment));
   addField("roomRolloff", TypeF32, Offset(mRoomRolloff, SFXSampleEnvironment));
   addField("airAbsorption", TypeF32, Offset(mAirAbsorption, SFXSampleEnvironment));
   addField("outsideVolumeHF", TypeS32, Offset(mOutsideVolumeHF, SFXSampleEnvironment));
   addField("flags", TypeS32, Offset(mFlags, SFXSampleEnvironment));
}

void SFXSampleEnvironment::packData(BitStream* stream)
{
   Parent::packData(stream);
   writeRangedS32(stream, mDirect, -10000, 1000);
   writeRangedS32(stream, mDirectHF, -10000, 0);
   writeRangedS32(stream, mRoom, -10000, 1000);
   writeRangedS32(stream, mRoomHF, -10000, 0);
   writeRangedF32(stream, mObstruction, 0.f, 1.f, 9);
   writeRangedF32(stream, mObstructionLFRatio, 0.f, 1.f, 8);
   writeRangedF32(stream, mOcclusion, 0.f, 1.f, 9);
   writeRangedF32(stream, mOcclusionLFRatio, 0.f, 1.f, 8);
   writeRangedF32(stream, mOcclusionRoomRatio, 0.f, 10.f, 9);
   writeRangedF32(stream, mRoomRolloff, 0.f, 10.f, 9);
   writeRangedF32(stream, mAirAbsorption, 0.f, 10.f, 9);
   writeRangedS32(stream, mOutsideVolumeHF, -10000, 0);
   stream->writeInt(mFlags, 3);
}

void SFXSampleEnvironment::unpackData(BitStream* stream)
{
   Parent::unpackData(stream);
   mDirect = readRangedS32(stream, -10000, 1000);
   mDirectHF = readRangedS32(stream, -10000, 0);
   mRoom = readRangedS32(stream, -10000, 1000);
   mRoomHF = readRangedS32(stream, -10000, 0);
   mObstruction = readRangedF32(stream, 0.f, 1.f, 9);
   mObstructionLFRatio = readRangedF32(stream, 0.f, 1.f, 8);
   mOcclusion = readRangedF32(stream, 0.f, 1.f, 9);
   mOcclusionLFRatio = readRangedF32(stream, 0.f, 1.f, 8);
   mOcclusionRoomRatio = readRangedF32(stream, 0.f, 10.f, 9);
   mRoomRolloff = readRangedF32(stream, 0.f, 10.f, 9);
   mAirAbsorption = readRangedF32(stream, 0.f, 10.f, 9);
   mOutsideVolumeHF = readRangedS32(stream, -10000, 0);
   mFlags = stream->readInt(3);
}
