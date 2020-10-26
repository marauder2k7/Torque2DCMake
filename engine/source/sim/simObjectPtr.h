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

#ifndef _SIM_OBJECT_PTR_H_
#define _SIM_OBJECT_PTR_H_

#include "collection/refBase.h"

#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif


//---------------------------------------------------------------------------
/// Smart SimObject pointer.
///
/// This class keeps track of the book-keeping necessary
/// to keep a registered reference to a SimObject or subclass
/// thereof.
///
/// Normally, if you want the SimObject to be aware that you
/// have a reference to it, you must call SimObject::registerReference()
/// when you create the reference, and SimObject::unregisterReference() when
/// you're done. If you change the reference, you must also register/unregister
/// it. This is a big headache, so this class exists to automatically
/// keep track of things for you.
///
/// @code
///     // Assign an object to the
///     SimObjectPtr<GameBase> mOrbitObject = Sim::findObject("anObject");
///
///     // Use it as a GameBase*.
///     mOrbitObject->getWorldBox().getCenter(&mPosition);
///
///     // And reassign it - it will automatically update the references.
///     mOrbitObject = Sim::findObject("anotherObject");
/// @endcode
template <typename T> 
class SimObjectPtr : public WeakRefPtr< T >
{
public:

   typedef WeakRefPtr< T > Parent;

   SimObjectPtr() {}
   SimObjectPtr(T *ptr) { this->mReference = NULL; set(ptr); }
   SimObjectPtr(const SimObjectPtr& ref) { this->mReference = NULL; set(ref.mReference); }

   T* getObject() const { return Parent::getPointer(); }

   ~SimObjectPtr() { set((WeakRefBase::WeakReference*)NULL); }

   SimObjectPtr<T>& operator=(const SimObjectPtr ref)
   {
      set(ref.mReference);
      return *this;
   }
   SimObjectPtr<T>& operator=(T *ptr)
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

#endif // _SIM_OBJECT_PTR_H_