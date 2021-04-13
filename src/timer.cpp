#include "timer.h"
#include "util.h"


namespace TinyServer
{

bool Timer::Compare::operator()(const Ref<Timer>& lhs, const Ref<Timer>& rhs)
{
    if (!lhs && !rhs)
        return false;
    if (!lhs)
        return true;
    if (!rhs)
        return false;
    if (lhs->m_next < rhs->m_next)
        return true;
    if (rhs->m_next < lhs->m_next)
        return false;
    return lhs.get() < rhs.get();
}


bool Timer::cancle()
{
    TimerManager::RWMutexType::WriteLockGuard lock(m_manager->m_mutex);
    if (m_cb)
    {
        m_cb = nullptr;
        auto iter = m_manager->m_timers.find(shared_from_this());
        m_manager->m_timers.erase(iter);
        return true;
    }
    return false;
}

bool Timer::refresh()
{
    TimerManager::RWMutexType::WriteLockGuard lock(m_manager->m_mutex);
    if (!m_cb)
        return false;
    auto iter = m_manager->m_timers.find(shared_from_this());
    if (iter == m_manager->m_timers.end())
        return false;
    //此处需要先移除再重设时间
    m_manager->m_timers.erase(iter);
    m_next = GetCurrentMs() + m_ms;
    m_manager->m_timers.insert(shared_from_this());
    return true;
}

//重设间隔时间
bool Timer::reset(uint64_t ms, bool from_now)
{
    if (ms == m_ms && !from_now)
        return false;
    TimerManager::RWMutexType::WriteLockGuard lock(m_manager->m_mutex);
    if (!m_cb)
        return false;
    auto iter = m_manager->m_timers.find(shared_from_this());
    if (iter == m_manager->m_timers.end())
        return false;
    //此处需要先移除再重设时间
    m_manager->m_timers.erase(iter);
    uint64_t start = 0;
    if (from_now)
        start = GetCurrentMs();
    else
    {
        start = m_next - m_ms;
    }
    m_ms = ms;
    m_next = start + m_ms;
    m_manager->addTimer(shared_from_this(), lock);
    return true;
}


Timer::Timer(uint64_t ms, std::function<void()> cb, bool recurring, TimerManager* manager)
    : m_ms(ms), m_cb(cb), m_recurring(recurring), m_manager(manager)
{
    m_next = GetCurrentMs() + m_ms;
}

Timer::Timer(uint64_t now_time)
    : m_ms(0), m_cb(nullptr), m_recurring(false), m_manager(nullptr)
{
    m_next = now_time;
}


TimerManager::TimerManager()
    : m_previousTime(GetCurrentMs())
{

}


TimerManager::~TimerManager()
{

}

Ref<Timer> TimerManager::addTimer(uint64_t ms, std::function<void()> cb, bool recurring)
{
    Ref<Timer> timer(new Timer(ms, cb, recurring, this));
    RWMutexType::WriteLockGuard lock(m_mutex);
    addTimer(timer, lock);
    return timer;
}

void TimerManager::addTimer(const Ref<Timer>& timer, TimerManager::RWMutexType::WriteLockGuard& lock)
{
    auto iter = m_timers.insert(timer).first;
    bool at_front = (iter == m_timers.begin()) && !m_tickled;
    if (at_front)
        m_tickled = true;
    lock.unlock();

    if (at_front)
    {
        onTimerInsertAtFront();
    }
}

static void OnTimer(std::weak_ptr<void> weak_cond, std::function<void()> cb)
{
    std::shared_ptr<void> temp = weak_cond.lock();
    if (temp)
        cb();
}


Ref<Timer> TimerManager::addConditionTimer(uint64_t ms, std::function<void()> cb, std::weak_ptr<void> weak_cond, bool recurring)
{
    return addTimer(ms, std::bind(&OnTimer, weak_cond, cb), recurring);
}

uint64_t TimerManager::getNextTimer()
{
    RWMutexType::ReadLockGuard lock(m_mutex);
    m_tickled = true;
    if (m_timers.empty())
        return ~0ull;
    uint64_t now_time = GetCurrentMs();
    const Ref<Timer>& timer = *(m_timers.begin());
    if (now_time > timer->m_next)
        return 0;
    else
    {
        return timer->m_next - now_time;
    }
}

void TimerManager::listExpireCB(std::vector<std::function<void()>>& cbs)
{
    uint64_t now_time = GetCurrentMs();
    std::vector<Ref<Timer>> expire;
    {
        RWMutexType::ReadLockGuard lock(m_mutex);
        if (m_timers.empty())
            return;
    }
    RWMutexType::WriteLockGuard lock(m_mutex);

    bool rollover = detectClockRollover(now_time);
    if (!rollover && (*(m_timers.begin()))->m_next > now_time)
        return;

    Ref<Timer> now_timer(new Timer(now_time));
    auto iter = rollover ? m_timers.end() : m_timers.lower_bound(now_timer);
    while (iter != m_timers.end() && (*iter)->m_next == now_time)
    {
        ++iter;
    }
    expire.insert(expire.begin(), m_timers.begin(), iter);
    m_timers.erase(m_timers.begin(), iter);
    cbs.reserve(expire.size());
    
    for (auto timer : expire)
    {
        cbs.push_back(timer->m_cb);
        if (timer->m_recurring)
        {
            timer->m_next = now_time + timer->m_ms;
            m_timers.insert(timer);
        }
        else
        {
            timer->m_cb = nullptr;
        }
    }
}

bool TimerManager::hasTimer()
{
    RWMutexType::ReadLockGuard lock(m_mutex);
    return !m_timers.empty();
}

bool TimerManager::detectClockRollover(uint64_t now_time)
{
    bool rollover = false;
    if (now_time < m_previousTime && now_time < (m_previousTime - 60 * 60 * 1000))
        rollover = true;
    m_previousTime = now_time;
    return rollover;
}

}