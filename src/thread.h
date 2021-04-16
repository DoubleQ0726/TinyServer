#pragma once
#include <memory>
#include <functional>
#include <string>
#include <pthread.h>
#include <semaphore.h>
#include <atomic>
#include "noncoptable.h"

namespace TinyServer
{
class Semaphore : public Noncopyable
{
public:
    Semaphore(const uint32_t count = 0);

    void wait();
    void notify();

//private:
    // Semaphore(const Semaphore&) = delete;
    // Semaphore(const Semaphore&&) = delete;
    // Semaphore operator=(const Semaphore&) = delete;
    // Semaphore operator=(const Semaphore&&) = delete;

private:
    sem_t m_semaphore;
};

template<typename T>
class ScopeLockImp
{
public:
    //此处为LockGuard模板，构造函数应为引用格式，普通的值传递会引起Lock失效
    ScopeLockImp(T& mutex)
        : m_mutex(mutex), m_locked(false)
    {
        m_mutex.lock();
        m_locked = true;
    }

    ~ScopeLockImp()
    {
        unlock();
    }

    void lock()
    {
        if (!m_locked)
        {
            m_mutex.lock();
            m_locked = true;
        }
    }

    void unlock()
    {
        if (m_locked)
        {
            m_mutex.unlock();
            m_locked = false;
        }
    }

private:
    T& m_mutex;
    bool m_locked;
};

template<typename T>
class ReadScopeLockImp
{
public:
    ReadScopeLockImp(T& mutex)
        : m_mutex(mutex), m_locked(false)
    {
        m_mutex.rdlock();
        m_locked = true;
    }
    ~ReadScopeLockImp()
    {
        unlock();
    }

    void lock()
    {
        if (!m_locked)
        {
            m_mutex.rdlock();
            m_locked = true;
        }
    }

    void unlock()
    {
        //if (m_locked)
        //{
            m_mutex.unlock();
            m_locked = false;
        //}
    }

private:
    T& m_mutex;
    bool m_locked;
};


template<typename T>
class WriteScopeLockImp
{
public:
    WriteScopeLockImp(T& mutex)
        : m_mutex(mutex), m_locked(false)
    {
        m_mutex.wrlock();
        m_locked = true;
    }
    ~WriteScopeLockImp()
    {
        unlock();
    }

    void lock()
    {
        if (!m_locked)
        {
            m_mutex.wrlock();
            m_locked = true;
        }
    }

    void unlock()
    {
        if (m_locked)
        {
            m_mutex.unlock();
            m_locked = false;
        }
    }

private:
    T& m_mutex;
    bool m_locked;
};

class MutexLock : public Noncopyable
{
public:
    using MutexLockGuard = ScopeLockImp<MutexLock>;
    MutexLock()
    {
        pthread_mutex_init(&m_mutex, nullptr);
    }

    ~MutexLock()
    {
        pthread_mutex_destroy(&m_mutex);
    }

    void lock()
    {
        pthread_mutex_lock(&m_mutex);
    }

    void unlock()
    {
        pthread_mutex_unlock(&m_mutex);
    }


private:
    pthread_mutex_t m_mutex;
};

class RWLock : public Noncopyable
{
public:
    using ReadLockGuard = ReadScopeLockImp<RWLock>;
    using WriteLockGuard = WriteScopeLockImp<RWLock>; 
    RWLock()
    {
        pthread_rwlock_init(&m_rwlock, nullptr);
    }

    ~RWLock()
    {
        pthread_rwlock_destroy(&m_rwlock);
    }

    void rdlock()
    {
        pthread_rwlock_rdlock(&m_rwlock);
    }

    void wrlock()
    {
        pthread_rwlock_wrlock(&m_rwlock);
    }

    void unlock()
    {
        pthread_rwlock_unlock(&m_rwlock);
    }

private:
    pthread_rwlock_t m_rwlock;
};

class Thread : public Noncopyable
{
public:
    Thread(std::function<void()> cb, const std::string& name);
    ~Thread();
    
    pid_t getId() const { return m_id; }
    std::string getName() const { return m_name; }
    void join();


    static Thread* GetThis();
    static std::string GetName();
    static void SetName(std::string name);

// private:
//     Thread(const Thread&) = delete;
//     Thread(const Thread&&) = delete;
//     Thread operator=(const Thread&) = delete;
//     Thread operator=(const Thread&&) = delete;

    static void* run(void* arg);
private:
    pid_t m_id;
    pthread_t m_thread;
    std::function<void()> m_callBack;
    std::string m_name;
    Semaphore m_semaphore;
};

class SpinLock : public Noncopyable
{
public:
    using MutexLockGuard = ScopeLockImp<SpinLock>;
    SpinLock()
    {
        pthread_spin_init(&m_spinlock, 0);
    }

    ~SpinLock()
    {
        pthread_spin_destroy(&m_spinlock);
    }

    void lock()
    {
        pthread_spin_lock(&m_spinlock);
    }

    void unlock()
    {
        pthread_spin_unlock(&m_spinlock);
    }

private:
    pthread_spinlock_t m_spinlock;
};

class CASLock : public Noncopyable
{
public:
    using MutexLockGuard = ScopeLockImp<CASLock>;
    CASLock()
    {
        m_mutex.clear();
    }

    ~CASLock()
    {
        
    }

    void lock()
    {
        //自旋获得改锁
        while (std::atomic_flag_test_and_set_explicit(&m_mutex, std::memory_order_acquire));
    }

    void unlock()
    {
        std::atomic_flag_clear_explicit(&m_mutex, std::memory_order_release);
    }

private:
    volatile std::atomic_flag m_mutex;
};

//Debug Multi Thread
class NullMutexLock : public Noncopyable
{
public:
    using MutexLockGuard = ScopeLockImp<NullMutexLock>;
    NullMutexLock() {}
    ~NullMutexLock() {}
    void lock() {}
    void unlock() {}
};
//Debug Multi Thread
class NullRWLock : public Noncopyable
{
public:
    using ReadLockGuard = ReadScopeLockImp<NullRWLock>;
    using WriteLockGuard = WriteScopeLockImp<NullRWLock>; 
    NullRWLock() {}
    ~NullRWLock() {}
    void rdlock() {}
    void wrlock() {}
    void unlock(){}
};

}
