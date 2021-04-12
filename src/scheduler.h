#pragma once
#include <memory>
#include "fiber.h"

/////////////////////////////////////////////////////////////////////
//                    main --> run --> 2.idle --> run              //
//                              |                                  //
//                              |                                  //
//                              1.test_fiber                       //
/////////////////////////////////////////////////////////////////////

namespace TinyServer
{
class Scheduler
{
public:
    typedef MutexLock MutexType;
    Scheduler(size_t threads = 1, bool use_call = true, const std::string& name = "");
    virtual ~Scheduler();

    std::string getName() const { return m_name; }

    static Scheduler* GetThis();
    static Fiber* GetMainFiber();

    void start();
    void stop();

    void setThis();
    virtual bool stopping();
    virtual void idle();

    template<typename FiberOrCb>
    void schedule(FiberOrCb fc, int threadId = -1)
    {
        bool need_tickle = false;
        {
            MutexType::MutexLockGuard lock(m_mutex);
            scheduleNoLock(fc, threadId);
        }
        if (need_tickle)
        {
            tickle();
        }
    }

    template<typename InputIterator>
    void schedule(InputIterator begin, InputIterator end)
    {
        bool need_tickle = false;
        {
            MutexType::MutexLockGuard lock(m_mutex);
            while (begin != end)
            {
                need_tickle = scheduleNoLock(&(*begin)) || need_tickle;
                ++begin;
            }
        }
        if (need_tickle)
        {
            tickle();
        }
    }

    void run();

    bool hasIdleThreads() { return m_idleThreadCount > 0; }

protected:
    virtual void tickle();

private:
    template<typename FiberOrCb>
    bool scheduleNoLock(FiberOrCb fc, int threadId = -1)  
    {
        bool need_tickle = m_fibers.empty();
        FiberAndThread ft(fc, threadId);
        if (ft.fiber || ft.cb)
        {
            m_fibers.push_back(ft);
        }
        return need_tickle;
    }

private:
    struct FiberAndThread
    {
        Ref<Fiber> fiber;
        std::function<void()> cb;
        int threadId;

        FiberAndThread(Ref<Fiber> f, int thr)
            : fiber(f), threadId(thr) {}

        FiberAndThread(Ref<Fiber> *f, int thr)
            : threadId(thr)
        {
            fiber.swap(*f);
        }

        FiberAndThread(std::function<void()> f, int thr)
            : cb(f), threadId(thr) {}

        FiberAndThread(std::function<void()>* f, int thr)
            : threadId(thr)
        {
            cb.swap(*f);
        }    
        
        FiberAndThread()
            : threadId(-1) {}

        void reset()
        {
            fiber = nullptr;
            cb = nullptr;
            threadId = -1;
        }
    };

private:
    std::string m_name;
    MutexType m_mutex;
    std::vector<Ref<Thread>> m_threads;
    std::list<FiberAndThread> m_fibers;
    Ref<Fiber> m_rootFiber;

protected:
    std::vector<int> m_threadIds;
    size_t m_threadCount;
    std::atomic<size_t> m_activateThreadCount;
    std::atomic<size_t> m_idleThreadCount;
    bool m_stopping;
    bool m_autoStop;
    int m_rootThread;
};



}
