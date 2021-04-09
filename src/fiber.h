#pragma once
#include <ucontext.h>
#include <memory>
#include <functional>
#include "log.h"

namespace TinyServer
{

class Fiber : public std::enable_shared_from_this<Fiber>
{
public:
    enum State
    {
        INIT, HOLD, EXEC, TERM, READY, EXCEPT
    };

public:
    Fiber(std::function<void()> cb, size_t stacksize = 0);
    ~Fiber();

    //重置协程函数，并重置状态
    //INIT TERM
    void reset(std::function<void()> cb);

    //切换到当前协程执行
    void swapIn();
    //切换到后台执行
    void swapOut();

    void call();

    //返回协程Id
    uint64_t getId() const { return m_id; }

    State getState() const { return m_state; }
    void setState(State state) { m_state = state; }

public:
    //设置当前协程
    static void SetThis(Fiber* f);
    //返回当前协程
    static Ref<Fiber> GetThis();
    //协程切换到后台，并设置为Ready状态
    static void YieldToReady();
    //协程切换到后台，并设置为Ready状态
    static void YieldToHold();
    //总协程数
    static uint64_t TotalFibers();

    static void MainFunc();

    static uint64_t GetFiberId();
private:
    Fiber();

private:
    uint64_t m_id = 0;
    uint32_t m_stacksize = 0;
    State m_state = State::INIT;
    ucontext_t m_context;
    void* m_stack = nullptr;
    std::function<void()> m_cb;

};


}