#pragma once
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include "thread.h"
#include "log.h"
#include "yaml-cpp/yaml.h"
namespace TinyServer
{
class ConfigVarBase
{
public:
    ConfigVarBase(std::string name, const std::string& description = "")
        : m_name(name), m_description(description)
    {
        std::transform(m_name.begin(), m_name.end(), m_name.begin(), ::tolower);
    }
    
    virtual ~ConfigVarBase() {}

    const std::string getName() const { return m_name; }
    const std::string getDescription() const { return m_description; }

    virtual std::string toString() = 0;
    virtual bool fromString(const std::string& val) = 0; 
    virtual std::string getTypeName() const = 0;


protected:
    std::string m_name;
    std::string m_description;
};

template<typename F, typename T>
class LexicalCast
{
public:
    T operator()(const F& f)
    {
        return boost::lexical_cast<T>(f);
    }
};

//偏特化
//vector
template<typename T>
class LexicalCast<std::string, std::vector<T>>
{
public:
    std::vector<T> operator()(const std::string& val)
    {
        YAML::Node node = YAML::Load(val);
        std::vector<T> vec;
        std::stringstream ss;
        for (size_t i = 0; i < node.size(); ++i)
        {
            ss.str("");
            ss << node[i];
            vec.push_back(LexicalCast<std::string, T>()(ss.str()));
        }
        return vec;
    }
};

template<typename T>
class LexicalCast<std::vector<T>, std::string>
{
public:
    std::string operator()(const std::vector<T>& val)
    {
        YAML::Node node;
        for (auto& item : val)
        {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(item)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

//list
template<typename T>
class LexicalCast<std::string, std::list<T>>
{
public:
    std::list<T> operator()(const std::string& val)
    {
        YAML::Node node = YAML::Load(val);
        std::list<T> vec;
        std::stringstream ss;
        for (size_t i = 0; i < node.size(); ++i)
        {
            ss.str("");
            ss << node[i];
            vec.push_back(LexicalCast<std::string, T>()(ss.str()));
        }
        return vec;
    }
};

template<typename T>
class LexicalCast<std::list<T>, std::string>
{
public:
    std::string operator()(const std::list<T>& val)
    {
        YAML::Node node;
        for (auto& item : val)
        {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(item)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

//set
template<typename T>
class LexicalCast<std::string, std::set<T>>
{
public:
    std::set<T> operator()(const std::string& val)
    {
        YAML::Node node = YAML::Load(val);
        std::set<T> vec;
        std::stringstream ss;
        for (size_t i = 0; i < node.size(); ++i)
        {
            ss.str("");
            ss << node[i];
            vec.insert(LexicalCast<std::string, T>()(ss.str()));
        }
        return vec;
    }
};

template<typename T>
class LexicalCast<std::set<T>, std::string>
{
public:
    std::string operator()(const std::set<T>& val)
    {
        YAML::Node node;
        for (auto& item : val)
        {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(item)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

//unordered_set
template<typename T>
class LexicalCast<std::string, std::unordered_set<T>>
{
public:
    std::unordered_set<T> operator()(const std::string& val)
    {
        YAML::Node node = YAML::Load(val);
        std::unordered_set<T> vec;
        std::stringstream ss;
        for (size_t i = 0; i < node.size(); ++i)
        {
            ss.str("");
            ss << node[i];
            vec.insert(LexicalCast<std::string, T>()(ss.str()));
        }
        return vec;
    }
};

template<typename T>
class LexicalCast<std::unordered_set<T>, std::string>
{
public:
    std::string operator()(const std::unordered_set<T>& val)
    {
        YAML::Node node;
        for (auto& item : val)
        {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(item)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

//map
template<typename T>
class LexicalCast<std::string, std::map<std::string, T>>
{
public:
    std::map<std::string, T> operator()(const std::string& val)
    {
        YAML::Node node = YAML::Load(val);
        std::map<std::string, T> vec;
        std::stringstream ss;
        for (auto iter = node.begin(); iter != node.end(); ++iter)
        {
            ss.str("");
            ss << iter->second;
            vec[iter->first.Scalar()] = LexicalCast<std::string, T>()(ss.str());
        }
        return vec;
    }
};

template<typename T>
class LexicalCast<std::map<std::string, T>, std::string>
{
public:
    std::string operator()(const std::map<std::string, T>& val)
    {
        YAML::Node node;
        for (auto& item : val)
        {
            node[item.first] = YAML::Load(LexicalCast<T, std::string>()(item.second));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

//unordered_map
template<typename T>
class LexicalCast<std::string, std::unordered_map<std::string, T>>
{
public:
    std::unordered_map<std::string, T> operator()(const std::string& val)
    {
        YAML::Node node = YAML::Load(val);
        std::unordered_map<std::string, T> vec;
        std::stringstream ss;
        for (auto iter = node.begin(); iter != node.end(); ++iter)
        {
            ss.str("");
            ss << iter->second;
            vec[iter->first.Scalar()] = LexicalCast<std::string, T>()(ss.str());
        }
        return vec;
    }
};

template<typename T>
class LexicalCast<std::unordered_map<std::string, T>, std::string>
{
public:
    std::string operator()(const std::unordered_map<std::string, T>& val)
    {
        YAML::Node node;
        for (auto& item : val)
        {
            node[item.first] = YAML::Load(LexicalCast<T, std::string>()(item.second));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

template<>
class LexicalCast<std::string, std::set<LogDefine>>
{
public:
    std::set<LogDefine> operator()(const std::string& val)
    {
        YAML::Node node = YAML::Load(val);
        std::set<LogDefine> vec;
        for (size_t i = 0; i < node.size(); ++i)
        {
            //std::cout << node.size() << "========================================\n";
            auto n = node[i];
            if (!n["name"].IsDefined())
            {
                std::cout << "log config error: name is null," << n << std::endl;
                continue;
            }
            LogDefine ld;
            ld.name = n["name"].as<std::string>();
            ld.level = LogLevel::FromString(n["level"].IsDefined() ? n["level"].as<std::string>() : "");
            if (n["formatter"].IsDefined())
            {
                ld.formatter = n["formatter"].as<std::string>();
            }
            if (n["appenders"].IsDefined())
            {
                for (size_t i = 0; i < n["appenders"].size(); ++i)
                {
                    auto ap = n["appenders"][i];
                    if (!ap["type"].IsDefined())
                    {
                        std::cout << "log config error: appender type is null," << n << std::endl;
                        continue;
                    }
                    std::string type = ap["type"].as<std::string>();
                    LogAppenderDefine lad;
                    if (type == "FileLog")
                    {
                        lad.type = 1;
                        if (!ap["file"].IsDefined())
                        {
                            std::cout << "log config error: file appender file path is null," << std::endl;
                            continue;
                        }
                        lad.file = ap["file"].as<std::string>();
                        if (ap["formatter"].IsDefined())
                        {
                            lad.formatter = ap["formatter"].as<std::string>();
                        }
                    }
                    else if (type == "ConsoleLog")
                    {
                        lad.type = 2;
                    }
                    else
                    {
                        std::cout << "log config error: appender type is invalid," << std::endl;
                        continue;
                    }
                    ld.appenders.push_back(lad);
                }
            }
            vec.insert(ld);
        }
        return vec;
    }
};

template<>
class LexicalCast<std::set<LogDefine>, std::string>
{
public:
    std::string operator()(const std::set<LogDefine>& vec)
    {
        YAML::Node node;
        for (auto& item : vec)
        {
            YAML::Node n;
            n["name"] = item.name;
            n["level"] = LogLevel::ToString(item.level);
            if (!item.formatter.empty())
            {
                n["formatter"] = item.formatter;
            }
            for (auto& i : item.appenders)
            {
                YAML::Node na;
                if (i.type == 1)
                {
                    na["type"] = "FileLog";
                    na["file"] = i.file;
                }
                else if (i.type == 2)
                {
                    na["type"] = "ConsoleLog";
                }
                //little bug item.level ==> i.level
                //否则appender还是会出现UNKNOW
                if (i.level != LogLevel::UNKNOW)
                    na["level"] = LogLevel::ToString(i.level);
                if (!i.formatter.empty())
                    na["formatter"] = i.formatter;
                n["appenders"].push_back(na);
            }
            node.push_back(n);
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};  


//FromStr T operator()(const string&)
//ToStr std::string operator()(const T&)
template<typename T, typename FromStr = LexicalCast<std::string, T>, 
                     typename ToStr = LexicalCast<T, std::string>>
class ConfigVar : public ConfigVarBase
{
public:
    typedef std::function<void (const T& old_value, const T& new_value)> ConfigChangeCB;
    typedef RWLock RWMutexType;

    ConfigVar(const std::string& name, const T& default_value, const std::string& description = "")
        : ConfigVarBase(name ,description), m_val(default_value) {}

    const T& getValue()
    {
        RWMutexType::ReadLockGuard lock(m_mutex);
        return m_val; 
    }

    void setValue(const T& value)
    {
        {
            RWMutexType::ReadLockGuard lock(m_mutex); 
            if (m_val == value) //operator==??
                return;
            for (auto& item : m_callbacks)
            {
                item.second(m_val, value);
            }
        }
        RWMutexType::WriteLockGuard lock(m_mutex);
        m_val = value;
    }

    uint64_t setCallBack(ConfigChangeCB cb)
    {
        static uint64_t s_init = 0;
        RWMutexType::WriteLockGuard lock(m_mutex);
        ++s_init;
        m_callbacks[s_init] = cb;
        return s_init;
    }

    void removeCallBack(uint64_t key)
    {
        RWMutexType::WriteLockGuard lock(m_mutex);
        m_callbacks.erase(key);
    }

    void clearCallBack()
    {
        RWMutexType::WriteLockGuard lock(m_mutex);
        m_callbacks.clear();
    }

    ConfigChangeCB getCallBack(uint64_t key)
    {
        RWMutexType::ReadLockGuard lock(m_mutex);
        auto iter = m_callbacks.find(key);
        return iter == m_callbacks.end() ? nullptr : m_callbacks[key];
    }

    std::string getTypeName() const override { return typeid(T).name(); }

    std::string toString() override
    {
        try
        {
            RWMutexType::ReadLockGuard lock(m_mutex);
            //return boost::lexical_cast<std::string>(m_val);
            return ToStr()(m_val);
        }
        catch(const std::exception& e)
        {
            TINY_LOG_ERROR(TINY_LOG_ROOT) << "Configvar::toString exception" << e.what() 
            << "convert " << typeid(m_val).name() << " to string";
        }
        return "";
    }
private:
    bool fromString(const std::string& val) override
    {
        try
        {
            //m_val = boost::lexical_cast<T>(val);
            setValue(FromStr()(val));
            return true;
        }
        catch(const std::exception& e)
        {
            TINY_LOG_ERROR(TINY_LOG_ROOT) << "Configvar::fromString exception" << e.what() 
            << "convert: string to " << typeid(m_val).name();
        }
        return false; 
    }

private:
    T m_val;
    std::map<uint64_t, ConfigChangeCB> m_callbacks;
    RWMutexType m_mutex;
};

class Config
{
public:
    typedef std::map<std::string, Ref<ConfigVarBase>> ConfigVarMaps; 
    typedef RWLock RWMutexType;
    template<typename T>
    static Ref<ConfigVar<T>> Lookup(const std::string& n, const T& default_value, const std::string& description = "")
    {
        RWMutexType::WriteLockGuard lock(GetMutex());
        std::string name = n;
        std::transform(name.begin(), name.end(), name.begin(), ::tolower);
        auto iter = GetDatats().find(name);
        if (iter != GetDatats().end())
        {
            //如果名字一样但类型不同那么转换失败返回nullptr
            auto temp = std::dynamic_pointer_cast<ConfigVar<T>>(iter->second);
            if (temp)
            {
                TINY_LOG_INFO(TINY_LOG_ROOT) << "Lookup name = " << name << " exists!";
                return temp;
            }
            else
            {
                TINY_LOG_ERROR(TINY_LOG_ROOT) << "Lookup name = " << name << " exists but type not " 
                << typeid(T).name() << " real type is: " << iter->second->getTypeName() << " " << iter->second->toString();
                return nullptr;
            }
        }

        if (name.find_first_not_of("abcdefghijklmnopqrstuvwxyz._0123456789") != std::string::npos)
        {
            TINY_LOG_ERROR(TINY_LOG_ROOT) << "Lookup name invalid " << name;
            throw std::invalid_argument(name);
        }
        Ref<ConfigVar<T>> config(new ConfigVar<T>(name, default_value, description));
        GetDatats()[name] = config;
        return config;
    }


    template<typename T>
    static Ref<ConfigVar<T>> Lookup(const std::string& name)
    {
        RWMutexType::ReadLockGuard lock(GetMutex());
        auto iter = GetDatats().find(name);
        if (iter == GetDatats().end())
            return nullptr;
        return std::dynamic_pointer_cast<ConfigVar<T>>(iter->second);
    }

    static void LoadFromYaml(YAML::Node& root);

    static Ref<ConfigVarBase> LookupBase(const std::string& name);

    static void Visit(std::function<void(Ref<ConfigVarBase>)> cb);

private:
    static ConfigVarMaps& GetDatats()
    {
        static ConfigVarMaps s_datas;
        return s_datas;
    }

    static RWMutexType& GetMutex()
    {
        static RWMutexType s_mutex;
        return s_mutex;
    }
};




} // namespace TinyServer
