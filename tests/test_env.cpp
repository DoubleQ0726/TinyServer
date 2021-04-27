#include "env.h"
#include <iostream>
#include <fstream>
#include <unistd.h>

using namespace TinyServer;

struct A
{
    A()
    {
        std::ifstream ifs("/proc/" + std::to_string(getpid()) + "/cmdline", std::ios::binary |std::ios::in);
        std::string content;
        content.resize(4096);

        ifs.read(&content[0], content.size());
        content.resize(ifs.gcount());
        for (size_t i = 0; i < content.size(); ++i)
        {
            std::cout << i << " - " << content[i] << " - " << (int)content[i] << std::endl;
        }
        std::cout << content << std::endl;
    }
};

A a;

int main(int argc, char** argv)
{
    EnvMgr::GetInstance()->addHelp("s", "start with terminal");
    EnvMgr::GetInstance()->addHelp("d", "run as daemon");
    EnvMgr::GetInstance()->addHelp("p", "print help");
    if (!EnvMgr::GetInstance()->init(argc, argv))
    {
        EnvMgr::GetInstance()->printHelp();
        return 0;
    }

    std::cout << "exe = " << EnvMgr::GetInstance()->getExe() << std::endl;
    std::cout << "cwd = " << EnvMgr::GetInstance()->getCwd() << std::endl;
    std::cout << "PATH = " << EnvMgr::GetInstance()->getEnv("PATH", "xxx") << std::endl;
    std::cout << "TEST = " << EnvMgr::GetInstance()->getEnv("TEST", "xxx") << std::endl;
    std::cout << "SET = " << EnvMgr::GetInstance()->setEnv("TEST", "yy") << std::endl;
    std::cout << "TEST = " << EnvMgr::GetInstance()->getEnv("TEST", "xxx") << std::endl;
    if (EnvMgr::GetInstance()->has("p"))
    {
        EnvMgr::GetInstance()->printHelp();
    }
    return 0;
}