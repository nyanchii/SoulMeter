#pragma once

template <class T>
class Singleton {
protected:
    Singleton() {}
    virtual ~Singleton() {}

public:
    Singleton(const Singleton&);
    Singleton& operator = (const Singleton&);

    static T& getInstance()
    {
        static T instance;
        return instance;
    }
};