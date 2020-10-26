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

#include "sfx/sfxState.h"
#include "sfx/sfxTypes.h"
#include "io/bitStream.h"
#include "console/consoleTypes.h"

IMPLEMENT_CO_DATABLOCK_V1(SFXState);


static Vector< SFXState* > sgActiveStates(__FILE__, __LINE__);


//-----------------------------------------------------------------------------

SFXState::SFXState()
   : mActiveCount(0),
   mDisableCount(0)
{
   dMemset(mIncludedStates, 0, sizeof(mIncludedStates));
   dMemset(mExcludedStates, 0, sizeof(mExcludedStates));
}

//-----------------------------------------------------------------------------

void SFXState::initPersistFields()
{
   addGroup("State");

   addField("includedStates", TypeSFXStateName, Offset(mIncludedStates, SFXState));
   addField("excludedStates", TypeSFXStateName, Offset(mExcludedStates, SFXState));

   endGroup("State");

   Parent::initPersistFields();
}

//-----------------------------------------------------------------------------

void SFXState::activate()
{
   mActiveCount++;
   if (mActiveCount == 1 && !isDisabled())
      _onActivate();
}

//-----------------------------------------------------------------------------

void SFXState::deactivate()
{
   if (!mActiveCount)
      return;

   mActiveCount--;
   if (!mActiveCount && !isDisabled())
      _onDeactivate();
}

//-----------------------------------------------------------------------------

void SFXState::enable()
{
   if (!mDisableCount)
      return;

   mDisableCount--;

   if (!mDisableCount && mActiveCount > 0)
      _onActivate();
}

//-----------------------------------------------------------------------------

void SFXState::disable()
{
   mDisableCount++;

   if (mDisableCount == 1 && mActiveCount > 0)
      _onDeactivate();
}

//-----------------------------------------------------------------------------

bool SFXState::onAdd()
{
   if (!Parent::onAdd())
      return false;

   Sim::getSFXStateSet()->addObject(this);

   return true;
}

//-----------------------------------------------------------------------------

bool SFXState::preload(bool server, char* errorStr)
{
   if (!Parent::preload(server, errorStr))
      return false;

   if (!server)
   {
      for (U32 i = 0; i < MaxIncludedStates; ++i)
         if (!sfxResolve(&mIncludedStates[i], (char&)errorStr))
            return false;

      for (U32 i = 0; i < MaxExcludedStates; ++i)
         if (!sfxResolve(&mExcludedStates[i], (char&)errorStr))
            return false;
   }

   return true;
}

//-----------------------------------------------------------------------------

void SFXState::packData(BitStream* stream)
{
   Parent::packData(stream);

   for (U32 i = 0; i < MaxIncludedStates; ++i)
      sfxWrite(stream, mIncludedStates[i]);
   for (U32 i = 0; i < MaxExcludedStates; ++i)
      sfxWrite(stream, mExcludedStates[i]);
}

//-----------------------------------------------------------------------------

void SFXState::unpackData(BitStream* stream)
{
   Parent::unpackData(stream);

   for (U32 i = 0; i < MaxIncludedStates; ++i)
      sfxRead(stream, &mIncludedStates[i]);
   for (U32 i = 0; i < MaxExcludedStates; ++i)
      sfxRead(stream, &mExcludedStates[i]);
}

//-----------------------------------------------------------------------------

void SFXState::_onActivate()
{
#ifdef DEBUG_SPEW
   Platform::outputDebugString("[SFXState] Activating '%s'", getName());
#endif

   //onActivate_callback();

   // Add the state to the list.

   sgActiveStates.push_back(this);

   // Activate included states.

   for (U32 i = 0; i < MaxIncludedStates; ++i)
      if (mIncludedStates[i])
         mIncludedStates[i]->activate();

   // Disable excluded states.

   for (U32 i = 0; i < MaxExcludedStates; ++i)
      if (mExcludedStates[i])
         mExcludedStates[i]->disable();
}

//-----------------------------------------------------------------------------

void SFXState::_onDeactivate()
{
#ifdef DEBUG_SPEW
   Platform::outputDebugString("[SFXState] Deactivating '%s'", getName());
#endif

   //onDeactivate_callback();

   // Remove the state from the list.

   for (U32 i = 0; i < sgActiveStates.size(); ++i)
      if (sgActiveStates[i] == this)
      {
         sgActiveStates.erase_fast(i);
         break;
      }

   // Deactivate included states.

   for (U32 i = 0; i < MaxIncludedStates; ++i)
      if (mIncludedStates[i])
         mIncludedStates[i]->deactivate();

   // Enable excluded states.

   for (U32 i = 0; i < MaxExcludedStates; ++i)
      if (mExcludedStates[i])
         mExcludedStates[i]->enable();
}