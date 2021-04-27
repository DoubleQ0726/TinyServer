#include "env.h"
#include <iomanip>
#include <string.h>
#include "log.h"

namespace TinyServer
{
static Ref<Logger> logger = TINY_LOG_NAME("system");

bool Env::init(int argc, char** argv)
{
    char link[1024] = {0};
    char path[1024] = {0};
    sprintf(link, "/proc/%d/exe", getpid());
    readlink(link, path, sizeof(path));
    m_exe = path;
    auto pos = m_exe.find_last_of('/');
    m_cwd = m_exe.substr(0, pos) + "/";
    m_program = argv[0];
    //-config /path/to/config -file xxx
    const char* now_key = nullptr;
    for (int i = 1; i < argc; ++i)
    {
        if (argv[i][0] == '-')
        {
            if (strlen(argv[i]) > 1)
            {
                if (now_key)
                {
                    add(now_key, "");
                }
                now_key = argv[i] + 1;
            }
            else
            {
                TINY_LOG_ERROR(logger) << "invalid arg idx = " << i << " val = " << argv[i];
                return false;
            }
        }
        else
        {
            if (now_key)
            {
                add(now_key, argv[i]);
                now_key = nullptr;
            }
            else
            {
                TINY_LOG_ERROR(logger) << "invalid arg idx = " << i << " val = " << argv[i];
                return false;
            }
        }
    }
    if (now_key)
        add(now_key, "");
    return true;
}

void Env::add(const std::string& key, const std::string& value)
{
    RWMutexType::WriteLockGuard lock(m_mutex);
    m_args[key] = value;
}

bool Env::has(const std::string& key, const std::string& value)
{
    RWMutexType::ReadLockGuard lock(m_mutex);
    auto iter = m_args.find(key);
    return iter != m_args.end();
}

void Env::del(const std::string& key)
{
    RWMutexType::WriteLockGuard lock(m_mutex);
    m_args.erase(key);
}

std::string Env::get(const std::string& key, const std::string& default_value)
{
    RWMutexType::ReadLockGuard lock(m_mutex);
    auto iter = m_args.find(key);
    return iter != m_args.end() ? iter->second : default_value;
}

void Env::addHelp(const std::string& key, const std::string& desc)
{
    removeHelp(key);
    m_helps.push_back(std::make_pair(key, desc));
}

void Env::removeHelp(const std::string& key)
{
    RWMutexType::WriteLockGuard lock(m_mutex);
    for (auto iter = m_helps.begin(); iter != m_helps.end();)
    {
        if (iter->first == key)
            iter = m_helps.erase(iter);
        else
            ++iter;
    }
}

void Env::printHelp()
{
    RWMutexType::ReadLockGuard lock(m_mutex);
    std::cout << "Usage: " << m_program << " [options]" << std::endl;
    for (auto& item : m_helps)
    {
        std::cout << std::setw(5) << "-" << item.first << " : " << item.second << std::endl; 
    }
}

bool Env::setEnv(const std::string& key, const std::string& value)
{
    return setenv(key.c_str(), value.c_str(), 1);
}

std::string Env::getEnv(const std::string& key, const std::string& default_value)
{
    const char* v = getenv(key.c_str());
    if (v == nullptr)
        return default_value;
    return v;
}

std::string Env::getAbsolutionPath(const std::string& path) const
{
    if (path.empty())
        return "/";
    if (path[0] == '/')
        return path;
    return m_cwd + path;
}

}