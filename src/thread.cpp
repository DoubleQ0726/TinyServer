#include "thread.h"
#include "log.h"
#include "util.h"

namespace TinyServer
{

static thread_local Thread* t_thread = nullptr;
static thread_local std::string t_name = "UNKNOW";
static Ref<Logger> logger = TINY_LOG_NAME("system");

Semaphore::Semaphore(const uint32_t count)
{
    if (sem_init(& m_semaphore, 0, count))
    {
        throw std::logic_error("sem_init error");
    }
}

void Semaphore::wait()
{
    if (sem_wait(&m_semaphore))
    {
        throw std::logic_error("sem_wait error");
    }  
}
    
void Semaphore::notify()
{
    if (sem_post(&m_semaphore))
    {
        throw std::logic_error("sem_post error");
    }
}


Thread* Thread::GetThis()
{
    return t_thread;
}

std::string Thread::GetName()
{
    return t_name;
}

void Thread::SetName(std::string name)
{
    if (t_thread)
        t_thread->m_name = name;
    t_name = name;
}

Thread::Thread(std::function<void()> cb, const std::string& name)
    : m_id(-1), m_thread(0), m_callBack(cb), m_name(name)
{
    if (name.empty())
        m_name = "UNKNOW";
    int rt = pthread_create(&m_thread, nullptr, &Thread::run, this);
    if (rt)
    {
        TINY_LOG_ERROR(logger) << "pthread_create thread fail, rt = " << rt << "name = " << name; 
        throw std::logic_error("pthread_create error");
    }
    m_semaphore.wait();
}

Thread::~Thread()
{
    if (m_thread)
        pthread_detach(m_thread);
}
    

void Thread::join()
{
    if (m_thread)
    {
        int rt = pthread_join(m_thread, nullptr);
        if (rt)
        {
            TINY_LOG_ERROR(logger) << "pthread_join thread fail, rt = " << rt << "name = " << m_name; 
            throw std::logic_error("pthread_join error");
        }
        m_thread = 0;
    }
}


void* Thread::run(void* arg)
{
    Thread* thread = (Thread*)arg;
    t_thread = thread;
    t_name = thread->m_name;
    thread->m_id = GetThreadId();
    //Set thread name visible in the kernel and its interfaces.
    pthread_setname_np(pthread_self(), thread->m_name.substr(0, 15).c_str());

    std::function<void()> cb;
    cb.swap(thread->m_callBack);

    thread->m_semaphore.notify();

    cb();
    return 0;
}



}