/*
 * Copyright (c) 2006, Creative Labs Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided
 * that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright notice, this list of conditions and
 * 	     the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright notice, this list of conditions
 * 	     and the following disclaimer in the documentation and/or other materials provided with the distribution.
 *     * Neither the name of Creative Labs Inc. nor the names of its contributors may be used to endorse or
 * 	     promote products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include "platformWin32/platformWin32.h"
#include "console/console.h"
#include <windows.h>
#include "platform/platformAL.h"


   ALboolean LoadOAL10Library(char *szOALFullPathName, LPOPENALFNTABLE lpOALFnTable)
   {
      if (!lpOALFnTable)
         return AL_FALSE;

      if (szOALFullPathName)
         winState.hinstOpenAL = LoadLibraryA(szOALFullPathName);
      else
         winState.hinstOpenAL = LoadLibraryA("openal32.dll");

      if (!winState.hinstOpenAL)
         return AL_FALSE;

      memset(lpOALFnTable, 0, sizeof(OPENALFNTABLE));

      // Get function pointers
      lpOALFnTable->alEnable = (LPALENABLE)GetProcAddress(winState.hinstOpenAL, "alEnable");
      if (lpOALFnTable->alEnable == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alEnable' function address\n");
         return AL_FALSE;
      }
      lpOALFnTable->alDisable = (LPALDISABLE)GetProcAddress(winState.hinstOpenAL, "alDisable");
      if (lpOALFnTable->alDisable == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alDisable' function address\n");
         return AL_FALSE;
      }
      lpOALFnTable->alIsEnabled = (LPALISENABLED)GetProcAddress(winState.hinstOpenAL, "alIsEnabled");
      if (lpOALFnTable->alIsEnabled == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alIsEnabled' function address\n");
         return AL_FALSE;
      }
      lpOALFnTable->alGetBoolean = (LPALGETBOOLEAN)GetProcAddress(winState.hinstOpenAL, "alGetBoolean");
      if (lpOALFnTable->alGetBoolean == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alGetBoolean' function address\n");
         return AL_FALSE;
      }
      lpOALFnTable->alGetInteger = (LPALGETINTEGER)GetProcAddress(winState.hinstOpenAL, "alGetInteger");
      if (lpOALFnTable->alGetInteger == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alGetInteger' function address\n");
         return AL_FALSE;
      }
      lpOALFnTable->alGetFloat = (LPALGETFLOAT)GetProcAddress(winState.hinstOpenAL, "alGetFloat");
      if (lpOALFnTable->alGetFloat == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alGetFloat' function address\n");
         return AL_FALSE;
      }
      lpOALFnTable->alGetDouble = (LPALGETDOUBLE)GetProcAddress(winState.hinstOpenAL, "alGetDouble");
      if (lpOALFnTable->alGetDouble == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alGetDouble' function address\n");
         return AL_FALSE;
      }
      lpOALFnTable->alGetBooleanv = (LPALGETBOOLEANV)GetProcAddress(winState.hinstOpenAL, "alGetBooleanv");
      if (lpOALFnTable->alGetBooleanv == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alGetBooleanv' function address\n");
         return AL_FALSE;
      }
      lpOALFnTable->alGetIntegerv = (LPALGETINTEGERV)GetProcAddress(winState.hinstOpenAL, "alGetIntegerv");
      if (lpOALFnTable->alGetIntegerv == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alGetIntegerv' function address\n");
         return AL_FALSE;
      }
      lpOALFnTable->alGetFloatv = (LPALGETFLOATV)GetProcAddress(winState.hinstOpenAL, "alGetFloatv");
      if (lpOALFnTable->alGetFloatv == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alGetFloatv' function address\n");
         return AL_FALSE;
      }
      lpOALFnTable->alGetDoublev = (LPALGETDOUBLEV)GetProcAddress(winState.hinstOpenAL, "alGetDoublev");
      if (lpOALFnTable->alGetDoublev == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alGetDoublev' function address\n");
         return AL_FALSE;
      }
      lpOALFnTable->alGetString = (LPALGETSTRING)GetProcAddress(winState.hinstOpenAL, "alGetString");
      if (lpOALFnTable->alGetString == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alGetString' function address\n");
         return AL_FALSE;
      }
      lpOALFnTable->alGetError = (LPALGETERROR)GetProcAddress(winState.hinstOpenAL, "alGetError");
      if (lpOALFnTable->alGetError == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alGetError' function address\n");
         return AL_FALSE;
      }
      lpOALFnTable->alIsExtensionPresent = (LPALISEXTENSIONPRESENT)GetProcAddress(winState.hinstOpenAL, "alIsExtensionPresent");
      if (lpOALFnTable->alIsExtensionPresent == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alIsExtensionPresent' function address\n");
         return AL_FALSE;
      }
      lpOALFnTable->alGetProcAddress = (LPALGETPROCADDRESS)GetProcAddress(winState.hinstOpenAL, "alGetProcAddress");
      if (lpOALFnTable->alGetProcAddress == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alGetProcAddress' function address\n");
         return AL_FALSE;
      }
      lpOALFnTable->alGetEnumValue = (LPALGETENUMVALUE)GetProcAddress(winState.hinstOpenAL, "alGetEnumValue");
      if (lpOALFnTable->alGetEnumValue == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alGetEnumValue' function address\n");
         return AL_FALSE;
      }
      lpOALFnTable->alListeneri = (LPALLISTENERI)GetProcAddress(winState.hinstOpenAL, "alListeneri");
      if (lpOALFnTable->alListeneri == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alListeneri' function address\n");
         return AL_FALSE;
      }
      lpOALFnTable->alListenerf = (LPALLISTENERF)GetProcAddress(winState.hinstOpenAL, "alListenerf");
      if (lpOALFnTable->alListenerf == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alListenerf' function address\n");
         return AL_FALSE;
      }
      lpOALFnTable->alListener3f = (LPALLISTENER3F)GetProcAddress(winState.hinstOpenAL, "alListener3f");
      if (lpOALFnTable->alListener3f == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alListener3f' function address\n");
         return AL_FALSE;
      }
      lpOALFnTable->alListenerfv = (LPALLISTENERFV)GetProcAddress(winState.hinstOpenAL, "alListenerfv");
      if (lpOALFnTable->alListenerfv == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alListenerfv' function address\n");
         return AL_FALSE;
      }
      lpOALFnTable->alGetListeneri = (LPALGETLISTENERI)GetProcAddress(winState.hinstOpenAL, "alGetListeneri");
      if (lpOALFnTable->alGetListeneri == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alGetListeneri' function address\n");
         return AL_FALSE;
      }
      lpOALFnTable->alGetListenerf = (LPALGETLISTENERF)GetProcAddress(winState.hinstOpenAL, "alGetListenerf");
      if (lpOALFnTable->alGetListenerf == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alGetListenerf' function address\n");
         return AL_FALSE;
      }
      lpOALFnTable->alGetListener3f = (LPALGETLISTENER3F)GetProcAddress(winState.hinstOpenAL, "alGetListener3f");
      if (lpOALFnTable->alGetListener3f == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alGetListener3f' function address\n");
         return AL_FALSE;
      }
      lpOALFnTable->alGetListenerfv = (LPALGETLISTENERFV)GetProcAddress(winState.hinstOpenAL, "alGetListenerfv");
      if (lpOALFnTable->alGetListenerfv == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alGetListenerfv' function address\n");
         return AL_FALSE;
      }
      lpOALFnTable->alGenSources = (LPALGENSOURCES)GetProcAddress(winState.hinstOpenAL, "alGenSources");
      if (lpOALFnTable->alGenSources == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alGenSources' function address\n");
         return AL_FALSE;
      }
      lpOALFnTable->alDeleteSources = (LPALDELETESOURCES)GetProcAddress(winState.hinstOpenAL, "alDeleteSources");
      if (lpOALFnTable->alDeleteSources == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alDeleteSources' function address\n");
         return AL_FALSE;
      }
      lpOALFnTable->alIsSource = (LPALISSOURCE)GetProcAddress(winState.hinstOpenAL, "alIsSource");
      if (lpOALFnTable->alIsSource == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alIsSource' function address\n");
         return AL_FALSE;
      }
      lpOALFnTable->alSourcei = (LPALSOURCEI)GetProcAddress(winState.hinstOpenAL, "alSourcei");
      if (lpOALFnTable->alSourcei == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alSourcei' function address\n");
         return AL_FALSE;
      }
      lpOALFnTable->alSourcef = (LPALSOURCEF)GetProcAddress(winState.hinstOpenAL, "alSourcef");
      if (lpOALFnTable->alSourcef == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alSourcef' function address\n");
         return AL_FALSE;
      }
      lpOALFnTable->alSource3f = (LPALSOURCE3F)GetProcAddress(winState.hinstOpenAL, "alSource3f");
      if (lpOALFnTable->alSource3f == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alSource3f' function address\n");
         return AL_FALSE;
      }
      lpOALFnTable->alSourcefv = (LPALSOURCEFV)GetProcAddress(winState.hinstOpenAL, "alSourcefv");
      if (lpOALFnTable->alSourcefv == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alSourcefv' function address\n");
         return AL_FALSE;
      }
      lpOALFnTable->alGetSourcei = (LPALGETSOURCEI)GetProcAddress(winState.hinstOpenAL, "alGetSourcei");
      if (lpOALFnTable->alGetSourcei == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alGetSourcei' function address\n");
         return AL_FALSE;
      }
      lpOALFnTable->alGetSourcef = (LPALGETSOURCEF)GetProcAddress(winState.hinstOpenAL, "alGetSourcef");
      if (lpOALFnTable->alGetSourcef == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alGetSourcef' function address\n");
         return AL_FALSE;
      }
      lpOALFnTable->alGetSourcefv = (LPALGETSOURCEFV)GetProcAddress(winState.hinstOpenAL, "alGetSourcefv");
      if (lpOALFnTable->alGetSourcefv == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alGetSourcefv' function address\n");
         return AL_FALSE;
      }
      lpOALFnTable->alSourcePlayv = (LPALSOURCEPLAYV)GetProcAddress(winState.hinstOpenAL, "alSourcePlayv");
      if (lpOALFnTable->alSourcePlayv == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alSourcePlayv' function address\n");
         return AL_FALSE;
      }
      lpOALFnTable->alSourceStopv = (LPALSOURCESTOPV)GetProcAddress(winState.hinstOpenAL, "alSourceStopv");
      if (lpOALFnTable->alSourceStopv == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alSourceStopv' function address\n");
         return AL_FALSE;
      }
      lpOALFnTable->alSourcePlay = (LPALSOURCEPLAY)GetProcAddress(winState.hinstOpenAL, "alSourcePlay");
      if (lpOALFnTable->alSourcePlay == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alSourcePlay' function address\n");
         return AL_FALSE;
      }
      lpOALFnTable->alSourcePause = (LPALSOURCEPAUSE)GetProcAddress(winState.hinstOpenAL, "alSourcePause");
      if (lpOALFnTable->alSourcePause == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alSourcePause' function address\n");
         return AL_FALSE;
      }
      lpOALFnTable->alSourceStop = (LPALSOURCESTOP)GetProcAddress(winState.hinstOpenAL, "alSourceStop");
      if (lpOALFnTable->alSourceStop == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alSourceStop' function address\n");
         return AL_FALSE;
      }
      lpOALFnTable->alSourceRewind = (LPALSOURCEREWIND)GetProcAddress(winState.hinstOpenAL, "alSourceRewind");
      if (lpOALFnTable->alSourceRewind == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alSourceRewind' function address\n");
         return AL_FALSE;
      }
      lpOALFnTable->alGenBuffers = (LPALGENBUFFERS)GetProcAddress(winState.hinstOpenAL, "alGenBuffers");
      if (lpOALFnTable->alGenBuffers == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alGenBuffers' function address\n");
         return AL_FALSE;
      }
      lpOALFnTable->alDeleteBuffers = (LPALDELETEBUFFERS)GetProcAddress(winState.hinstOpenAL, "alDeleteBuffers");
      if (lpOALFnTable->alDeleteBuffers == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alDeleteBuffers' function address\n");
         return AL_FALSE;
      }
      lpOALFnTable->alIsBuffer = (LPALISBUFFER)GetProcAddress(winState.hinstOpenAL, "alIsBuffer");
      if (lpOALFnTable->alIsBuffer == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alIsBuffer' function address\n");
         return AL_FALSE;
      }
      lpOALFnTable->alBufferData = (LPALBUFFERDATA)GetProcAddress(winState.hinstOpenAL, "alBufferData");
      if (lpOALFnTable->alBufferData == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alBufferData' function address\n");
         return AL_FALSE;
      }
      lpOALFnTable->alGetBufferi = (LPALGETBUFFERI)GetProcAddress(winState.hinstOpenAL, "alGetBufferi");
      if (lpOALFnTable->alGetBufferi == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alGetBufferi' function address\n");
         return AL_FALSE;
      }
      lpOALFnTable->alGetBufferf = (LPALGETBUFFERF)GetProcAddress(winState.hinstOpenAL, "alGetBufferf");
      if (lpOALFnTable->alGetBufferf == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alGetBufferf' function address\n");
         return AL_FALSE;
      }
      lpOALFnTable->alSourceQueueBuffers = (LPALSOURCEQUEUEBUFFERS)GetProcAddress(winState.hinstOpenAL, "alSourceQueueBuffers");
      if (lpOALFnTable->alSourceQueueBuffers == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alSourceQueueBuffers' function address\n");
         return AL_FALSE;
      }
      lpOALFnTable->alSourceUnqueueBuffers = (LPALSOURCEUNQUEUEBUFFERS)GetProcAddress(winState.hinstOpenAL, "alSourceUnqueueBuffers");
      if (lpOALFnTable->alSourceUnqueueBuffers == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alSourceUnqueueBuffers' function address\n");
         return AL_FALSE;
      }
      lpOALFnTable->alDistanceModel = (LPALDISTANCEMODEL)GetProcAddress(winState.hinstOpenAL, "alDistanceModel");
      if (lpOALFnTable->alDistanceModel == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alDistanceModel' function address\n");
         return AL_FALSE;
      }
      lpOALFnTable->alDopplerFactor = (LPALDOPPLERFACTOR)GetProcAddress(winState.hinstOpenAL, "alDopplerFactor");
      if (lpOALFnTable->alDopplerFactor == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alDopplerFactor' function address\n");
         return AL_FALSE;
      }
      lpOALFnTable->alDopplerVelocity = (LPALDOPPLERVELOCITY)GetProcAddress(winState.hinstOpenAL, "alDopplerVelocity");
      if (lpOALFnTable->alDopplerVelocity == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alDopplerVelocity' function address\n");
         return AL_FALSE;
      }
      lpOALFnTable->alcGetString = (LPALCGETSTRING)GetProcAddress(winState.hinstOpenAL, "alcGetString");
      if (lpOALFnTable->alcGetString == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alcGetString' function address\n");
         return AL_FALSE;
      }
      lpOALFnTable->alcGetIntegerv = (LPALCGETINTEGERV)GetProcAddress(winState.hinstOpenAL, "alcGetIntegerv");
      if (lpOALFnTable->alcGetIntegerv == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alcGetIntegerv' function address\n");
         return AL_FALSE;
      }
      lpOALFnTable->alcOpenDevice = (LPALCOPENDEVICE)GetProcAddress(winState.hinstOpenAL, "alcOpenDevice");
      if (lpOALFnTable->alcOpenDevice == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alcOpenDevice' function address\n");
         return AL_FALSE;
      }
      lpOALFnTable->alcCloseDevice = (LPALCCLOSEDEVICE)GetProcAddress(winState.hinstOpenAL, "alcCloseDevice");
      if (lpOALFnTable->alcCloseDevice == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alcCloseDevice' function address\n");
         return AL_FALSE;
      }
      lpOALFnTable->alcCreateContext = (LPALCCREATECONTEXT)GetProcAddress(winState.hinstOpenAL, "alcCreateContext");
      if (lpOALFnTable->alcCreateContext == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alcCreateContext' function address\n");
         return AL_FALSE;
      }
      lpOALFnTable->alcMakeContextCurrent = (LPALCMAKECONTEXTCURRENT)GetProcAddress(winState.hinstOpenAL, "alcMakeContextCurrent");
      if (lpOALFnTable->alcMakeContextCurrent == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alcMakeContextCurrent' function address\n");
         return AL_FALSE;
      }
      lpOALFnTable->alcProcessContext = (LPALCPROCESSCONTEXT)GetProcAddress(winState.hinstOpenAL, "alcProcessContext");
      if (lpOALFnTable->alcProcessContext == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alcProcessContext' function address\n");
         return AL_FALSE;
      }
      lpOALFnTable->alcGetCurrentContext = (LPALCGETCURRENTCONTEXT)GetProcAddress(winState.hinstOpenAL, "alcGetCurrentContext");
      if (lpOALFnTable->alcGetCurrentContext == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alcGetCurrentContext' function address\n");
         return AL_FALSE;
      }
      lpOALFnTable->alcGetContextsDevice = (LPALCGETCONTEXTSDEVICE)GetProcAddress(winState.hinstOpenAL, "alcGetContextsDevice");
      if (lpOALFnTable->alcGetContextsDevice == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alcGetContextsDevice' function address\n");
         return AL_FALSE;
      }
      lpOALFnTable->alcSuspendContext = (LPALCSUSPENDCONTEXT)GetProcAddress(winState.hinstOpenAL, "alcSuspendContext");
      if (lpOALFnTable->alcSuspendContext == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alcSuspendContext' function address\n");
         return AL_FALSE;
      }
      lpOALFnTable->alcDestroyContext = (LPALCDESTROYCONTEXT)GetProcAddress(winState.hinstOpenAL, "alcDestroyContext");
      if (lpOALFnTable->alcDestroyContext == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alcDestroyContext' function address\n");
         return AL_FALSE;
      }
      lpOALFnTable->alcGetError = (LPALCGETERROR)GetProcAddress(winState.hinstOpenAL, "alcGetError");
      if (lpOALFnTable->alcGetError == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alcGetError' function address\n");
         return AL_FALSE;
      }
      lpOALFnTable->alcIsExtensionPresent = (LPALCISEXTENSIONPRESENT)GetProcAddress(winState.hinstOpenAL, "alcIsExtensionPresent");
      if (lpOALFnTable->alcIsExtensionPresent == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alcIsExtensionPresent' function address\n");
         return AL_FALSE;
      }
      lpOALFnTable->alcGetProcAddress = (LPALCGETPROCADDRESS)GetProcAddress(winState.hinstOpenAL, "alcGetProcAddress");
      if (lpOALFnTable->alcGetProcAddress == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alcGetProcAddress' function address\n");
         return AL_FALSE;
      }
      lpOALFnTable->alcGetEnumValue = (LPALCGETENUMVALUE)GetProcAddress(winState.hinstOpenAL, "alcGetEnumValue");
      if (lpOALFnTable->alcGetEnumValue == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alcGetEnumValue' function address\n");
         return AL_FALSE;
      }
#if defined(AL_ALEXT_PROTOTYPES)
      lpOALFnTable->alGenEffects = (LPALGENEFFECTS)GetProcAddress(winState.hinstOpenAL, "alGenEffects");
      if (lpOALFnTable->alGenEffects == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alGenEffects' function address\n");
      }
      lpOALFnTable->alEffecti = (LPALEFFECTI)GetProcAddress(winState.hinstOpenAL, "alEffecti");
      if (lpOALFnTable->alEffecti == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alEffecti' function address\n");
      }
      lpOALFnTable->alEffectiv = (LPALEFFECTIV)GetProcAddress(winState.hinstOpenAL, "alEffectiv");
      if (lpOALFnTable->alEffectiv == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alEffectiv' function address\n");
      }
      lpOALFnTable->alEffectf = (LPALEFFECTF)GetProcAddress(winState.hinstOpenAL, "alEffectf");
      if (lpOALFnTable->alEffectf == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alEffectf' function address\n");
      }
      lpOALFnTable->alEffectfv = (LPALEFFECTFV)GetProcAddress(winState.hinstOpenAL, "alEffectfv");
      if (lpOALFnTable->alEffectfv == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alEffectfv' function address\n");
      }
      lpOALFnTable->alGetEffecti = (LPALGETEFFECTI)GetProcAddress(winState.hinstOpenAL, "alGetEffecti");
      if (lpOALFnTable->alGetEffecti == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alGetEffecti' function address\n");
      }
      lpOALFnTable->alGetEffectiv = (LPALGETEFFECTIV)GetProcAddress(winState.hinstOpenAL, "alGetEffectiv");
      if (lpOALFnTable->alGetEffectiv == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alGetEffectiv' function address\n");
      }
      lpOALFnTable->alGetEffectf = (LPALGETEFFECTF)GetProcAddress(winState.hinstOpenAL, "alGetEffectf");
      if (lpOALFnTable->alGetEffectf == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alGetEffectf' function address\n");
      }
      lpOALFnTable->alGetEffectfv = (LPALGETEFFECTFV)GetProcAddress(winState.hinstOpenAL, "alGetEffectfv");
      if (lpOALFnTable->alGetEffectfv == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alGetEffectfv' function address\n");
      }

      lpOALFnTable->alDeleteEffects = (LPALDELETEEFFECTS)GetProcAddress(winState.hinstOpenAL, "alDeleteEffects");
      if (lpOALFnTable->alDeleteEffects == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alDeleteEffects' function address\n");
      }
      lpOALFnTable->alIsEffect = (LPALISEFFECT)GetProcAddress(winState.hinstOpenAL, "alIsEffect");
      if (lpOALFnTable->alIsEffect == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alIsEffect' function address\n");
      }
      lpOALFnTable->alAuxiliaryEffectSlotf = (LPALAUXILIARYEFFECTSLOTF)GetProcAddress(winState.hinstOpenAL, "alAuxiliaryEffectSlotf");
      if (lpOALFnTable->alAuxiliaryEffectSlotf == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alAuxiliaryEffectSlotf' function address\n");
      }
      lpOALFnTable->alAuxiliaryEffectSlotfv = (LPALAUXILIARYEFFECTSLOTFV)GetProcAddress(winState.hinstOpenAL, "alAuxiliaryEffectSlotfv");
      if (lpOALFnTable->alAuxiliaryEffectSlotfv == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alAuxiliaryEffectSlotfv' function address\n");
      }
      lpOALFnTable->alAuxiliaryEffectSloti = (LPALAUXILIARYEFFECTSLOTI)GetProcAddress(winState.hinstOpenAL, "alAuxiliaryEffectSloti");
      if (lpOALFnTable->alAuxiliaryEffectSloti == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alAuxiliaryEffectSloti' function address\n");
      }
      lpOALFnTable->alAuxiliaryEffectSlotiv = (LPALAUXILIARYEFFECTSLOTIV)GetProcAddress(winState.hinstOpenAL, "alAuxiliaryEffectSlotiv");
      if (lpOALFnTable->alAuxiliaryEffectSlotiv == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alAuxiliaryEffectSlotiv' function address\n");
      }
      lpOALFnTable->alIsAuxiliaryEffectSlot = (LPALISAUXILIARYEFFECTSLOT)GetProcAddress(winState.hinstOpenAL, "alIsAuxiliaryEffectSlot");
      if (lpOALFnTable->alIsAuxiliaryEffectSlot == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alIsAuxiliaryEffectSlot' function address\n");
      }
      lpOALFnTable->alGenAuxiliaryEffectSlots = (LPALGENAUXILIARYEFFECTSLOTS)GetProcAddress(winState.hinstOpenAL, "alGenAuxiliaryEffectSlots");
      if (lpOALFnTable->alGenAuxiliaryEffectSlots == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alGenAuxiliaryEffectSlots' function address\n");
      }
      lpOALFnTable->alDeleteAuxiliaryEffectSlots = (LPALDELETEAUXILIARYEFFECTSLOTS)GetProcAddress(winState.hinstOpenAL, "alDeleteAuxiliaryEffectSlots");
      if (lpOALFnTable->alDeleteAuxiliaryEffectSlots == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alDeleteAuxiliaryEffectSlots' function address\n");
      }
      lpOALFnTable->alGetAuxiliaryEffectSlotf = (LPALGETAUXILIARYEFFECTSLOTF)GetProcAddress(winState.hinstOpenAL, "alGetAuxiliaryEffectSlotf");
      if (lpOALFnTable->alGetAuxiliaryEffectSlotf == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alGetAuxiliaryEffectSlotf' function address\n");
      }
      lpOALFnTable->alGetAuxiliaryEffectSlotfv = (LPALGETAUXILIARYEFFECTSLOTFV)GetProcAddress(winState.hinstOpenAL, "alGetAuxiliaryEffectSlotfv");
      if (lpOALFnTable->alGetAuxiliaryEffectSlotfv == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alGetAuxiliaryEffectSlotfv' function address\n");
      }
      lpOALFnTable->alGetAuxiliaryEffectSloti = (LPALGETAUXILIARYEFFECTSLOTI)GetProcAddress(winState.hinstOpenAL, "alGetAuxiliaryEffectSloti");
      if (lpOALFnTable->alGetAuxiliaryEffectSloti == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alGetAuxiliaryEffectSloti' function address\n");
      }
      lpOALFnTable->alGetAuxiliaryEffectSlotiv = (LPALGETAUXILIARYEFFECTSLOTIV)GetProcAddress(winState.hinstOpenAL, "alGetAuxiliaryEffectSlotiv");
      if (lpOALFnTable->alGetAuxiliaryEffectSlotiv == NULL)
      {
         OutputDebugStringA("Failed to retrieve 'alGetAuxiliaryEffectSlotiv' function address\n");
      }
      lpOALFnTable->alSource3i = (LPALSOURCE3I)GetProcAddress(winState.hinstOpenAL, "alSource3i");
#endif
      return AL_TRUE;
   }

   ALvoid UnloadOAL10Library()
   {
      // Unload the dll
      if (winState.hinstOpenAL)
      {
         FreeLibrary(winState.hinstOpenAL);
         winState.hinstOpenAL = NULL;
      }
   }