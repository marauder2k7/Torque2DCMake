#ifndef _SFXPROVIDER_H_
#define _SFXPROVIDER_H_

#include "platform/platform.h"

#include "sfx/sfxDevice.h"
#include "sfx/LoadOal.h"
#include "console/console.h"

class SFXProvider
{
public:

   SFXProvider() {
      dMemset(&mOpenAL, 0, sizeof(mOpenAL));
   }
   ~SFXProvider();

protected:
   OPENALFNTABLE mOpenAL;

   

public:
   void init();
   bool mTrue;
   SFXDevice *createDevice();

};

#endif // !_SFXPROVIDER_H_
