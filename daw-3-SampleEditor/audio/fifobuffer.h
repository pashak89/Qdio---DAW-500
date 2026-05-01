#ifndef FIFOBUFFER_H
#define FIFOBUFFER_H

#include <QSemaphore>

template <typename T>
class FifoBuffer {
public:
    FifoBuffer(int size)
        : m_readSem(size)
        , m_writeSem(size)
        , m_readIndex(0)
        , m_writeIndex(0)
        , m_size(size)
    {
        m_buffer = new T[size];
        m_readSem.acquire(size);
    }

    ~FifoBuffer()
    {
        delete[] m_buffer;
        m_readSem.release(m_size);
    }

    void write(T element)
    {
        m_writeSem.acquire();
        m_buffer[m_writeIndex++] = element;
        m_writeIndex %= m_size;
        m_readSem.release();
    }

    T read()
    {
        m_readSem.acquire();
        T element = m_buffer[m_readIndex++];
        m_readIndex %= m_size;
        m_writeSem.release();
        return element;
    }

    void waitUntilRead()
    {
        m_writeSem.acquire(m_size);
        m_writeSem.release(m_size);
    }

    bool available()
    {
        return m_readSem.available();
    }

private:
    QSemaphore m_readSem;
    QSemaphore m_writeSem;
    int m_readIndex;
    int m_writeIndex;
    int m_size;
    T* m_buffer;
};

#endif // FIFOBUFFER_H
