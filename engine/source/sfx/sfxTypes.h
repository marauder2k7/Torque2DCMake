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

#ifndef _SFXTYPES_H_
#define _SFXTYPES_H_

#ifndef _CONSOLETYPES_H_
#include "console/consoleTypes.h"
#endif


/// @file
/// Additional SFX console types.


class BitStream;
class SFXTrack;
class SFXEnvironment;
class SFXDescription;
class SFXState;
class SFXSource;


///
DefineConsoleType(TypeSFXSourceName, SFXSource*);

/// SFXDescription datablock reference.  May be client-only.
DefineConsoleType(TypeSFXDescriptionName, SFXDescription*);

/// SFXTrack datablock reference.  May be client-only.
DefineConsoleType(TypeSFXTrackName, SFXTrack*);

/// SFXEnvironment datablock reference.  May be client-only.
DefineConsoleType(TypeSFXEnvironmentName, SFXEnvironment*);

/// SFXState datablock reference.  May be client-only.
DefineConsoleType(TypeSFXStateName, SFXState*);



/// @name Datablock Network I/O
///
/// The following functions allow to transmit datablock references over the network
/// even when the referenced datablocks are not networked ("client-only" datablocks).
///
/// The write functions write a reference to the bitstream.
///
/// The read functions read a reference from the bitstream and store it in a non-resolved
/// form, i.e. the resulting pointer will not yet be valid.  This is necessary as during
/// datablock transmission, the referenced datablock may actually only be transmitted
/// later in the stream and cannot thus be immediately looked up on the client.
///
/// The resolve functions take an unresolved datablock reference and resolve them to
/// valid pointers.
///
/// @{

void sfxWrite(BitStream* stream, SFXSource* source);
void sfxWrite(BitStream* stream, SFXDescription* description);
void sfxWrite(BitStream* stream, SFXTrack* track);
void sfxWrite(BitStream* stream, SFXEnvironment* environment);
void sfxWrite(BitStream* stream, SFXState* state);

void sfxRead(BitStream* stream, SFXDescription** description);
void sfxRead(BitStream* stream, SFXTrack** track);
void sfxRead(BitStream* stream, SFXEnvironment** environment);
void sfxRead(BitStream* stream, SFXState** state);

bool sfxResolve(SFXDescription** description, char& errorString);
bool sfxResolve(SFXTrack** track, char& errorString);
bool sfxResolve(SFXEnvironment** environment, char& errorString);
bool sfxResolve(SFXState** state, char& errorString);

bool sfxReadAndResolve(BitStream* stream, SFXSource** source, char& errorString);

inline bool sfxReadAndResolve(BitStream* stream, SFXDescription** description, char& errorString)
{
   sfxRead(stream, description);
   return sfxResolve(description, errorString);
}

inline bool sfxReadAndResolve(BitStream* stream, SFXTrack** track, char& errorString)
{
   sfxRead(stream, track);
   return sfxResolve(track, errorString);
}

inline bool sfxReadAndResolve(BitStream* stream, SFXEnvironment** environment, char& errorString)
{
   sfxRead(stream, environment);
   return sfxResolve(environment, errorString);
}

inline bool sfxReadAndResolve(BitStream* stream, SFXState** state, char& errorString)
{
   sfxRead(stream, state);
   return sfxResolve(state, errorString);
}


/// @}

#endif // !_SFXTYPES_H_
