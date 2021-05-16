#include "fiber.h"
#include "config.h"
#include "macro.h"
#include <atomic>
#include "scheduler.h"

namespace TinyServer
{
static Ref<Logger> logger = TINY_LOG_NAME("system");
static std::atomic<uint64_t> s_fiber_id {0};
static std::atomic<uint64_t> s_fiber_count {0};

static thread_local Fiber* t_fiber = nullptr;
static thread_local Ref<Fiber> t_threadFiber = nullptr; // ==> main fiber

static Ref<ConfigVar<uint32_t>> fiber_stack_size = Config::Lookup<uint32_t>("fiber.stack.size", 1024 * 1024, "fiber stack size");

class MallocStackAllocator
{
public:
    static void* Alloc(size_t size)
    {
        return malloc(size);
    }

    static void Dealloc(void* ptr, size_t size)
    {
        return free(ptr);
        ptr = nullptr;
    }
};

using StackAllocator = MallocStackAllocator;

Fiber::Fiber()
{
    m_state = EXEC;
    SetThis(this);
    if (::getcontext(&m_context))
    {
        TINY_ASSERT_P(false, "getcontext");
    }
    ++s_fiber_count;
    TINY_LOG_DEBUG(logger) << "Fiber::Fiber id = " << m_id;
}

Fiber::Fiber(std::function<void()> cb, size_t stacksize, bool use_call)
    : m_id(++s_fiber_id), m_cb(cb)
{
    ++s_fiber_count;
    m_stacksize = m_stacksize ? stacksize : fiber_stack_size->getValue();
    m_stack = StackAllocator::Alloc(m_stacksize);

    if (::getcontext(&m_context))
    {
        TINY_ASSERT_P(false, "getcontext");
    }
    m_context.uc_link = nullptr;
    m_context.uc_stack.ss_sp = m_stack;
    m_context.uc_stack.ss_size = m_stacksize;
    if (!use_call)
        ::makecontext(&m_context, &MainFunc, 0);
    else
        ::makecontext(&m_context, &CallerMainFunc, 0);
    TINY_LOG_DEBUG(logger) << "Fiber::Fiber id = " << m_id;
}

Fiber::~Fiber()
{
    --s_fiber_count;
    if (m_stack)
    {
        TINY_ASSERT(m_state == State::TERM || m_state == State::INIT || m_state == State::EXCEPT);
        StackAllocator::Dealloc(m_stack, m_stacksize);
    }
    else
    {
        TINY_ASSERT(!m_cb);
        TINY_ASSERT(m_state == State::EXEC);

        Fiber* cur = t_fiber;
        if (cur == this)
            SetThis(nullptr);
    }
    TINY_LOG_DEBUG(logger) << "Fiber::~Fiber id = " << m_id;
}

//重置协程函数，并重置状态
//INIT TERM
void Fiber::reset(std::function<void()> cb)
{
    TINY_ASSERT(m_stack);
    TINY_ASSERT(m_state == State::INIT || m_state == State::TERM || m_state == State::EXCEPT);
    m_cb = cb;
    if (getcontext(&m_context))
    {
        TINY_ASSERT_P(false, "getcontext");
    }
    m_context.uc_link = nullptr;
    m_context.uc_stack.ss_sp = m_stack;
    m_context.uc_stack.ss_size = m_stacksize;
    ::makecontext(&m_context, &MainFunc, 0);
    m_state = State::INIT;
}

//切换到当前协程执行
void Fiber::swapIn()
{
    SetThis(this);
    TINY_ASSERT(m_state != State::EXEC);
    m_state = State::EXEC;
    if (::swapcontext(&(Scheduler::GetMainFiber()->m_context), &m_context))
    {
        TINY_ASSERT_P(false, "swapcontext");
    }
}

//切换到后台执行
void Fiber::swapOut()
{
    SetThis(Scheduler::GetMainFiber());
    if (::swapcontext(&m_context, &(Scheduler::GetMainFiber()->m_context)))
    {
        TINY_ASSERT_P(false, "swapcontext");
    }
}

void Fiber::call()
{
    SetThis(this);
    m_state = State::EXEC;
    if (::swapcontext(&(t_threadFiber->m_context), &m_context))
    {
        TINY_ASSERT_P(false, "swapcontext");
    }
}

void Fiber::back()
{
    SetThis(t_threadFiber.get());
    if (::swapcontext(&m_context, &(t_threadFiber->m_context)))
    {
        TINY_ASSERT_P(false, "swapcontext");
    }
}


//设置当前协程
void Fiber::SetThis(Fiber* f)
{
    t_fiber = f;
}

//返回当前协程
Ref<Fiber> Fiber::GetThis()
{
    if (t_fiber)
    {
        return t_fiber->shared_from_this();
    }
    Ref<Fiber> main_fiber(new Fiber);
    TINY_ASSERT(t_fiber == main_fiber.get());
    t_threadFiber = main_fiber;
    return t_fiber->shared_from_this();
}

//协程切换到后台，并设置为Ready状态
void Fiber::YieldToReady()
{
    Ref<Fiber> cur = GetThis();
    cur->m_state = State::READY;
    cur->swapOut();
}

//协程切换到后台，并设置为HOLD状态
void Fiber::YieldToHold()
{
    Ref<Fiber> cur = GetThis();
    TINY_ASSERT(cur->m_state == EXEC);
    //cur->m_state = State::HOLD;
    cur->swapOut();
}
//总协程数
uint64_t Fiber::TotalFibers()
{
    return s_fiber_count;
}

void Fiber::MainFunc()
{
    Ref<Fiber> cur = GetThis();
    TINY_ASSERT(cur);
    try
    {
        cur->m_cb();
        cur->m_cb = nullptr;
        cur->m_state = State::TERM;
    }
    catch(const std::exception& e)
    {
        cur->m_state = State::EXCEPT;
        TINY_LOG_ERROR(logger) << "Fiber Except: " << e.what() << " fiber_id = " << cur->m_id << "\n" << BackTraceToString();
    }
    auto raw_ptr = cur.get();
    cur.reset();
    raw_ptr->swapOut();

    TINY_ASSERT_P(false, "never reach fiber_id = " + std::to_string(raw_ptr->m_id));
}

void Fiber::CallerMainFunc()
{
    Ref<Fiber> cur = GetThis();
    TINY_ASSERT(cur);
    try
    {
        cur->m_cb();
        cur->m_cb = nullptr;
        cur->m_state = State::TERM;
    }
    catch(const std::exception& e)
    {
        cur->m_state = State::EXCEPT;
        TINY_LOG_ERROR(logger) << "Fiber Except: " << e.what() << " fiber_id = " << cur->m_id << "\n" << BackTraceToString();
    }
    auto raw_ptr = cur.get();
    cur.reset();
    raw_ptr->back();

    TINY_ASSERT_P(false, "never reach fiber_id = " + std::to_string(raw_ptr->m_id));
}

uint64_t Fiber::GetFiberId()
{
    if (t_fiber)
    {
        return t_fiber->getId();
    }
    return 0;
}

    
}