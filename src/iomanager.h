#pragma once
#include "scheduler.h"
#include "timer.h"
namespace TinyServer
{
class IOManager : public Scheduler, public TimerManager 
{
public:
    typedef RWLock RWMutexType;
    enum EventType
    {
        NONE = 0x00, 
        READ = 0x01,    //EPOLLIN
        WRITE = 0x04    //EPOLLOUT
    };

private:
    struct FdEvent
    {
        typedef MutexLock MutexType;
        struct Event
        {
            Scheduler* scheduler = nullptr; //事件执行的scheduler
            Ref<Fiber> fiber;               //事件协程
            std::function<void()> cb;       //事件回调
        };

        Event& getEvent(EventType et);
        void resetEvent(Event e);
        void triggerEvent(EventType eventtype);
        Event read;                 //读事件
        Event write;                //写事件
        int fd = 0;                 //事件关联的句柄
        EventType et = NONE;        //已注册的事件类型
        MutexType mutex;
    };

public:
    IOManager(size_t threads = 1, bool use_call = true, const std::string& name = "");

    ~IOManager();

    // 1 success, 0 retary, -1 error
    int addEvent(int fd, EventType et, std::function<void()> cb = nullptr);
    bool delEvent(int fd, EventType et);
    bool cancelEvent(int fd, EventType et);
    bool cancelAll(int fd);

    static IOManager* GetThis();

protected:
    void tickle() override;
    bool stopping() override;
    bool stopping(uint64_t& time_out);
    void idle() override;

    void onTimerInsertAtFront() override;

    void eventResize(size_t size);

private:
    int m_epollfd;
    int m_ticklefd[2];  //用于唤醒epoll_wait(tick)
    std::atomic<size_t> m_pendingEventCount = {0};
    RWMutexType m_mutex;
    std::vector<FdEvent*> m_fdEvents;
};

}