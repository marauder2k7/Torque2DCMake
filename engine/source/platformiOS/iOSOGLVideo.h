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

#ifndef _iOSOGLVIDEO_H_
#define _iOSOGLVIDEO_H_

#import "UIKit/UIKit.h"
#import "UIKit/UIAccelerometer.h"

#include "platform/platformVideo.h"

class OpenGLDevice : public DisplayDevice
{
private:
    
    /// Gamma value
    F32 mGamma;
    
    /// Cleans up the opengl context, and destroys the rendering window
    bool cleanupContextAndWindow();
    
public:
    OpenGLDevice();
    static DisplayDevice* create();
    
    /// The following are inherited from DisplayDevice
    void initDevice();
    bool activate( U32 width, U32 height, U32 bpp, bool fullScreen );
    
    void shutdown();
    
    bool setScreenMode( U32 width, U32 height, U32 bpp, bool fullScreen, bool forceIt = false, bool repaint = true );
    void swapBuffers();
    
    const char* getDriverInfo();
    bool getGammaCorrection(F32 &g);
    bool setGammaCorrection(F32 g);
    bool getVerticalSync( );
    bool setVerticalSync( bool on );
};

#endif // _iOSOGLVIDEO_H_
