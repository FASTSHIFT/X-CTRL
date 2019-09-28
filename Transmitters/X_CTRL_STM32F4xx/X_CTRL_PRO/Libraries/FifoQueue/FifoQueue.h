#ifndef __FIFOQUEUE_H
#define __FIFOQUEUE_H

#include <stdint.h>

template <class T> class FifoQueue {
public:
    FifoQueue(uint32_t bufferSize);
    FifoQueue(uint32_t bufferSize, char* buffer);
    ~FifoQueue();
    uint32_t available();
    bool write(T data);
    T read();
    void flush();
private:
    T *Buffer;
    uint32_t Head;
    uint32_t Tail;
    uint32_t BufferSize;
};

template <class T> FifoQueue<T>::FifoQueue(uint32_t bufferSize, char* buffer)
{
    BufferSize = bufferSize;
    Buffer = (T *)buffer;
}

template <class T> FifoQueue<T>::FifoQueue(uint32_t bufferSize)
{
    BufferSize = bufferSize;
    Buffer = new T[BufferSize];
}

template <class T> FifoQueue<T>::~FifoQueue()
{
    delete[] Buffer;
}

template <class T> uint32_t FifoQueue<T>::available()
{
    return ((uint32_t)(BufferSize + Head - Tail)) % BufferSize;
}

template <class T> T FifoQueue<T>::read()
{
    if (Head == Tail)
        return T(0);
    else
    {
        T c = Buffer[Tail];
        Tail = (Tail + 1) % BufferSize;
        return c;
    }
}

template <class T> bool FifoQueue<T>::write(T data)
{
    uint32_t i = (uint32_t)(Head + 1) % BufferSize;
    if (i != Tail)
    {
        Buffer[Head] = data;
        Head = i;
        return true;
    }
    else
        return false;
}

template <class T> void FifoQueue<T>::flush()
{
    Head = Tail;
}

#endif
