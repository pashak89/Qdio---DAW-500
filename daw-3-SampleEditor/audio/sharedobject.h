#ifndef SHAREDOBJECT_H
#define SHAREDOBJECT_H

#include <atomic>

class sharedObject {
public:
    sharedObject()
        : m_referenceCount(1)
    {
    }

    virtual ~sharedObject() = default;

    template <class T>
    static T* ref(T* object)
    {
        // Incrementing an atomic reference count can be relaxed since no action
        // is ever taken as a result of increasing the count.
        // Other loads and stores can be reordered around this without consequence.
        object->m_referenceCount.fetch_add(1, std::memory_order_relaxed);
        return object;
    }

    template <class T>
    static void unref(T* object)
    {
        // When decrementing an atomic reference count, we need to provide
        // two ordering guarantees:
        // 1. All reads and writes to the referenced object occur before
        //    the count reaches zero.
        // 2. Deletion occurs after the count reaches zero.
        //
        // To accomplish this, each decrement must be store-released,
        // and the final thread (which is deleting the referenced data)
        // must load-acquire those stores.
        // The simplest way to do this to give the decrement acquire-release
        // semantics.
        //
        // See https://www.boost.org/doc/libs/1_67_0/doc/html/atomic/usage_examples.html
        // for further discussion, along with a slightly more complicated
        // (but possibly more performant on weakly-ordered hardware like ARM)
        // approach.
        const bool deleteObject = object->m_referenceCount.fetch_sub(1, std::memory_order_acq_rel) == 1;

        if (deleteObject) {
            object->deleteLater();
        }
    }

private:
    std::atomic_int m_referenceCount;
};
#endif // SHAREDOBJECT_H
