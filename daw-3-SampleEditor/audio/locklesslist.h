#ifndef LOCKLESSLIST_H
#define LOCKLESSLIST_H

#include "LocklessAllocator.h"

#include <atomic>

template <typename T>
class LocklessList {
public:
    struct Element {
        T value;
        Element* next;
    };

    LocklessList(size_t size)
        : m_first(nullptr)
        , m_allocator(new LocklessAllocatorT<Element>(size))
    {
    }

    ~LocklessList()
    {
        delete m_allocator;
    }

    void push(T value)
    {
        Element* e = m_allocator->alloc();
        e->value = value;
        e->next = m_first.load(std::memory_order_relaxed);

        while (!m_first.compare_exchange_weak(e->next, e,
            std::memory_order_release,
            std::memory_order_relaxed)) {
            // Empty loop (compare_exchange_weak updates e->next)
        }
    }

    Element* popList()
    {
        return m_first.exchange(nullptr);
    }

    Element* first()
    {
        return m_first.load(std::memory_order_acquire);
    }

    void setFirst(Element* e)
    {
        m_first.store(e, std::memory_order_release);
    }

    void free(Element* e)
    {
        m_allocator->free(e);
    }

private:
    std::atomic<Element*> m_first;
    LocklessAllocatorT<Element>* m_allocator;
};

#endif // LOCKLESSLIST_H
