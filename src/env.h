#pragma once
#include "thread.h"
#include "Singleton.h"
#include <map>
#include <vector>
#include <stdlib.h>

namespace TinyServer
{

class Env
{
public:
    typedef RWLock RWMutexType;
    bool init(int argc, char** argv);

    void add(const std::string& key, const std::string& value);
    bool has(const std::string& key, const std::string& value = "");
    void del(const std::string& key);
    std::string get(const std::string& key, const std::string& default_value = "");

    void addHelp(const std::string& key, const std::string& desc);
    void removeHelp(const std::string& key);
    void printHelp();

    std::string getExe() const { return m_exe; }
    std::string getCwd() const { return m_cwd; }

    bool setEnv(const std::string& key, const std::string& value);
    std::string getEnv(const std::string& key, const std::string& default_value = "");

    std::string getAbsolutionPath(const std::string& path) const;

private:
    RWMutexType m_mutex;
    std::map<std::string, std::string> m_args;
    std::vector<std::pair<std::string, std::string>> m_helps;
    std::string m_program;

    std::string m_exe;
    std::string m_cwd;
};
typedef Singleton<Env> EnvMgr;

}