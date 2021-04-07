#include "config.h"

namespace TinyServer
{
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
        TINY_LOG_ERROR(TINY_LOG_ROOT) << "Config invalid name: " << prefix << " : " << node;
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
    //std::cout << all_nodes.size() << std::endl;
    for (auto& item : all_nodes)
    {
        std::string key = item.first;
        if (key.empty())
            continue;
        std::transform(key.begin(), key.end(), key.begin(), ::tolower);
        auto config = LookupBase(key);
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
