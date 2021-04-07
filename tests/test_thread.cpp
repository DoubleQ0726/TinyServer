#include "TinyServer.h"
#include <unistd.h>
using namespace TinyServer;


typedef MutexLock MutexType;
Ref<Logger> logger = TINY_LOG_ROOT;
int count = 0;

//RWLock s_mutex;
MutexType s_mutex;

void func1()
{
    TINY_LOG_INFO(logger) << "name: " << Thread::GetName()
                          << " this.name: " << Thread::GetThis()->getName()
                          << " id: " << GetThreadId()
                          << " this.id: " << Thread::GetThis()->getID();
    //sleep(5);
    for (int i = 0; i < 10000000; ++i)
    {
        //MutexType::WriteLockGuard lock(s_mutex);
        MutexType::MutexLockGuard lock(s_mutex);
        ++count;
    }
}

void func2()
{   
    while (true)
    {
        TINY_LOG_INFO(logger) << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
    }
}

void func3()
{
    while (true)
    {
        TINY_LOG_INFO(logger) << "=========================================";
    }
}


int main()
{
    TINY_LOG_INFO(logger) << "test thread begin";
    // YAML::Node root = YAML::LoadFile("../bin/config/test1.yml");
    // Config::LoadFromYaml(root);
    std::vector<Ref<Thread>> threads;
    for (size_t i = 0; i < 5; ++i)
    {
        // Ref<Thread> thread1(new Thread(&func2, "name_" + std::to_string(i * 2)));
        // Ref<Thread> thread2(new Thread(&func3, "name_" + std::to_string(i * 2 + 1)));
        // threads.push_back(thread1);
        // threads.push_back(thread2);
        Ref<Thread> thread(new Thread(&func1, "name_" + std::to_string(i)));
        threads.push_back(thread);
    }
    for (size_t i = 0; i < threads.size(); ++i)
    {
        threads[i]->join();
    }

    TINY_LOG_INFO(logger) << "test thread end";
    TINY_LOG_INFO(logger) << "count = " << count;

    return 0;
}