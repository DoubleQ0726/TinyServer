#include "config.h"
#include "env.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


namespace TinyServer
{
static Ref<Logger> logger = TINY_LOG_NAME("system");

Ref<ConfigVarBase> Config::LookupBase(const std::string& name)
{
    RWMutexType::ReadLockGuard lock(GetMutex());
    auto iter = GetDatats().find(name);
    return iter == GetDatats().end() ? nullptr : iter->second;
}

static void ListAllMember(const std::string& prefix, 
                          YAML::Node& node, 
                          std::list<std::pair<std::string, const YAML::Node>>& output)
{
    if (prefix.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ._0123456789") != std::string::npos)
    {
        TINY_LOG_ERROR(logger) << "Config invalid name: " << prefix << " : " << node;
        return;
    }
    output.push_back(std::make_pair(prefix, node));
    if (node.IsMap())
    {
        for (auto iter = node.begin(); iter != node.end(); ++iter)
        {
            ListAllMember(prefix.empty() ? iter->first.Scalar() : 
            prefix + "." + iter->first.Scalar(), iter->second, output);
        }
    }
}

void Config::LoadFromYaml(YAML::Node& root)
{
    std::list<std::pair<std::string, const YAML::Node>> all_nodes;
    ListAllMember("", root, all_nodes);

    for (auto& item : all_nodes)
    {
        std::string key = item.first;
        if (key.empty())
            continue;
        std::transform(key.begin(), key.end(), key.begin(), ::tolower);
        Ref<ConfigVarBase> config = LookupBase(key);
        if (config)
        {
            if (item.second.IsScalar())
            {
                config->fromString(item.second.Scalar());
            }
            else
            {
                std::stringstream ss;
                ss << item.second;
                config->fromString(ss.str());
            }
        }
        
    }
}

static std::map<std::string, uint64_t> s_file2modifytime;
static MutexLock s_mutex;

void Config::LoadFromConfDir(const std::string& path)
{
    std::string absolute_path = EnvMgr::GetInstance()->getAbsolutionPath(path);
    std::vector<std::string> files;
    FSUtil::ListAllFile(files, absolute_path, ".yml");

    for (auto& item : files)
    {
        {
            struct stat st;
            lstat(item.c_str(), &st);
            MutexLock::MutexLockGuard lock(s_mutex);
            if (s_file2modifytime[item] == (uint64_t)st.st_mtime)        
                continue;
            s_file2modifytime[item] = (uint64_t)st.st_mtime;
        }
        try
        {
            YAML::Node root = YAML::LoadFile(item);
            LoadFromYaml(root);
            TINY_LOG_INFO(logger) << "LoadConfFile file = " << item << " ok";
        }
        catch(const std::exception& e)
        {
            TINY_LOG_ERROR(logger) << "LoadConfFile file = " << item << " failed";
        }
        
    }
}

void Config::Visit(std::function<void(Ref<ConfigVarBase>)> cb)
{
    RWMutexType::ReadLockGuard lock(GetMutex());
    ConfigVarMaps& m = GetDatats();
    for (auto iter = m.begin(); iter != m.end(); ++iter)
    {
        cb(iter->second);
    }
}

} // namespace TinyServer
