#ifndef LOCKLESSALLOCATOR_H
#define LOCKLESSALLOCATOR_H

#include <atomic>
#include <cstddef>

class LocklessAllocator {
public:
    LocklessAllocator(size_t nmemb, size_t size);
    virtual ~LocklessAllocator();
    void* alloc();
    void free(void* ptr);

private:
    char* m_pool;
    size_t m_capacity;
    size_t m_elementSize;

    std::atomic_int* m_freeState;
    size_t m_freeStateSets;

    std::atomic_int m_available;
    std::atomic_int m_startIndex;
};

template <typename T>
class LocklessAllocatorT : private LocklessAllocator {
public:
    LocklessAllocatorT(size_t nmemb)
        : LocklessAllocator(nmemb, sizeof(T))
    {
    }

    ~LocklessAllocatorT() override = default;

    T* alloc()
    {
        return (T*)LocklessAllocator::alloc();
    }

    void free(T* ptr)
    {
        LocklessAllocator::free(ptr);
    }
};

#endif // LOCKLESSALLOCATOR_H
