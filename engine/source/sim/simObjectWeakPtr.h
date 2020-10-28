#ifndef _SIM_OBJECT_WEAK_PTR_H_
#define _SIM_OBJECT_WEAK_PTR_H_

#include "collection/refBase.h"
#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif

template< typename T >
class SimObjectWeakPtr : public WeakRefPtr< T >
{
public:

   typedef WeakRefPtr< T > Parent;

   SimObjectWeakPtr() {}
   SimObjectWeakPtr(T *ptr) { this->mReference = NULL; set(ptr); }
   SimObjectWeakPtr(const SimObjectWeakPtr& ref) { this->mReference = NULL; set(ref.mReference); }

   T* getObject() const { return Parent::getPointer(); }

   ~SimObjectWeakPtr() { set((WeakRefBase::WeakReference*)NULL); }

   SimObjectWeakPtr<T>& operator=(const SimObjectWeakPtr ref)
   {
      set(ref.mReference);
      return *this;
   }
   SimObjectWeakPtr<T>& operator=(T *ptr)
   {
      set(ptr);
      return *this;
   }

protected:
   void set(WeakRefBase::WeakReference * ref)
   {
      if (ref == this->mReference)
         return;

      if (this->mReference)
      {
         // Auto-delete
         T* obj = this->getPointer();
         if (this->mReference->getRefCount() == 2 && obj && obj->isAutoDeleted())
            obj->deleteObject();

         this->mReference->decRefCount();
      }
      this->mReference = NULL;
      if (ref)
      {
         this->mReference = ref;
         this->mReference->incRefCount();
      }
   }

   void set(T * obj)
   {
      set(obj ? obj->getWeakReference() : (WeakRefBase::WeakReference *)NULL);
   }
};

#endif