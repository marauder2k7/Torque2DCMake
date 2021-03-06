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

#include "platformX86UNIX/platformX86UNIX.h"
#include "console/consoleTypes.h"
#include "platform/event.h"
#include "game/gameInterface.h"
#include "platformX86UNIX/x86UNIXState.h"
#include "platformX86UNIX/x86UNIXInputManager.h"
#include "math/mMathFn.h"

#include <SDL2/SDL.h>

// keymap table
static const U32 SDLtoTKeyMapSize = NUM_KEYS;
static U32 T2DtoSDLKey[SDLtoTKeyMapSize];
static U32 SDLtoTKeyMap[SDLtoTKeyMapSize];
static bool keyMapsInitialized = false;
U32 getTorqueMod(U16 mod);
AsciiData AsciiTable[NUM_KEYS];

// helper functions
static void MapKey(U32 SDLkey, U32 tkey);
static void InitKeyMaps();

// unix platform state
extern x86UNIXPlatformState * x86UNIXState;

// constants

static const U32 MouseMask = SDL_MOUSEMOTION;
static const U32 KeyboardMask = SDL_KEYUP | SDL_KEYDOWN;
static const U32 JoystickMask = SDL_JOYAXISMOTION;

static const U32 AllInputEvents = MouseMask | KeyboardMask | JoystickMask;

extern "C" U16 X11_KeyToUnicode( U16 keysym, U16 modifiers );

//==============================================================================
// Static helper functions
//==============================================================================
static void MapKey(U32 SDLkey, U32 tkey)
{
   SDLtoTKeyMap[SDLkey] = tkey;
   T2DtoSDLKey[tkey] = SDLkey;

}

U32 getTorqueScanCodeFromSDL(U32 sdl)
{
    return SDLtoTKeyMap[sdl];
}

U32 getSDLScanCodeFromTorque(U32 torque)
{
    return T2DtoSDLKey[torque];
}

U32 getTorqueMod(U16 mod)
{
    U32 ret = 0;

    if (mod & KMOD_LSHIFT)
    {
        ret |= SI_LSHIFT;
        ret |= SI_SHIFT;
    }

    if (mod & KMOD_RSHIFT)
    {
        ret |= SI_RSHIFT;
        ret |= SI_SHIFT;
    }

    if (mod & KMOD_LCTRL)
    {
        ret |= SI_LCTRL;
        ret |= SI_CTRL;
    }

    if (mod & KMOD_RCTRL)
    {
        ret |= SI_RCTRL;
        ret |= SI_CTRL;
    }

    if (mod & KMOD_LALT)
    {
        ret |= SI_LALT;
        ret |= SI_ALT;
    }

    if (mod & KMOD_RALT)
    {
        ret |= SI_RALT;
        ret |= SI_ALT;
    }

    return ret;
}

//------------------------------------------------------------------------------
void InitKeyMaps()
{

    for(int i = 0; i < SDLtoTKeyMapSize; ++i)
    {
        SDLtoTKeyMap[i] = 0;
        T2DtoSDLKey[i] = 0;
    }

   // SDL, Torque
   MapKey(SDL_SCANCODE_A, KEY_A);
   MapKey(SDL_SCANCODE_B, KEY_B);
   MapKey(SDL_SCANCODE_C, KEY_C);
   MapKey(SDL_SCANCODE_D, KEY_D);
   MapKey(SDL_SCANCODE_E, KEY_E);
   MapKey(SDL_SCANCODE_F, KEY_F);
   MapKey(SDL_SCANCODE_G, KEY_G);
   MapKey(SDL_SCANCODE_H, KEY_H);
   MapKey(SDL_SCANCODE_I, KEY_I);
   MapKey(SDL_SCANCODE_J, KEY_J);
   MapKey(SDL_SCANCODE_K, KEY_K);
   MapKey(SDL_SCANCODE_L, KEY_L);
   MapKey(SDL_SCANCODE_M, KEY_M);
   MapKey(SDL_SCANCODE_N, KEY_N);
   MapKey(SDL_SCANCODE_O, KEY_O);
   MapKey(SDL_SCANCODE_P, KEY_P);
   MapKey(SDL_SCANCODE_Q, KEY_Q);
   MapKey(SDL_SCANCODE_R, KEY_R);
   MapKey(SDL_SCANCODE_S, KEY_S);
   MapKey(SDL_SCANCODE_T, KEY_T);
   MapKey(SDL_SCANCODE_U, KEY_U);
   MapKey(SDL_SCANCODE_V, KEY_V);
   MapKey(SDL_SCANCODE_W, KEY_W);
   MapKey(SDL_SCANCODE_X, KEY_X);
   MapKey(SDL_SCANCODE_Y, KEY_Y);
   MapKey(SDL_SCANCODE_Z, KEY_Z);

   MapKey(SDL_SCANCODE_1, KEY_1);
   MapKey(SDL_SCANCODE_2, KEY_2);
   MapKey(SDL_SCANCODE_3, KEY_3);
   MapKey(SDL_SCANCODE_4, KEY_4);
   MapKey(SDL_SCANCODE_5, KEY_5);
   MapKey(SDL_SCANCODE_6, KEY_6);
   MapKey(SDL_SCANCODE_7, KEY_7);
   MapKey(SDL_SCANCODE_8, KEY_8);
   MapKey(SDL_SCANCODE_9, KEY_9);
   MapKey(SDL_SCANCODE_0, KEY_0);

   MapKey(SDL_SCANCODE_BACKSPACE, KEY_BACKSPACE);
   MapKey(SDL_SCANCODE_TAB, KEY_TAB);
   MapKey(SDL_SCANCODE_RETURN, KEY_RETURN);
   MapKey(SDL_SCANCODE_LCTRL, KEY_LCONTROL);
   MapKey(SDL_SCANCODE_RCTRL, KEY_RCONTROL);
   MapKey(SDL_SCANCODE_LALT, KEY_LALT);
   MapKey(SDL_SCANCODE_RALT, KEY_RALT);
   MapKey(SDL_SCANCODE_LSHIFT, KEY_LSHIFT);
   MapKey(SDL_SCANCODE_RSHIFT, KEY_RSHIFT);
   MapKey(SDL_SCANCODE_PAUSE, KEY_PAUSE);
   MapKey(SDL_SCANCODE_CAPSLOCK, KEY_CAPSLOCK);
   MapKey(SDL_SCANCODE_ESCAPE, KEY_ESCAPE);
   MapKey(SDL_SCANCODE_SPACE, KEY_SPACE);
   MapKey(SDL_SCANCODE_PAGEDOWN, KEY_PAGE_DOWN);
   MapKey(SDL_SCANCODE_PAGEUP, KEY_PAGE_UP);
   MapKey(SDL_SCANCODE_END, KEY_END);
   MapKey(SDL_SCANCODE_HOME, KEY_HOME);
   MapKey(SDL_SCANCODE_LEFT, KEY_LEFT);
   MapKey(SDL_SCANCODE_UP, KEY_UP);
   MapKey(SDL_SCANCODE_RIGHT, KEY_RIGHT);
   MapKey(SDL_SCANCODE_DOWN, KEY_DOWN);
   MapKey(SDL_SCANCODE_PRINTSCREEN, KEY_PRINT);
   MapKey(SDL_SCANCODE_INSERT, KEY_INSERT);
   MapKey(SDL_SCANCODE_DELETE, KEY_DELETE);
   MapKey(SDL_SCANCODE_HELP, KEY_HELP);

   MapKey(SDL_SCANCODE_GRAVE, KEY_TILDE);
   MapKey(SDL_SCANCODE_MINUS, KEY_MINUS);
   MapKey(SDL_SCANCODE_EQUALS, KEY_EQUALS);
   MapKey(SDL_SCANCODE_LEFTBRACKET, KEY_LBRACKET);
   MapKey(SDL_SCANCODE_RIGHTBRACKET, KEY_RBRACKET);
   MapKey(SDL_SCANCODE_BACKSLASH, KEY_BACKSLASH);
   MapKey(SDL_SCANCODE_SEMICOLON, KEY_SEMICOLON);
   MapKey(SDL_SCANCODE_APOSTROPHE, KEY_APOSTROPHE);
   MapKey(SDL_SCANCODE_COMMA, KEY_COMMA);
   MapKey(SDL_SCANCODE_PERIOD, KEY_PERIOD);
   MapKey(SDL_SCANCODE_SLASH, KEY_SLASH);
   MapKey(SDL_SCANCODE_KP_0, KEY_NUMPAD0);
   MapKey(SDL_SCANCODE_KP_1, KEY_NUMPAD1);
   MapKey(SDL_SCANCODE_KP_2, KEY_NUMPAD2);
   MapKey(SDL_SCANCODE_KP_3, KEY_NUMPAD3);
   MapKey(SDL_SCANCODE_KP_4, KEY_NUMPAD4);
   MapKey(SDL_SCANCODE_KP_5, KEY_NUMPAD5);
   MapKey(SDL_SCANCODE_KP_6, KEY_NUMPAD6);
   MapKey(SDL_SCANCODE_KP_7, KEY_NUMPAD7);
   MapKey(SDL_SCANCODE_KP_8, KEY_NUMPAD8);
   MapKey(SDL_SCANCODE_KP_9, KEY_NUMPAD9);
   MapKey(SDL_SCANCODE_KP_MULTIPLY, KEY_MULTIPLY);
   MapKey(SDL_SCANCODE_KP_PLUS, KEY_ADD);
   MapKey(SDL_SCANCODE_KP_EQUALS, KEY_SEPARATOR);
   MapKey(SDL_SCANCODE_KP_MINUS, KEY_SUBTRACT);
   MapKey(SDL_SCANCODE_KP_PERIOD, KEY_DECIMAL);
   MapKey(SDL_SCANCODE_KP_DIVIDE, KEY_DIVIDE);
   MapKey(SDL_SCANCODE_KP_ENTER, KEY_NUMPADENTER);

   MapKey(SDL_SCANCODE_F1, KEY_F1);
   MapKey(SDL_SCANCODE_F2, KEY_F2);
   MapKey(SDL_SCANCODE_F3, KEY_F3);
   MapKey(SDL_SCANCODE_F4, KEY_F4);
   MapKey(SDL_SCANCODE_F5, KEY_F5);
   MapKey(SDL_SCANCODE_F6, KEY_F6);
   MapKey(SDL_SCANCODE_F7, KEY_F7);
   MapKey(SDL_SCANCODE_F8, KEY_F8);
   MapKey(SDL_SCANCODE_F9, KEY_F9);
   MapKey(SDL_SCANCODE_F10, KEY_F10);
   MapKey(SDL_SCANCODE_F11, KEY_F11);
   MapKey(SDL_SCANCODE_F12, KEY_F12);
   MapKey(SDL_SCANCODE_F13, KEY_F13);
   MapKey(SDL_SCANCODE_F14, KEY_F14);
   MapKey(SDL_SCANCODE_F15, KEY_F15);
   MapKey(SDL_SCANCODE_F16, KEY_F16);
   MapKey(SDL_SCANCODE_F17, KEY_F17);
   MapKey(SDL_SCANCODE_F18, KEY_F18);
   MapKey(SDL_SCANCODE_F19, KEY_F19);
   MapKey(SDL_SCANCODE_F20, KEY_F20);
   MapKey(SDL_SCANCODE_F21, KEY_F21);
   MapKey(SDL_SCANCODE_F22, KEY_F22);
   MapKey(SDL_SCANCODE_F23, KEY_F23);
   MapKey(SDL_SCANCODE_F24, KEY_F24);

   MapKey(SDL_SCANCODE_LCTRL, KEY_CONTROL);
   MapKey(SDL_SCANCODE_LALT, KEY_ALT);
   MapKey(SDL_SCANCODE_LSHIFT, KEY_SHIFT);


   keyMapsInitialized = true;
};

//------------------------------------------------------------------------------
// this shouldn't be used, use TranslateSDLKeytoTKey instead
U8 TranslateOSKeyCode(U8 vcode)
{
   Con::printf("WARNING: TranslateOSKeyCode is not supported in unix");
   return 0;
}

//==============================================================================
// UInputManager
//==============================================================================
UInputManager::UInputManager()
{
   mActive = false;
   mEnabled = false;
   mLocking = true; // locking enabled by default
   mKeyboardEnabled = mMouseEnabled = mJoystickEnabled = false;
   mKeyboardActive = mMouseActive = mJoystickActive = false;
}

//------------------------------------------------------------------------------
void UInputManager::init()
{
   Con::addVariable( "pref::Input::KeyboardEnabled",
      TypeBool, &mKeyboardEnabled );
   Con::addVariable( "pref::Input::MouseEnabled",
      TypeBool, &mMouseEnabled );
   Con::addVariable( "pref::Input::JoystickEnabled",
      TypeBool, &mJoystickEnabled );
}

//------------------------------------------------------------------------------
bool UInputManager::enable()
{
   disable();
#ifdef LOG_INPUT
   Input::log( "Enabling Input...\n" );
#endif

   mModifierKeys = 0;
   dMemset( mMouseButtonState, 0, sizeof( mMouseButtonState ) );
   dMemset( mKeyboardState, 0, 256 );

   InitKeyMaps();

   mJoystickEnabled = false;
   initJoystick();

   mEnabled = true;
   mMouseEnabled = true;
   mKeyboardEnabled = true;

   return true;
}

//------------------------------------------------------------------------------
void UInputManager::disable()
{
   deactivate();
   mEnabled = false;
   return;
}

//------------------------------------------------------------------------------
void UInputManager::initJoystick()
{
   mJoystickList.clear();

   // initialize SDL joystick system
   if (SDL_InitSubSystem(SDL_INIT_JOYSTICK) < 0)
   {
      Con::warnf("   Unable to initialize joystick: %s", SDL_GetError());
      return;
   }

   int numJoysticks = SDL_NumJoysticks();
   if (numJoysticks == 0)
      Con::printf("   No joysticks found.");

   // disable joystick events (use polling instead)
   SDL_JoystickEventState(SDL_IGNORE);

   // install joysticks
   for(int i = 0; i < numJoysticks; i++ )
   {
      JoystickInputDevice* newDevice = new JoystickInputDevice(i);
      addObject(newDevice);
      mJoystickList.push_back(newDevice);
      Con::printf("   %s: %s",
         newDevice->getDeviceName(), newDevice->getName());
#ifdef LOG_INPUT
      Input::log("   %s: %s\n",
         newDevice->getDeviceName(), newDevice->getName());
#endif
   }

   mJoystickEnabled = true;
}

//------------------------------------------------------------------------------
void UInputManager::activate()
{
   if (mEnabled && !isActive())
   {
      mActive = true;
      SDL_ShowCursor(SDL_DISABLE);
      resetInputState();
      // hack; if the mouse or keyboard has been disabled, re-enable them.
      // prevents scripts like default.cs from breaking our input, although
      // there is probably a better solution
      mMouseEnabled = mKeyboardEnabled = true;
      activateMouse();
      activateKeyboard();
      activateJoystick();
      if (x86UNIXState->windowLocked())
         lockInput();
   }
}

//------------------------------------------------------------------------------
void UInputManager::deactivate()
{
   if (mEnabled && isActive())
   {
      unlockInput();
      deactivateKeyboard();
      deactivateMouse();
      deactivateJoystick();
      resetInputState();
      SDL_ShowCursor(SDL_ENABLE);
      mActive = false;
   }
}

//------------------------------------------------------------------------------
void UInputManager::resetKeyboardState()
{
   // unpress any pressed keys; in the future we may want
   // to actually sync with the keyboard state
   for (int i = 0; i < 256; ++i)
   {
      if (mKeyboardState[i])
      {
         InputEvent event;

         event.deviceInst = 0;
         event.deviceType = KeyboardDeviceType;
         event.objType = SI_KEY;
         event.objInst = i;
         event.action = SI_BREAK;
         event.fValues[0] = 0.0;
         Game->postEvent(event);
      }
   }
   dMemset(mKeyboardState, 0, 256);

   // clear modifier keys
   mModifierKeys = 0;
}

//------------------------------------------------------------------------------
void UInputManager::resetMouseState()
{
   // unpress any buttons; in the future we may want
   // to actually sync with the mouse state
   for (int i = 0; i < 3; ++i)
   {
      if (mMouseButtonState[i])
      {
         // add KEY_BUTTON0 to the index to get the real
         // button ID
         S32 buttonID = i + KEY_BUTTON0;
         InputEvent event;

         event.deviceInst = 0;
         event.deviceType = MouseDeviceType;
         event.objType = SI_BUTTON;
         event.objInst = buttonID;
         event.action = SI_BREAK;
         event.fValues[0] = 0.0;
         Game->postEvent(event);
      }
   }

   dMemset(mMouseButtonState, 0, 3);
}

//------------------------------------------------------------------------------
void UInputManager::resetInputState()
{
   resetKeyboardState();
   resetMouseState();
   // reset joysticks
   for (Vector<JoystickInputDevice*>::iterator iter = mJoystickList.begin();
        iter != mJoystickList.end();
        ++iter)
   {
      (*iter)->reset();
   }

   // JMQTODO: make event arrays be members
   // dispose of any lingering SDL input events
   static const int MaxEvents = 255;
   static SDL_Event events[MaxEvents];
   SDL_PumpEvents();

   SDL_PeepEvents(events,MaxEvents,SDL_GETEVENT,SDL_FIRSTEVENT, SDL_LASTEVENT);

}

//------------------------------------------------------------------------------
void UInputManager::setLocking(bool enabled)
{
   mLocking = enabled;
   if (mLocking)
      lockInput();
   else
      unlockInput();
}

//------------------------------------------------------------------------------
void UInputManager::lockInput()
{
   if (x86UNIXState->windowActive() && x86UNIXState->windowLocked() &&
      mLocking && !SDL_GetWindowGrab(x86UNIXState->getSdlWindow()))
      {
        SDL_SetWindowGrab(x86UNIXState->getSdlWindow(),SDL_bool(SDL_TRUE));
        SDL_SetRelativeMouseMode(SDL_TRUE);
      }
}

//------------------------------------------------------------------------------
void UInputManager::unlockInput()
{
    if (SDL_GetWindowGrab(x86UNIXState->getSdlWindow()))
    {
        SDL_SetWindowGrab(x86UNIXState->getSdlWindow(),SDL_bool(SDL_FALSE));
        SDL_SetRelativeMouseMode(SDL_FALSE);
    }
}


//------------------------------------------------------------------------------
void UInputManager::onDeleteNotify( SimObject* object )
{
   Parent::onDeleteNotify( object );
}

//------------------------------------------------------------------------------
bool UInputManager::onAdd()
{
   if ( !Parent::onAdd() )
      return false;

   return true;
}

//------------------------------------------------------------------------------
void UInputManager::onRemove()
{
   deactivate();
   Parent::onRemove();
}

//------------------------------------------------------------------------------
void UInputManager::joyButtonEvent(const SDL_Event& event)
{
   joyButtonEvent(event.jbutton.which, event.jbutton.button,
      event.type == SDL_JOYBUTTONDOWN);
}

//------------------------------------------------------------------------------
void UInputManager::joyButtonEvent(U8 deviceID, U8 buttonNum, bool pressed)

{
   S32 action = pressed ? SI_MAKE : SI_BREAK;
   S32 objInst = buttonNum + KEY_BUTTON0;

   InputEvent ievent;

   ievent.deviceInst = deviceID;
   ievent.deviceType = JoystickDeviceType;
   ievent.modifier = mModifierKeys;
   ievent.ascii = 0;
   ievent.objType = SI_BUTTON;
   ievent.objInst = objInst;
   ievent.action = action;
   ievent.fValues[0] = (action == SI_MAKE) ? 1.0 : 0.0;

   Game->postEvent(ievent);
#ifdef LOG_INPUT
   Input::log( "EVENT (Input): joystick%d button%d %s. MODS:%c%c%c \n",
      deviceID,
      buttonNum,
      pressed ? "pressed" : "released",
      ( mModifierKeys & SI_SHIFT ? 'S' : '.' ),
      ( mModifierKeys & SI_CTRL ? 'C' : '.' ),
      ( mModifierKeys & SI_ALT ? 'A' : '.' ));
#endif
}

//------------------------------------------------------------------------------
void UInputManager::joyHatEvent(U8 deviceID, U8 hatNum,
   U8 prevHatState, U8 currHatState)
{
   if (prevHatState == currHatState)
      return;

   InputEvent ievent;

   ievent.deviceInst = deviceID;
   ievent.deviceType = JoystickDeviceType;
   ievent.modifier = mModifierKeys;
   ievent.ascii = 0;
   ievent.objType = SI_POV;

   // first break any positions that are no longer valid
   ievent.action = SI_BREAK;
   ievent.fValues[0] = 0.0;

   if (prevHatState & SDL_HAT_UP && !(currHatState & SDL_HAT_UP))
   {
#ifdef LOG_INPUT
      Input::log( "EVENT (Input): Up POV released.\n");
#endif
      ievent.objInst = SI_UPOV;
      Game->postEvent(ievent);
   }
   else if (prevHatState & SDL_HAT_DOWN && !(currHatState & SDL_HAT_DOWN))
   {
#ifdef LOG_INPUT
      Input::log( "EVENT (Input): Down POV released.\n");
#endif
      ievent.objInst = SI_DPOV;
      Game->postEvent(ievent);
   }
   if (prevHatState & SDL_HAT_LEFT && !(currHatState & SDL_HAT_LEFT))
   {
#ifdef LOG_INPUT
      Input::log( "EVENT (Input): Left POV released.\n");
#endif
      ievent.objInst = SI_LPOV;
      Game->postEvent(ievent);
   }
   else if (prevHatState & SDL_HAT_RIGHT && !(currHatState & SDL_HAT_RIGHT))
   {
#ifdef LOG_INPUT
      Input::log( "EVENT (Input): Right POV released.\n");
#endif
      ievent.objInst = SI_RPOV;
      Game->postEvent(ievent);
   }

   // now do the make events
   ievent.action = SI_MAKE;
   ievent.fValues[0] = 1.0;

   if (!(prevHatState & SDL_HAT_UP) && currHatState & SDL_HAT_UP)
   {
#ifdef LOG_INPUT
      Input::log( "EVENT (Input): Up POV pressed.\n");
#endif
      ievent.objInst = SI_UPOV;
      Game->postEvent(ievent);
   }
   else if (!(prevHatState & SDL_HAT_DOWN) && currHatState & SDL_HAT_DOWN)
   {
#ifdef LOG_INPUT
      Input::log( "EVENT (Input): Down POV pressed.\n");
#endif
      ievent.objInst = SI_DPOV;
      Game->postEvent(ievent);
   }
   if (!(prevHatState & SDL_HAT_LEFT) && currHatState & SDL_HAT_LEFT)
   {
#ifdef LOG_INPUT
      Input::log( "EVENT (Input): Left POV pressed.\n");
#endif
      ievent.objInst = SI_LPOV;
      Game->postEvent(ievent);
   }
   else if (!(prevHatState & SDL_HAT_RIGHT) && currHatState & SDL_HAT_RIGHT)
   {
#ifdef LOG_INPUT
      Input::log( "EVENT (Input): Right POV pressed.\n");
#endif
      ievent.objInst = SI_RPOV;
      Game->postEvent(ievent);
   }
}

//------------------------------------------------------------------------------
void UInputManager::joyAxisEvent(const SDL_Event& event)
{
   joyAxisEvent(event.jaxis.which, event.jaxis.axis, event.jaxis.value);
}

//------------------------------------------------------------------------------
void UInputManager::joyAxisEvent(U8 deviceID, U8 axisNum, S16 axisValue)
{
   JoystickInputDevice* stick;

   stick = mJoystickList[deviceID];
   AssertFatal(stick, "JoystickInputDevice* is NULL");
   JoystickAxisInfo axisInfo = stick->getAxisInfo(axisNum);

   if (axisInfo.type == -1)
      return;

   // scale the value to [-1,1]
   F32 scaledValue = 0;
   if (axisValue < 0)
      scaledValue = -F32(axisValue) / axisInfo.minValue;
   else if (axisValue > 0)
      scaledValue = F32(axisValue) / axisInfo.maxValue;

//    F32 range = F32(axisInfo.maxValue - axisInfo.minValue);
//    F32 scaledValue = F32((2 * axisValue) - axisInfo.maxValue -
//       axisInfo.minValue) / range;

   if (scaledValue > 1.f)
      scaledValue = 1.f;
   else if (scaledValue < -1.f)
      scaledValue = -1.f;

   // create and post the event
   InputEvent ievent;

   ievent.deviceInst = deviceID;
   ievent.deviceType = JoystickDeviceType;
   ievent.modifier = mModifierKeys;
   ievent.ascii = 0;
   ievent.objType = axisInfo.type;
   ievent.objInst = 0;
   ievent.action = SI_MOVE;
   ievent.fValues[0] = scaledValue;

   Game->postEvent(ievent);

#ifdef LOG_INPUT
      Input::log( "EVENT (Input): joystick axis %d moved: %.1f.\n",
         axisNum, ievent.fValues[0]);
#endif

}


//------------------------------------------------------------------------------
// MOUSE EVENTS
//------------------------------------------------------------------------------

void UInputManager::mouseMotionEvent(const SDL_Event& event)
{
//    Con::printf("motion event: %d %d %d %d",
//       event.motion.xrel, event.motion.yrel,
//       event.motion.x, event.motion.y);
   if (x86UNIXState->windowLocked())
   {
      InputEvent ievent;
      ievent.deviceInst = 0;
      ievent.deviceType = MouseDeviceType;
      ievent.objInst = 0;
      ievent.modifier = mModifierKeys;
      ievent.ascii = 0;
      ievent.action = SI_MOVE;

      // post events if things have changed
      if (event.motion.xrel != 0)
      {
         ievent.objType = SI_XAXIS;
         ievent.fValues[0] = event.motion.xrel;
         Game->postEvent(ievent);
      }
      if (event.motion.yrel != 0)
      {
         ievent.objType = SI_YAXIS;
         ievent.fValues[0] = event.motion.yrel;
         Game->postEvent(ievent);
      }
#ifdef LOG_INPUT
#ifdef LOG_MOUSEMOVE
         Input::log( "EVENT (Input): Mouse relative move (%.1f, %.1f).\n",
            event.motion.xrel != 0 ? F32(event.motion.xrel) : 0.0,
            event.motion.yrel != 0 ? F32(event.motion.yrel) : 0.0);
#endif
#endif
   }
   else
   {
      MouseMoveEvent mmevent;
      mmevent.xPos = mLastMouseX = event.motion.x;
      mmevent.yPos = mLastMouseY = event.motion.y;
      mmevent.modifier = mModifierKeys;
      Game->postEvent(mmevent);
#ifdef LOG_INPUT
#ifdef LOG_MOUSEMOVE
         Input::log( "EVENT (Input): Mouse absolute move (%.1f, %.1f).\n",
            F32(event.motion.x),
            F32(event.motion.y));
#endif
#endif
   }
}

//------------------------------------------------------------------------------

void UInputManager::mouseWheelEvent(const SDL_Event& event)
{

    U32 mods = getTorqueMod(SDL_GetModState());
    S32 wheel = Con::getIntVariable("$pref::Input::MouseWheelSpeed",120);
    S32 wheelXDelta = event.wheel.x * wheel;
    S32 wheelYDelta = event.wheel.y * wheel;

    InputEvent ievent;
    ievent.deviceType = MouseDeviceType;
    ievent.objInst = 0;

    if(wheelYDelta)
    {
        ievent.objType = SI_ZAXIS;
        ievent.fValues[0] = (F32)wheelYDelta;
        Game->postEvent(ievent);
    }

    if(wheelXDelta)
    {
        ievent.objType = SI_RZAXIS;
        ievent.fValues[0] = (F32)wheelXDelta;
        Game->postEvent(ievent);
    }

}
//------------------------------------------------------------------------------
void UInputManager::mouseButtonEvent(const SDL_Event& event)
{
   S32 action = (event.type == SDL_MOUSEBUTTONDOWN) ? SI_MAKE : SI_BREAK;
   S32 objInst = -1;

   switch (event.button.button)
   {
      case SDL_BUTTON_LEFT:
         objInst = KEY_BUTTON0;
         break;
      case SDL_BUTTON_RIGHT:
         objInst = KEY_BUTTON1;
         break;
      case SDL_BUTTON_MIDDLE:
         objInst = KEY_BUTTON2;
         break;
   }

   if (objInst == -1)
      // unsupported button
      return;

   InputEvent ievent;

   ievent.deviceInst = 0;
   ievent.deviceType = MouseDeviceType;
   ievent.modifier = mModifierKeys;
   ievent.ascii = 0;

      S32 buttonID = (objInst - KEY_BUTTON0);
      if (buttonID < 3)
         mMouseButtonState[buttonID] = ( action == SI_MAKE ) ? true : false;

      ievent.objType = SI_BUTTON;
      ievent.objInst = objInst;
      ievent.action = action;
      ievent.fValues[0] = (action == SI_MAKE) ? 1.0 : 0.0;
#ifdef LOG_INPUT
      Input::log( "EVENT (Input): mouse button%d %s. MODS:%c%c%c\n",
         buttonID,
         action == SI_MAKE ? "pressed" : "released",
         ( mModifierKeys & SI_SHIFT ? 'S' : '.' ),
         ( mModifierKeys & SI_CTRL ? 'C' : '.' ),
         ( mModifierKeys & SI_ALT ? 'A' : '.' ));
#endif
   Game->postEvent(ievent);
}

//------------------------------------------------------------------------------
// KEYBOARD EVENTS
//------------------------------------------------------------------------------

void UInputManager::textEvent(const SDL_Event& event)
{

    U32 mod = getTorqueMod(SDL_GetModState());
    if(event.text.text[1])
    {
        U16 wchar = event.text.text[0];
        processTextEvent(mod,wchar);
        return;
    }
    else
    {
        const U32 len = strlen(event.text.text);
        U16 wchar[16] = {};
        dMemcpy(wchar, event.text.text, sizeof(char)*len);

        for(int i = 0; i < 16; ++i)
        {
            if(!wchar[i])
                return;

            processTextEvent(mod,wchar[i]);

        }
    }
}

//------------------------------------------------------------------------------

void UInputManager::processTextEvent(U32 mod, U16 key)
{

    InputEvent ievent;
    ievent.deviceInst = 0;
    ievent.deviceType = KeyboardDeviceType;
    ievent.objType = SI_KEY;
    ievent.objInst = KEY_NULL;
    ievent.modifier = mod;
    ievent.ascii = key;
    ievent.action = SI_MAKE;
    ievent.fValues[0] = 1.0f;
    Game->postEvent(ievent);

    ievent.action = SI_BREAK;
    ievent.fValues[0] = 0.0f;
    Game->postEvent(ievent);

}

//------------------------------------------------------------------------------

void UInputManager::keyEvent(const SDL_Event& event)
{
   InputEvent ievent;

   S32 iA = SI_MAKE;
   SDL_Keysym tKey = event.key.keysym;

   if(event.type == SDL_KEYUP)
   {
        iA = SI_BREAK;
   }

   if(event.key.repeat)
   {
        iA = SI_REPEAT;
   }

   U32 torqueMod = getTorqueMod(event.key.keysym.mod);
   U32 torqueKey = getTorqueScanCodeFromSDL(tKey.scancode);

   ievent.deviceInst = 0;
   ievent.deviceType = KeyboardDeviceType;
   ievent.objType = SI_KEY;
   ievent.objInst = (InputObjectInstances)torqueKey;
   ievent.modifier = torqueMod;
   // if the action is a make but this key is already pressed,
   // count it as a repeat
   if (iA == SI_MAKE && mKeyboardState[ievent.objInst])
      iA = SI_REPEAT;

   ievent.action = iA;
   switch(iA)
   {
        case SI_MAKE:
            ievent.fValues[0] = 1.0f;
        case SI_REPEAT:
            ievent.fValues[0] = 1.0f;
        case SI_BREAK:
            ievent.fValues[0] = 0.0f;
   }

   KEY_STATE state = STATE_LOWER;
   if (ievent.modifier & (SI_CTRL|SI_ALT) )
   {
      state = STATE_GOOFY;
   }
   if ( ievent.modifier & SI_SHIFT )
   {
      state = STATE_UPPER;
   }

   ievent.ascii = Input::getAscii( ievent.objInst, state );

   //processKeyEvent(ievent);
   Game->postEvent(ievent);


}

//------------------------------------------------------------------------------
// This function was ripped from DInputDevice almost entirely intact.
bool UInputManager::processKeyEvent( InputEvent &event )
{
   if ( event.deviceType != KeyboardDeviceType || event.objType != SI_KEY )
      return false;

   KEY_STATE state = STATE_LOWER;
   if (event.modifier & (SI_CTRL|SI_ALT) )
   {
      state = STATE_GOOFY;
   }
   if ( event.modifier & SI_SHIFT )
   {
      state = STATE_UPPER;
   }

   event.ascii = Input::getAscii( event.objInst, state );

   return true;
}

//------------------------------------------------------------------------------
// WINDOW STATE
//------------------------------------------------------------------------------
void UInputManager::setWindowLocked(bool locked)
{
   if (locked)
      lockInput();
   else
   {
      unlockInput();
      // SDL keeps track of abs mouse position in fullscreen mode, which means
      // that if you switch to unlocked mode while fullscreen, the mouse will
      // suddenly warp to someplace unexpected on screen.  To fix this, we
      // warp the mouse to the last known Torque abs mouse position.
      if (mLastMouseX != -1 && mLastMouseY != -1)
         SDL_WarpMouseInWindow(x86UNIXState->getSdlWindow(),mLastMouseX, mLastMouseY);
   }
}

//------------------------------------------------------------------------------
void UInputManager::process()
{
   if (!mEnabled || !isActive())
      return;

   // JMQTODO: make these be class members
   static const int MaxEvents = 255;
   static SDL_Event events[MaxEvents];

   U32 mask = 0;

   // process keyboard and mouse events
   if (mMouseActive)
      mask |= MouseMask;
   if (mKeyboardActive)
      mask |= KeyboardMask;

   if (mask != 0)
   {
      SDL_PumpEvents();
      S32 numEvents = SDL_PeepEvents(events, MaxEvents, SDL_GETEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT);

      for (int i = 0; i < numEvents; ++i)
      {
         switch (events[i].type)
         {
            case SDL_MOUSEMOTION:
               mouseMotionEvent(events[i]);
               break;
            case SDL_MOUSEWHEEL:
                mouseWheelEvent(events[i]);
                break;
            case SDL_MOUSEBUTTONUP:
            case SDL_MOUSEBUTTONDOWN:
               mouseButtonEvent(events[i]);
               break;
            case SDL_KEYDOWN:
            case SDL_KEYUP:
               keyEvent(events[i]);
               break;
            //case SDL_TEXTINPUT:
            //   textEvent(events[i]);
            //   break;
         }
      }
   }

   // poll joysticks
   if (!mJoystickActive)
      return;

   SDL_JoystickUpdate();

   for (Vector<JoystickInputDevice*>::iterator iter = mJoystickList.begin();
        iter != mJoystickList.end();
        ++iter)
   {
      (*iter)->process();
   }
}

//------------------------------------------------------------------------------
bool UInputManager::enableKeyboard()
{
   if ( !isEnabled() )
      return( false );

   if ( isKeyboardEnabled() && isKeyboardActive() )
      return( true );

   mKeyboardEnabled = true;
   if ( isActive() )
      mKeyboardEnabled = activateKeyboard();

   if ( mKeyboardEnabled )
   {
      Con::printf( "Keyboard enabled." );
#ifdef LOG_INPUT
      Input::log( "Keyboard enabled.\n" );
#endif
   }
   else
   {
      Con::warnf( "Keyboard failed to enable!" );
#ifdef LOG_INPUT
      Input::log( "Keyboard failed to enable!\n" );
#endif
   }

   return( mKeyboardEnabled );
}

//------------------------------------------------------------------------------
void UInputManager::disableKeyboard()
{
   if ( !isEnabled() || !isKeyboardEnabled())
      return;

   deactivateKeyboard();
   mKeyboardEnabled = false;
   Con::printf( "Keyboard disabled." );
#ifdef LOG_INPUT
   Input::log( "Keyboard disabled.\n" );
#endif
}

//------------------------------------------------------------------------------
bool UInputManager::activateKeyboard()
{
   if ( !isEnabled() || !isActive() || !isKeyboardEnabled() )
      return( false );

   mKeyboardActive = true;
#ifdef LOG_INPUT
   Input::log( mKeyboardActive ? "Keyboard activated.\n" : "Keyboard failed to activate!\n" );
#endif
   return( mKeyboardActive );
}

//------------------------------------------------------------------------------
void UInputManager::deactivateKeyboard()
{
   if ( isEnabled() && isKeyboardActive() )
   {
      mKeyboardActive = false;
#ifdef LOG_INPUT
      Input::log( "Keyboard deactivated.\n" );
#endif
   }
}

//------------------------------------------------------------------------------
bool UInputManager::enableMouse()
{
   if ( !isEnabled() )
      return( false );

   if ( isMouseEnabled() && isMouseActive() )
      return( true );

   mMouseEnabled = true;
   if ( isActive() )
      mMouseEnabled = activateMouse();

   if ( mMouseEnabled )
   {
      Con::printf( "Mouse enabled." );
#ifdef LOG_INPUT
      Input::log( "Mouse enabled.\n" );
#endif
   }
   else
   {
      Con::warnf( "Mouse failed to enable!" );
#ifdef LOG_INPUT
      Input::log( "Mouse failed to enable!\n" );
#endif
   }

   return( mMouseEnabled );
}

//------------------------------------------------------------------------------
void UInputManager::disableMouse()
{
   if ( !isEnabled() || !isMouseEnabled())
      return;

   deactivateMouse();
   mMouseEnabled = false;
   Con::printf( "Mouse disabled." );
#ifdef LOG_INPUT
   Input::log( "Mouse disabled.\n" );
#endif
}

//------------------------------------------------------------------------------
bool UInputManager::activateMouse()
{
   if ( !isEnabled() || !isActive() || !isMouseEnabled() )
      return( false );

   mMouseActive = true;
#ifdef LOG_INPUT
   Input::log( mMouseActive ?
      "Mouse activated.\n" : "Mouse failed to activate!\n" );
#endif
   return( mMouseActive );
}

//------------------------------------------------------------------------------
void UInputManager::deactivateMouse()
{
   if ( isEnabled() && isMouseActive() )
   {
      mMouseActive = false;
#ifdef LOG_INPUT
      Input::log( "Mouse deactivated.\n" );
#endif
   }
}

//------------------------------------------------------------------------------
bool UInputManager::enableJoystick()
{
   if ( !isEnabled() )
      return( false );

   if ( isJoystickEnabled() && isJoystickActive() )
      return( true );

   mJoystickEnabled = true;
   if ( isActive() )
      mJoystickEnabled = activateJoystick();

   if ( mJoystickEnabled )
   {
      Con::printf( "Joystick enabled." );
#ifdef LOG_INPUT
      Input::log( "Joystick enabled.\n" );
#endif
   }
   else
   {
      Con::warnf( "Joystick failed to enable!" );
#ifdef LOG_INPUT
      Input::log( "Joystick failed to enable!\n" );
#endif
   }

   return( mJoystickEnabled );
}

//------------------------------------------------------------------------------
void UInputManager::disableJoystick()
{
   if ( !isEnabled() || !isJoystickEnabled())
      return;

   deactivateJoystick();
   mJoystickEnabled = false;
   Con::printf( "Joystick disabled." );
#ifdef LOG_INPUT
   Input::log( "Joystick disabled.\n" );
#endif
}

//------------------------------------------------------------------------------
bool UInputManager::activateJoystick()
{
   if ( !isEnabled() || !isActive() || !isJoystickEnabled() )
      return( false );

   mJoystickActive = false;
   JoystickInputDevice* dptr;
   for ( iterator ptr = begin(); ptr != end(); ptr++ )
   {
      dptr = dynamic_cast<JoystickInputDevice*>( *ptr );
      if ( dptr && dptr->getDeviceType() == JoystickDeviceType)
         if ( dptr->activate() )
            mJoystickActive = true;
   }
#ifdef LOG_INPUT
   Input::log( mJoystickActive ?
      "Joystick activated.\n" : "Joystick failed to activate!\n" );
#endif
   return( mJoystickActive );
}

//------------------------------------------------------------------------------
void UInputManager::deactivateJoystick()
{
   if ( isEnabled() && isJoystickActive() )
   {
      mJoystickActive = false;
      JoystickInputDevice* dptr;
      for ( iterator ptr = begin(); ptr != end(); ptr++ )
      {
         dptr = dynamic_cast<JoystickInputDevice*>( *ptr );
         if ( dptr && dptr->getDeviceType() == JoystickDeviceType)
            dptr->deactivate();
      }
#ifdef LOG_INPUT
      Input::log( "Joystick deactivated.\n" );
#endif
   }
}

//------------------------------------------------------------------------------
const char* UInputManager::getJoystickAxesString( U32 deviceID )
{
   for (Vector<JoystickInputDevice*>::iterator iter = mJoystickList.begin();
        iter != mJoystickList.end();
        ++iter)
   {
      if ((*iter)->getDeviceID() == deviceID)
         return (*iter)->getJoystickAxesString();
   }
   return( "" );
}

//==============================================================================
// JoystickInputDevice
//==============================================================================
JoystickInputDevice::JoystickInputDevice(U8 deviceID)
{
   mActive = false;
   mStick = NULL;
   mAxisList.clear();
   mDeviceID = deviceID;
   dSprintf(mName, 29, "joystick%d", mDeviceID);

   mButtonState.clear();
   mHatState.clear();
   mNumAxes = mNumButtons = mNumHats = mNumBalls = 0;

   loadJoystickInfo();

   // initialize state variables
   for (int i = 0; i < mNumButtons; ++i)
      mButtonState.push_back(false); // all buttons unpressed initially

   for (int i = 0; i < mNumHats; ++i)
      mHatState.push_back(SDL_HAT_CENTERED); // hats centered initially
}

//------------------------------------------------------------------------------
JoystickInputDevice::~JoystickInputDevice()
{
   if (isActive())
      deactivate();
}

//------------------------------------------------------------------------------
bool JoystickInputDevice::activate()
{
   if (isActive())
      return true;

   // open the stick
   mStick = SDL_JoystickOpen(mDeviceID);
   if (mStick == NULL)
   {
      Con::printf("Unable to activate %s: %s", getDeviceName(), SDL_GetError());
      return false;
   }

   // reload axis mapping info
   loadAxisInfo();

   mActive = true;
   return true;
}

//------------------------------------------------------------------------------
bool JoystickInputDevice::deactivate()
{
   if (!isActive())
      return true;

   if (mStick != NULL)
   {
      SDL_JoystickClose(mStick);
      mStick = NULL;
   }

   mActive = false;
   return true;
}

//------------------------------------------------------------------------------
const char* JoystickInputDevice::getName()
{
    SDL_Joystick *inputDevice = SDL_JoystickOpen(mDeviceID);

   return SDL_JoystickName(inputDevice);
}

//------------------------------------------------------------------------------
void JoystickInputDevice::reset()
{
   UInputManager* manager = dynamic_cast<UInputManager*>(Input::getManager());
   if (!manager)
      return;

   // clear joystick state variables

   // buttons
   for (int i = 0; i < mButtonState.size(); ++i)
      if (mButtonState[i])
      {
         manager->joyButtonEvent(mDeviceID, i, false);
         mButtonState[i] = false;
      }

   // hats
   for (int i = 0; i < mHatState.size(); ++i)
      if (mHatState[i] != SDL_HAT_CENTERED)
      {
         manager->joyHatEvent(mDeviceID, i, mHatState[i], SDL_HAT_CENTERED);
         mHatState[i] = SDL_HAT_CENTERED;
      }

   // axis and ball state is not maintained
}

//------------------------------------------------------------------------------
bool JoystickInputDevice::process()
{
   if (!isActive())
      return false;

   UInputManager* manager = dynamic_cast<UInputManager*>(Input::getManager());
   if (!manager)
      return false;

   // axes
   for (int i = 0; i < mNumAxes; ++i)
   {
      // skip the axis if we don't have a mapping for it
      if (mAxisList[i].type == -1)
         continue;
      manager->joyAxisEvent(mDeviceID, i, SDL_JoystickGetAxis(mStick, i));
   }

   // buttons
   for (int i = 0; i < mNumButtons; ++i)
   {
      if (bool(SDL_JoystickGetButton(mStick, i)) ==
         mButtonState[i])
         continue;
      mButtonState[i] = !mButtonState[i];
      manager->joyButtonEvent(mDeviceID, i, mButtonState[i]);
   }

   // hats
   for (int i = 0; i < mNumHats; ++i)
   {
      U8 currHatState = SDL_JoystickGetHat(mStick, i);
      if (mHatState[i] == currHatState)
         continue;

      manager->joyHatEvent(mDeviceID, i, mHatState[i], currHatState);
      mHatState[i] = currHatState;
   }

   // ballz
   // JMQTODO: how to map ball events (xaxis,yaxis?)
   return true;
}

//------------------------------------------------------------------------------
static S32 GetAxisType(S32 axisNum, const char* namedType)
{
   S32 axisType = -1;

   if (namedType != NULL)
   {
      if (dStricmp(namedType, "xaxis")==0)
         axisType = SI_XAXIS;
      else if (dStricmp(namedType, "yaxis")==0)
         axisType = SI_YAXIS;
      else if (dStricmp(namedType, "zaxis")==0)
         axisType = SI_ZAXIS;
      else if (dStricmp(namedType, "rxaxis")==0)
         axisType = SI_RXAXIS;
      else if (dStricmp(namedType, "ryaxis")==0)
         axisType = SI_RYAXIS;
      else if (dStricmp(namedType, "rzaxis")==0)
         axisType = SI_RZAXIS;
      else if (dStricmp(namedType, "slider")==0)
         axisType = SI_SLIDER;
   }

   if (axisType == -1)
   {
      // use a hardcoded default mapping if possible
      switch (axisNum)
      {
         case 0:
            axisType = SI_XAXIS;
            break;
         case 1:
            axisType = SI_YAXIS;
            break;
         case 2:
            axisType = SI_RZAXIS;
            break;
         case 3:
            axisType = SI_SLIDER;
            break;
      }
   }

   return axisType;
}

//------------------------------------------------------------------------------
void JoystickInputDevice::loadJoystickInfo()
{
   bool opened = false;
   if (mStick == NULL)
   {
      mStick = SDL_JoystickOpen(mDeviceID);
      if (mStick == NULL)
      {
         Con::printf("Unable to open %s: %s", getDeviceName(), SDL_GetError());
         return;
      }
      opened = true;
   }

   // get the number of thingies on this joystick
   mNumAxes = SDL_JoystickNumAxes(mStick);
   mNumButtons = SDL_JoystickNumButtons(mStick);
   mNumHats = SDL_JoystickNumHats(mStick);
   mNumBalls = SDL_JoystickNumBalls(mStick);

   // load axis mapping info
   loadAxisInfo();

   if (opened)
      SDL_JoystickClose(mStick);
}

//------------------------------------------------------------------------------
// for each axis on a joystick, torque needs to know the type of the axis
// (SI_XAXIS, etc), the minimum value, and the maximum value.  However none of
// this information is generally available with the unix/linux api.  All you
// get is a device and axis number and a value.  Therefore,
// we allow the user to specify these values in preferences.  hopefully
// someday we can implement a gui joystick calibrator that takes care of this
// cruft for the user.
void JoystickInputDevice::loadAxisInfo()
{
   mAxisList.clear();

   AssertFatal(mStick, "mStick is NULL");

   static int AxisDefaults[] = { SI_XAXIS, SI_YAXIS, SI_ZAXIS,
                                 SI_RXAXIS, SI_RYAXIS, SI_RZAXIS,
                                 SI_SLIDER };

   int numAxis = SDL_JoystickNumAxes(mStick);
   for (int i = 0; i < numAxis; ++i)
   {
      JoystickAxisInfo axisInfo;

      // defaults
      axisInfo.type = -1;
      axisInfo.minValue = -32768;
      axisInfo.maxValue = 32767;

      // look in console to see if there is mapping information for this axis
      const int TempBufSize = 1024;
      char tempBuf[TempBufSize];
      dSprintf(tempBuf, TempBufSize, "$Pref::Input::Joystick%d::Axis%d",
         mDeviceID, i);

      const char* axisStr = Con::getVariable(tempBuf);
      if (axisStr == NULL || dStrlen(axisStr) == 0)
      {
         if (i < sizeof(AxisDefaults))
            axisInfo.type = AxisDefaults[i];
      }
      else
      {
         // format is "TorqueAxisName MinValue MaxValue";
         dStrncpy(tempBuf, axisStr, TempBufSize);
         char* temp = dStrtok( tempBuf, " \0" );
         if (temp)
         {
            axisInfo.type = GetAxisType(i, temp);
            temp = dStrtok( NULL, " \0" );
            if (temp)
            {
               axisInfo.minValue = dAtoi(temp);
               temp = dStrtok( NULL, "\0" );
               if (temp)
               {
                  axisInfo.maxValue = dAtoi(temp);
               }
            }
         }
      }

      mAxisList.push_back(axisInfo);
   }
}

//------------------------------------------------------------------------------
const char* JoystickInputDevice::getJoystickAxesString()
{
   char buf[64];
   dSprintf( buf, sizeof( buf ), "%d", mAxisList.size());

   for (Vector<JoystickAxisInfo>::iterator iter = mAxisList.begin();
        iter != mAxisList.end();
        ++iter)
   {
      switch ((*iter).type)
      {
         case SI_XAXIS:
            dStrcat( buf, "\tX" );
            break;
         case SI_YAXIS:
            dStrcat( buf, "\tY" );
            break;
         case SI_ZAXIS:
            dStrcat( buf, "\tZ" );
            break;
         case SI_RXAXIS:
            dStrcat( buf, "\tR" );
            break;
         case SI_RYAXIS:
            dStrcat( buf, "\tU" );
            break;
         case SI_RZAXIS:
            dStrcat( buf, "\tV" );
            break;
         case SI_SLIDER:
            dStrcat( buf, "\tS" );
            break;
      }
   }

   char* returnString = Con::getReturnBuffer( dStrlen( buf ) + 1 );
   dStrcpy( returnString, buf );
   return( returnString );
}


//==============================================================================
// Console Functions
//==============================================================================
ConsoleFunction( activateKeyboard, bool, 1, 1, "activateKeyboard()" )
{
   UInputManager* mgr = dynamic_cast<UInputManager*>( Input::getManager() );
   if ( mgr )
      return( mgr->activateKeyboard() );

   return( false );
}

// JMQ: disabled deactivateKeyboard since the script calls it but there is
// no fallback keyboard input in unix, resulting in a permanently disabled
// keyboard
//------------------------------------------------------------------------------
ConsoleFunction( deactivateKeyboard, void, 1, 1, "deactivateKeyboard()" )
{
#if 0
   UInputManager* mgr = dynamic_cast<UInputManager*>( Input::getManager() );
   if ( mgr )
      mgr->deactivateKeyboard();
#endif
}

//------------------------------------------------------------------------------
ConsoleFunction( enableMouse, bool, 1, 1, "enableMouse()" )
{
   UInputManager* mgr = dynamic_cast<UInputManager*>( Input::getManager() );
   if ( mgr )
      return( mgr->enableMouse() );

   return ( false );
}

//------------------------------------------------------------------------------
ConsoleFunction( disableMouse, void, 1, 1, "disableMouse()" )
{
   UInputManager* mgr = dynamic_cast<UInputManager*>( Input::getManager() );
   if ( mgr )
      mgr->disableMouse();
}

//------------------------------------------------------------------------------
ConsoleFunction( enableJoystick, bool, 1, 1, "enableJoystick()" )
{
   UInputManager* mgr = dynamic_cast<UInputManager*>( Input::getManager() );
   if ( mgr )
      return( mgr->enableJoystick() );

   return ( false );
}

//------------------------------------------------------------------------------
ConsoleFunction( disableJoystick, void, 1, 1, "disableJoystick()" )
{
   UInputManager* mgr = dynamic_cast<UInputManager*>( Input::getManager() );
   if ( mgr )
      mgr->disableJoystick();
}

//------------------------------------------------------------------------------
ConsoleFunction( enableLocking, void, 1, 1, "enableLocking()" )
{
   UInputManager* mgr = dynamic_cast<UInputManager*>( Input::getManager() );
   if ( mgr )
      mgr->setLocking(true);
}

//------------------------------------------------------------------------------
ConsoleFunction( disableLocking, void, 1, 1, "disableLocking()" )
{
   UInputManager* mgr = dynamic_cast<UInputManager*>( Input::getManager() );
   if ( mgr )
      mgr->setLocking(false);
}

//------------------------------------------------------------------------------
ConsoleFunction( toggleLocking, void, 1, 1, "toggleLocking()" )
{
   UInputManager* mgr = dynamic_cast<UInputManager*>( Input::getManager() );
   if ( mgr )
      mgr->setLocking(!mgr->getLocking());
}

//------------------------------------------------------------------------------
ConsoleFunction( echoInputState, void, 1, 1, "echoInputState()" )
{
   UInputManager* mgr = dynamic_cast<UInputManager*>( Input::getManager() );
   if ( mgr && mgr->isEnabled() )
   {
      Con::printf( "Input is enabled %s.",
         mgr->isActive() ? "and active" : "but inactive" );
      Con::printf( "- Keyboard is %sabled and %sactive.",
         mgr->isKeyboardEnabled() ? "en" : "dis",
         mgr->isKeyboardActive() ? "" : "in" );
      Con::printf( "- Mouse is %sabled and %sactive.",
         mgr->isMouseEnabled() ? "en" : "dis",
         mgr->isMouseActive() ? "" : "in" );
      Con::printf( "- Joystick is %sabled and %sactive.",
         mgr->isJoystickEnabled() ? "en" : "dis",
         mgr->isJoystickActive() ? "" : "in" );
   }
   else
      Con::printf( "Input is not enabled." );
}

