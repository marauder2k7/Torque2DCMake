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

#ifndef _GUISPRITECTRL_H_
#define _GUISPRITECTRL_H_

#ifndef _IMAGE_FRAME_PROVIDER_H
#include "2d/core/ImageFrameProvider.h"
#endif

//-----------------------------------------------------------------------------

class GuiSpriteCtrl : public GuiControl, public ImageFrameProvider
{
private:
    typedef GuiControl Parent;

protected:
    StringTableEntry                mImageAssetId;
    U32                             mImageFrameId;
    StringTableEntry                mNamedImageFrameId;
    StringTableEntry                mAnimationAssetId;

public:
    GuiSpriteCtrl();
    virtual ~GuiSpriteCtrl();
    bool onWake();
    void onSleep();
    void onRender(Point2I offset, const RectI &updateRect);
    static void initPersistFields();

    virtual void copyTo(SimObject* object);

    // Static and Animated Assets.
    inline bool setImage( const char* pImageAssetId ) { return setImage( pImageAssetId, mImageFrame ); }
    virtual bool setImage( const char* pImageAssetId, const U32 frame );
    virtual bool setImage( const char* pImageAssetId, const char* pNamedFrame );
    inline StringTableEntry getImage( void ) const{ return mImageAssetId; }
    virtual bool setImageFrame( const U32 frame );
    inline U32 getImageFrame( void ) const { return mImageFrameId; }
    virtual bool setNamedImageFrame ( const char* namedFrame );
    inline StringTableEntry getNamedImageFrame( void ) const { return mNamedImageFrameId; }
    virtual bool setAnimation( const char* pAnimationAssetId );
    inline StringTableEntry getAnimation( void ) const { return mAnimationAssetId; }

    // Declare type.
    DECLARE_CONOBJECT(GuiSpriteCtrl);

protected:
    virtual void onAnimationEnd( void );
	virtual void processTick();
	virtual void interpolateTick(F32 delta) {};
	virtual void advanceTime(F32 timeDelta) {};

protected:
    static bool setImage(void* obj, const char* data) { static_cast<GuiSpriteCtrl*>(obj)->setImage( data ); return false; }
    static bool writeImage(void* obj, StringTableEntry pFieldName) { GuiSpriteCtrl* pCastObject = static_cast<GuiSpriteCtrl*>(obj); if ( !pCastObject->isStaticFrameProvider() ) return false; return pCastObject->mImageAssetId != StringTable->EmptyString; }
    static bool setImageFrame(void* obj, const char* data) { static_cast<GuiSpriteCtrl*>(obj)->setImageFrame( dAtoi(data) ); return false; }
    static bool writeImageFrame(void* obj, StringTableEntry pFieldName) { GuiSpriteCtrl* pCastObject = static_cast<GuiSpriteCtrl*>(obj); if ( !pCastObject->isStaticFrameProvider() || pCastObject->isUsingNamedImageFrame() ) return false; return pCastObject->getImageFrame() > 0; }
    static bool setNamedImageFrame(void* obj, const char* data) { static_cast<GuiSpriteCtrl*>(obj)->setNamedImageFrame(data); return false; }
    static bool writeNamedImageFrame(void* obj, StringTableEntry pFieldName) { GuiSpriteCtrl* pCastObject = static_cast<GuiSpriteCtrl*>(obj); if ( !pCastObject->isStaticFrameProvider() || !pCastObject->isUsingNamedImageFrame() ) return false; return pCastObject->mNamedImageFrameId != StringTable->EmptyString; }
    static bool setAnimation(void* obj, const char* data) { static_cast<GuiSpriteCtrl*>(obj)->setAnimation(data); return false; };
    static bool writeAnimation(void* obj, StringTableEntry pFieldName) { GuiSpriteCtrl* pCastObject = static_cast<GuiSpriteCtrl*>(obj); if ( pCastObject->isStaticFrameProvider() ) return false; return pCastObject->mAnimationAssetId != StringTable->EmptyString; }
};

#endif //_GUISPRITECTRL_H_
