#pragma once

namespace TinyServer
{
template<typename T, class X = void, int N = 0>
class Singleton
{
public:
    static T* GetInstance()
    {
        static T t;
        return &t;
    }
};

template<typename T, class X = void, int N = 0>
class SingletonPtr
{
public:
    static std::shared_ptr<T> GetIntance()
    {
        static std::shared_ptr<T> t(new T);
        return t;
    }
};



} // namespace TinyServer
