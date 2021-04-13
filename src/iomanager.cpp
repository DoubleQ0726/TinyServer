#include "iomanager.h"
#include "macro.h"
#include "log.h"
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

namespace TinyServer
{
static Ref<Logger> logger = TINY_LOG_NAME("system");


IOManager::IOManager(size_t threads, bool use_call, const std::string& name)
    : Scheduler(threads, use_call, name)
{
    m_epollfd = epoll_create(5000);
    TINY_ASSERT(m_epollfd >= 0);

    int res = pipe(m_ticklefd);
    TINY_ASSERT(res == 0);

    epoll_event event;
    memset(&event, 0, sizeof(event));
    event.events = EPOLLIN | EPOLLET;
    event.data.fd = m_ticklefd[0];

    res = fcntl(m_ticklefd[0], F_SETFL, O_NONBLOCK);
    TINY_ASSERT(res >= 0);

    res = epoll_ctl(m_epollfd, EPOLL_CTL_ADD, m_ticklefd[0], &event);
    TINY_ASSERT(res == 0);

    eventResize(32);
    start();
}

IOManager::~IOManager()
{
    stop();
    close(m_epollfd);
    close(m_ticklefd[0]);
    close(m_ticklefd[1]);

    for (size_t i = 0; i < m_fdEvents.size(); ++i)
    {
        if (m_fdEvents[i])
            delete m_fdEvents[i];
    }
}

IOManager::FdEvent::Event& IOManager::FdEvent::getEvent(IOManager::EventType et)
{
    switch (et)
    {
    case READ:
        return read;
    case WRITE:
        return write;
    default:
        TINY_ASSERT_P(false, "getEvent");
    }
}

void IOManager::FdEvent::resetEvent(Event e)
{
    e.scheduler = nullptr;
    e.fiber.reset();
    e.cb = nullptr;
}

void IOManager::FdEvent::triggerEvent(EventType eventtype)
{
    TINY_ASSERT(et & eventtype);
    et = (EventType)(et & ~eventtype);
    Event& event = getEvent(eventtype);
    if (event.cb)
    {
        event.scheduler->schedule(&event.cb);
    }
    else
    {
        event.scheduler->schedule(&event.fiber);
    }
    event.scheduler = nullptr;
    return;
}

void IOManager::eventResize(size_t size)
{
    m_fdEvents.resize(size);
    for (size_t i = 0; i < m_fdEvents.size(); ++i)
    {
        if (!m_fdEvents[i])
        {
            m_fdEvents[i] = new FdEvent();
            m_fdEvents[i]->fd = i;
        }
    }
}

// 0 success, -1 error
int IOManager::addEvent(int fd, EventType et, std::function<void()> cb)
{
    TINY_LOG_INFO(logger) << "addEvent";
    FdEvent* fd_event = nullptr;
    RWMutexType::ReadLockGuard lock(m_mutex);
    if ((int)m_fdEvents.size() > fd)
    {
        fd_event = m_fdEvents[fd];
        lock.unlock();
    }
    else
    {
        lock.unlock();
        RWMutexType::WriteLockGuard lock2(m_mutex);
        eventResize(fd * 1.5);
        fd_event = m_fdEvents[fd];
    }
    FdEvent::MutexType::MutexLockGuard lock2(fd_event->mutex);
    if (fd_event->et == et)
    {
        TINY_LOG_ERROR(logger) << "addEvent assert fd = " << fd << " event = " << et
            << " fd_event.event = " << fd_event->et;
        TINY_ASSERT(!(fd_event->et & et));
    }
    int op = fd_event->et ? EPOLL_CTL_MOD : EPOLL_CTL_ADD;
    epoll_event epevent;
    epevent.events = EPOLLET | fd_event->et | et;
    epevent.data.ptr = fd_event;

    int res = epoll_ctl(m_epollfd, op, fd, &epevent);
    if (res)
    {
        TINY_LOG_ERROR(logger) << "epoll_ctl(" << m_epollfd <<", "
            << op << ", " << fd << ", " << epevent.events << "): "
            << res << " (" << errno << ") (" << strerror(errno) << ")";
        return -1;
    }
    ++m_pendingEventCount;
    fd_event->et = (EventType)(fd_event->et | et);
    FdEvent::Event& event = fd_event->getEvent(et);
    TINY_ASSERT(!event.scheduler && !event.fiber && !event.cb);
    event.scheduler = Scheduler::GetThis();
    if (cb)
    {
        event.cb.swap(cb);
    }
    else
    {
        event.fiber = Fiber::GetThis();
        TINY_ASSERT(event.fiber->getState() == Fiber::EXEC);
    }
    return 0;
}

bool IOManager::delEvent(int fd, EventType et)
{
    RWMutexType::ReadLockGuard lock(m_mutex);
    if ((int)m_fdEvents.size() <= fd)
        return false;
    FdEvent* fd_event = nullptr;
    fd_event = m_fdEvents[fd];
    lock.unlock();

    FdEvent::MutexType::MutexLockGuard lock2(fd_event->mutex);
    if (!(fd_event->et & et))
        return false;
    EventType new_event = (EventType)(fd_event->et & ~et);
    int op = new_event ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
    epoll_event epevent;
    epevent.events = EPOLLET | new_event;
    epevent.data.ptr = fd_event;

    int res = epoll_ctl(m_epollfd, op, fd, &epevent);
    if (res)
    {
        TINY_LOG_ERROR(logger) << "epoll_ctl(" << m_epollfd <<", "
            << op << ", " << fd << ", " << epevent.events << "): "
            << res << " (" << errno << ") (" << strerror(errno) << ")";
        return false;
    }
    --m_pendingEventCount;
    fd_event->et = new_event;
    FdEvent::Event& event = fd_event->getEvent(et);
    fd_event->resetEvent(event);
    return true;
}

bool IOManager::cancelEvent(int fd, EventType et)
{
    RWMutexType::ReadLockGuard lock(m_mutex);
    if ((int)m_fdEvents.size() <= fd)
        return false;
    FdEvent* fd_event = nullptr;
    fd_event = m_fdEvents[fd];
    lock.unlock();

    FdEvent::MutexType::MutexLockGuard lock2(fd_event->mutex);
    if (!(fd_event->et & et))
        return false;
    EventType new_event = (EventType)(fd_event->et & ~et);
    int op = new_event ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
    epoll_event epevent;
    epevent.events = EPOLLET | new_event;
    epevent.data.ptr = fd_event;

    int res = epoll_ctl(m_epollfd, op, fd, &epevent);
    if (res)
    {
        TINY_LOG_ERROR(logger) << "epoll_ctl(" << m_epollfd <<", "
            << op << ", " << fd << ", " << epevent.events << "): "
            << res << " (" << errno << ") (" << strerror(errno) << ")";
        return false;
    }
    //FdEvent::Event& event = fd_event->getEvent(et);
    fd_event->triggerEvent(et);
    --m_pendingEventCount;
    return true;
}

bool IOManager::cancelAll(int fd)
{
    RWMutexType::ReadLockGuard lock(m_mutex);
    if ((int)m_fdEvents.size() <= fd)
        return false;
    FdEvent* fd_event = nullptr;
    fd_event = m_fdEvents[fd];
    lock.unlock();

    FdEvent::MutexType::MutexLockGuard lock2(fd_event->mutex);
    if (!fd_event->et)
        return false;
    int op = EPOLL_CTL_DEL;
    epoll_event epevent;
    epevent.events = 0;
    epevent.data.ptr = fd_event;

    int res = epoll_ctl(m_epollfd, op, fd, &epevent);
    if (res)
    {
        TINY_LOG_ERROR(logger) << "epoll_ctl(" << m_epollfd <<", "
            << op << ", " << fd << ", " << epevent.events << "): "
            << res << " (" << errno << ") (" << strerror(errno) << ")";
        return false;
    }

    if (fd_event->et == READ)
    {
        //FdEvent::Event& event = fd_event->getEvent(READ);
        fd_event->triggerEvent(READ);
        --m_pendingEventCount;
    }
    if (fd_event->et == WRITE)
    {
        //FdEvent::Event& event = fd_event->getEvent(WRITE);
        fd_event->triggerEvent(WRITE);
        --m_pendingEventCount;
    }
    TINY_ASSERT(fd_event->et == 0);
    return true;
}

IOManager* IOManager::GetThis()
{
    return dynamic_cast<IOManager*>(Scheduler::GetThis());
}

void IOManager::tickle()
{
    if (hasIdleThreads())
        return;
    //TINY_LOG_INFO(logger) << "tick";
    int res = ::write(m_ticklefd[1], "T", 1);
    TINY_ASSERT(res == 1);
}

bool IOManager::stopping()
{
    uint64_t time_out = 0;
    return stopping(time_out);
}

bool IOManager::stopping(uint64_t& time_out)
{
    time_out = getNextTimer();
    return time_out == ~0ull
        && m_pendingEventCount == 0
        && Scheduler::stopping();
}

void IOManager::idle()
{
    epoll_event* epevents = new epoll_event[64];
    std::shared_ptr<epoll_event> shared_events(epevents, [](epoll_event* ptr){
        delete[] ptr;
    });

    while (true)
    {
        uint64_t next_timeout = 0;
        if (stopping(next_timeout))
        {
            TINY_LOG_INFO(logger) << "name = " << getName() << " idle stopping exit";
            break;
        }
        int res = 0;
        do
        {
            static const int MAX_TIMEOUT = 5000;
            if (next_timeout != ~0ull)
                next_timeout = (int)next_timeout < MAX_TIMEOUT ? next_timeout : MAX_TIMEOUT;
            else
                next_timeout = MAX_TIMEOUT;
            res = epoll_wait(m_epollfd, epevents, 64, next_timeout);
            //TINY_LOG_INFO(logger) << next_timeout;
            //TINY_LOG_INFO(logger) << "epoll_wait res = " << res;

            if (res < 0 && errno == EINTR)
            {

            }
            else
            {
                break;
            }
        } while (true);

        std::vector<std::function<void()>> cbs;
        listExpireCB(cbs);
        if (!cbs.empty())
        {
            schedule(cbs.begin(), cbs.end());
            cbs.clear();
        }

        for (int i = 0; i < res; ++i)
        {
            epoll_event& epevent = epevents[i];
            if (epevents->data.fd == m_ticklefd[0])
            {
                uint8_t dummy;
                while (read(m_ticklefd[0], &dummy, 1) == 1)
                {
                    //TINY_LOG_INFO(logger) << "T";
                }
                continue;
            }
            FdEvent* fd_event = (FdEvent*)epevent.data.ptr;
            FdEvent::MutexType::MutexLockGuard lock(fd_event->mutex);
            if (epevent.events & (EPOLLERR | EPOLLHUP))
            {
                epevent.events |= (EPOLLIN | EPOLLOUT); 
            }
            EventType real_event_type = NONE;
            if (epevent.events & EPOLLIN)
            {
                real_event_type = READ;  
            }
            if (epevent.events & EPOLLOUT)
            {
                real_event_type = WRITE;
            }
            if ((real_event_type & fd_event->et) == NONE)
                continue;
            
            EventType left_type = (EventType)(fd_event->et & ~real_event_type);
            int op = left_type ? EPOLL_CTL_MOD :EPOLL_CTL_DEL;
            epevent.events |= (EPOLLET | left_type);

            int res2 = epoll_ctl(m_epollfd, op, fd_event->fd, &epevent);
            if (res2)
            {
                TINY_LOG_ERROR(logger) << "epoll_ctl(" << m_epollfd <<", "
                    << op << ", " << fd_event->fd << ", " << epevent.events << "): "
                    << res2 << " (" << errno << ") (" << strerror(errno) << ")";
                continue;
            }
            if (real_event_type & READ)
            {
                fd_event->triggerEvent(READ);
                --m_pendingEventCount;
            }
            if (real_event_type & WRITE)
            {
                fd_event->triggerEvent(WRITE);
                --m_pendingEventCount;
            }
        }
        Ref<Fiber> cur = Fiber::GetThis();
        Fiber* raw_ptr = cur.get();
        cur.reset();
        raw_ptr->swapOut();
    }  
}

void IOManager::onTimerInsertAtFront()
{
    tickle();
}



}