#include "scheduler.h"
#include "log.h"
#include "macro.h"

namespace TinyServer
{
static Ref<Logger> logger = TINY_LOG_NAME("system");
static thread_local Scheduler* t_scheduler = nullptr;
static thread_local Fiber* t_fiber = nullptr;   // ==> run fiber

Scheduler::Scheduler(size_t threads, bool use_call, const std::string& name)
    : m_name(name), m_threadCount(0), m_activateThreadCount(0), m_idleThreadCount(), m_stopping(true), m_autoStop(false), m_rootThread(0)
{
    if (use_call)
    {
        //使用当前构造Scheduler的线程
        Fiber::GetThis();
        --threads;
        TINY_ASSERT(GetThis() == nullptr);        
        t_scheduler = this;

        m_rootFiber.reset(new Fiber(std::bind(&Scheduler::run, this), 0, true));
        Thread::SetName(m_name);
        t_fiber = m_rootFiber.get();
        m_rootThread = GetThreadId();
        m_threadIds.push_back(m_rootThread);
    }
    else
    {
        m_rootThread = -1;
    }
    m_threadCount = threads;
}

Scheduler* Scheduler::GetThis()
{
    return t_scheduler;
}

Fiber* Scheduler::GetMainFiber()
{
    return t_fiber;
}

Scheduler::~Scheduler()
{
    TINY_ASSERT(m_stopping);
    if (GetThis() == this)
    {
        t_scheduler = nullptr;
    }
}

void Scheduler::start()
{
    {
        MutexType::MutexLockGuard lock(m_mutex);
         if (!m_stopping)
        {
            return;
        }
        m_stopping = false;
        TINY_ASSERT(m_threads.empty());

        m_threads.reserve(m_threadCount);
        for (size_t i = 0; i < m_threadCount; ++i)
        {
            m_threads.push_back(Ref<Thread>(new Thread(std::bind(&Scheduler::run, this), m_name + "_" + std::to_string(i))));
            m_threadIds.push_back(m_threads[i]->getId());    
        }   
    }
    if (m_rootFiber)
    {
        m_rootFiber->call();
        //m_rootFiber->swapIn()
        TINY_LOG_INFO(logger) << "call out";
    }
}

void Scheduler::stop()
{
    m_autoStop = true;
    if (m_rootFiber && m_threadCount == 0 && (m_rootFiber->getState() == Fiber::TERM || m_rootFiber->getState() == Fiber::INIT))
    {
        TINY_LOG_INFO(logger) << this << " stopped";
        m_stopping = true;
        if (stopping())
            return;
    }

    //bool exit_on_this_fiber = false;
    if (m_rootThread != -1)
    {
        TINY_ASSERT(GetThis() == this);
    }
    else
    {
        TINY_ASSERT(GetThis() != this);
    }
    m_stopping = true;
    for (size_t i = 0; i < m_threadCount; ++i)
    {
        tickle();
    }
    if (m_rootFiber)
    {
        tickle();
    }
    if (stopping())
        return;
}

bool Scheduler::stopping()
{
    MutexType::MutexLockGuard lock(m_mutex);
    return m_autoStop && m_stopping && m_fibers.empty() && m_activateThreadCount == 0;
}

void Scheduler::idle()
{
    TINY_LOG_INFO(logger) << "idle";
}

void Scheduler::tickle()
{
    TINY_LOG_INFO(logger) << "tickle";
}

void Scheduler::setThis()
{
    t_scheduler = this;
}

void Scheduler::run()
{
    TINY_LOG_INFO(logger) << "run";
    setThis();
    if (GetThreadId() != m_rootThread)
    {
        t_fiber = Fiber::GetThis().get();
    }
    Ref<Fiber> idle_fiber(new Fiber(std::bind(&Scheduler::idle, this)));
    Ref<Fiber> cb_fiber;

    FiberAndThread ft;
    while (!stopping())
    {
        ft.reset();
        bool tickle_me = false;
        {
            MutexType::MutexLockGuard lock(m_mutex);
            auto iter = m_fibers.begin();
            while (iter != m_fibers.end())
            {
                if (iter->threadId != -1 && iter->threadId != GetThreadId())
                {
                    ++iter;
                    tickle_me = true;
                    continue;
                } 
                TINY_ASSERT(iter->fiber || iter->cb);
                if (iter->fiber && iter->fiber->getState() == Fiber::EXEC)
                {
                    ++iter;
                    continue;
                }
                ft = *iter;
                tickle_me = true;
                //list erase之后迭代器也被销毁了，不可再使用
                m_fibers.erase(iter);
                break;
            }
        }

        if (tickle_me)
        {
            tickle();
        }
        if (ft.fiber && (ft.fiber->getState() != Fiber::TERM && ft.fiber->getState() != Fiber::EXCEPT))
        {
            ++m_activateThreadCount;
            ft.fiber->swapIn();
            --m_activateThreadCount;
            if (ft.fiber->getState() == Fiber::READY)
            {
                schedule(ft.fiber);
            }
            else if (ft.fiber->getState() != Fiber::TERM && ft.fiber->getState() != Fiber::EXCEPT)
            {
                ft.fiber->setState(Fiber::HOLD);
            }
            ft.reset();
        }
        else if (ft.cb)
        {
            if (cb_fiber)
            {
                cb_fiber->reset(ft.cb);          
            }
            else
            {
                cb_fiber.reset(new Fiber(ft.cb));
            }
            ft.reset();
            ++m_activateThreadCount;
            cb_fiber->swapIn();
            --m_activateThreadCount;
            if (cb_fiber->getState() == Fiber::READY)
            {
                schedule(cb_fiber);
                cb_fiber.reset();
            }
            else if (cb_fiber->getState() == Fiber::TERM || cb_fiber->getState() == Fiber::EXCEPT)
            {
                cb_fiber->reset(nullptr);
            }
            else
            {
                cb_fiber->setState(Fiber::HOLD);
                cb_fiber.reset();
            }
        }
        else
        {
            if (idle_fiber->getState() == Fiber::TERM)
            {
                TINY_LOG_INFO(logger) << "idle fiber term";
                break;
            }
            ++m_idleThreadCount;
            idle_fiber->swapIn();
            --m_idleThreadCount;
            if (idle_fiber->getState() != Fiber::TERM && idle_fiber->getState() != Fiber::EXCEPT)
            {
                idle_fiber->setState(Fiber::HOLD);
            }
        }
    }


}

}