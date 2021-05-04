#pragma once

template <class T>
class MultiThreadSync
{
public:
    class ThreadSync
    {
    public:
        ThreadSync()
        {
            T::mSync.Enter();
        }

        ~ThreadSync()
        {
            T::mSync.Leave();
        }
    };

    friend class ThreadSync;

private:
    static CCriticalSection mSync;
};

template <class T>
CCriticalSection MultiThreadSync<T>::mSync;