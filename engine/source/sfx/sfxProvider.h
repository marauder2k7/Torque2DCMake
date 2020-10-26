#include "platform/platform.h"

#include "sfx/sfxDevice.h"
#include "sfx/aldlist.h"
#include "sfx/LoadOAL.h"

#include "string/stringTable.h"
#include "console/console.h"

struct ALDeviceInfo
{
   char  driver;
   StringTableEntry  name;
   bool  hasHardware;
   S32   maxBuffers;
};

typedef Vector<ALDeviceInfo*> ALDeviceInfoVector;

class SFXProvider
{
public:

   SFXProvider()
   {
      dMemset(&mOpenAL, 0, sizeof(mOpenAL)); mALDL = NULL;
   }
   virtual ~SFXProvider();

protected:
   OPENALFNTABLE mOpenAL;
   ALDeviceList *mALDL;

   ALDeviceInfoVector  mDeviceInfo;

   void init();

public:
   SFXDevice *createDevice(const StringTableEntry& deviceName, bool useHardware, S32 maxBuffers);

   ALDeviceInfo * _findDeviceInfo(const StringTableEntry& deviceName);

};