#pragma once
#include <memory>
#include <set>
#include <vector>
#include "thread.h"
#include "log.h"
namespace TinyServer
{

class TimerManager;
class Timer : public std::enable_shared_from_this<Timer>
{
friend class TimerManager;
public:
    bool cancle();
    bool refresh();
    bool reset(uint64_t ms, bool from_now);

private:
    Timer(uint64_t ms, std::function<void()> cb, bool recurring, TimerManager* manager);

    Timer(uint64_t now_time);

private:
    struct Compare
    {
        bool operator()(const Ref<Timer>& lhs, const Ref<Timer>& rhs);
    };

private:
    uint64_t m_ms;      //执行周期
    uint64_t m_next;    //精确的执行时间
    std::function<void()> m_cb; //定时器任务
    bool m_recurring;   //是否循环执行定时器
    TimerManager* m_manager;
};

class TimerManager
{
friend class Timer;
public:
    typedef RWLock RWMutexType;

    TimerManager();
    virtual ~TimerManager();

    Ref<Timer> addTimer(uint64_t ms, std::function<void()> cb, bool recurring = false);

    void addTimer(const Ref<Timer>& timer, RWMutexType::WriteLockGuard& lock);

    Ref<Timer> addConditionTimer(uint64_t ms, std::function<void()> cb, std::weak_ptr<void> weak_cond, bool recurring = false);

    uint64_t getNextTimer();

    void listExpireCB(std::vector<std::function<void()>>& cbs);

    bool hasTimer();

protected:
    virtual void onTimerInsertAtFront() = 0;

private:
    bool detectClockRollover(uint64_t now_time);

private:
    RWMutexType m_mutex;
    std::set<Ref<Timer>, Timer::Compare> m_timers;
    bool m_tickled = false;
    uint64_t m_previousTime = 0;
};



}