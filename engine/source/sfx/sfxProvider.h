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
   friend class SFXSystem;

private:
   /// The head of the linked list of avalible providers.
   static SFXProvider* smProviders;

   /// The next provider in the linked list of available providers. 
   SFXProvider* mNextProvider;

   char* mName;

   static Vector<SFXProvider*> sAllProviders;
   
public:

   SFXProvider(const char *name)
   {
      mName = (char*)name;
      dMemset(&mOpenAL, 0, sizeof(mOpenAL)); mALDL = NULL;
   }
   virtual ~SFXProvider();

protected:
   OPENALFNTABLE mOpenAL;
   ALDeviceList *mALDL;

   ALDeviceInfoVector  mDeviceInfo;

   void regProvider(SFXProvider * provider);

   void init();

public:
   
   SFXDevice *createDevice(const StringTableEntry& deviceName, bool useHardware, S32 maxBuffers);

   static SFXProvider * findProvider(char providerName);

   ALDeviceInfo * _findDeviceInfo(const StringTableEntry& deviceName);

   const ALDeviceInfoVector& getDeviceInfo() const { return mDeviceInfo; }
   
   static void initializeAllProviders();
   
   static SFXProvider* getFirstProvider() { return smProviders; }
   
   SFXProvider* getNextProvider() const { return mNextProvider; }
   
   char& getName() const { return (char&)mName; }
   
};