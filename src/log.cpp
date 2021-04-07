#include "log.h"
#include <functional>
#include <iostream>
#include <ctime>
#include "config.h"

namespace TinyServer
{
const char* LogLevel::ToString(LogLevel::Level level)
{
    switch (level)
    {
    case LogLevel::Level::DEBUG:
        return "DEBUG";
    case LogLevel::Level::INFO:
        return "INFO";
    case LogLevel::Level::WARN:
        return "WARN";
    case LogLevel::Level::ERROR:
        return "ERROR";
    case LogLevel::Level::FATAL:
        return "FATAL";
    default:
        return "UNKNOW";
    }
    return "UNKNOW";
}

LogLevel::Level LogLevel::FromString(const std::string& str)
{
#define XX(level, name)\
    if (str == #name)\
    {\
        return LogLevel::level;\
    }
    XX(DEBUG, debug);
    XX(INFO, info);
    XX(WARN, warn);
    XX(ERROR, error);
    XX(FATAL, fatal);

    XX(DEBUG, DEBUG);
    XX(INFO, INFO);
    XX(WARN, WARN);
    XX(ERROR, ERROR);
    XX(FATAL, FATAL);
    return LogLevel::UNKNOW;
#undef XX


}

Logger::Logger(const std::string& name)
    : m_name(name), m_level(LogLevel::Level::DEBUG)
{
    //Default Format
    m_formatter.reset(new LogFormatter("%d{%Y-%m-%d %H:%M:%S}%T%t%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"));
}

void Logger::debug(Ref<LogEvent>& event)
{
    log(LogLevel::DEBUG, event);
}

void Logger::info(Ref<LogEvent>& event)
{
    log(LogLevel::INFO, event);
}
void Logger::warn(Ref<LogEvent>& event)
{
    log(LogLevel::WARN, event);
}

void Logger::error(Ref<LogEvent>& event)
{
    log(LogLevel::ERROR, event);
}

void Logger::fatal(Ref<LogEvent>& event)
{
    log(LogLevel::FATAL, event);
}

LogEventWarp::LogEventWarp(const Ref<LogEvent>& event)
    : m_event(event) {}

LogEventWarp::~LogEventWarp()
{
    m_event->getLogger()->log(m_event->getLevel(), m_event);
}

std::stringstream& LogEventWarp::getSS()
{
    return m_event->getSS();
}

void Logger::log(LogLevel::Level level, Ref<LogEvent>& event)
{
    if (m_level <= level)
    {
        if (!m_appenders.empty())
        {
            auto self = shared_from_this();
            MutexType::MutexLockGuard lock(m_mutex);
            for (auto& elem : m_appenders)
            {
                elem->log(level, self, event);
            }
        }
        else if (m_root)
        {
            m_root->log(level, event);
        }
    }
}

void Logger::addAppender(const Ref<LogAppender>& appender)
{
    MutexType::MutexLockGuard lock(m_mutex); 
    if (!appender->getFomatter())
    {
        MutexType::MutexLockGuard la(appender->m_mutex); 
        appender->m_formatter = m_formatter;
    }
    m_appenders.push_back(appender);
}

void Logger::removeAppender(Ref<LogAppender>& appender)
{
    MutexType::MutexLockGuard lock(m_mutex); 
    for (auto iter = m_appenders.begin(); iter != m_appenders.end(); ++iter)
    {
        if (*iter == appender)
        {
            m_appenders.erase(iter);
            break;
        }
    }
}

void Logger::clearAppenders()
{
    m_appenders.clear();
}

std::string Logger::toYamlString()
{
    MutexType::MutexLockGuard lock(m_mutex);
    YAML::Node node;
    node["name"] = m_name;
    node["level"] = LogLevel::ToString(m_level);
    if (m_formatter)
        node["formatter"] = m_formatter->getPattern();
    for (auto& item : m_appenders)
    {
        node["appenders"].push_back(YAML::Load(item->toYamlString()));
    }
    std::stringstream ss;
    ss << node;
    return ss.str();
}

void ConsoleLog::log(LogLevel::Level level, Ref<Logger>& logger, Ref<LogEvent>& event)
{
    if (m_level <= level)
    {
        MutexType::MutexLockGuard lock(m_mutex); 
        std::cout << m_formatter->format(level, logger, event);
    }
}

std::string ConsoleLog::toYamlString()
{
    MutexType::MutexLockGuard lock(m_mutex); 
    YAML::Node node;
    node["type"] = "ConsoleLog";
    if (m_level != LogLevel::UNKNOW)
        node["level"] = LogLevel::ToString(m_level);
    if (m_hasFormatter && m_formatter)
    {
        node["fomatter"] = m_formatter->getPattern();
    }
    std::stringstream ss;
    ss << node;
    return ss.str();
}

FileLog::FileLog(const std::string& fileName)
    : m_fileName(fileName)
{
    reopen();
}

bool FileLog::reopen()
{
    MutexType::MutexLockGuard lock(m_mutex); 
    if (m_fileStream)
    {
        m_fileStream.close();
    }
    m_fileStream.open(m_fileName);
    return !!m_fileStream;
}

void FileLog::log(LogLevel::Level level, Ref<Logger>& logger, Ref<LogEvent>& event)
{
    if (m_level <= level)
    {
        uint64_t now = time(0);
        if (now != m_lastTime)
        {
            reopen();
            m_lastTime = now;
        }
        MutexType::MutexLockGuard lock(m_mutex); 
        m_fileStream << m_formatter->format(level, logger, event);
    }
}

std::string FileLog::toYamlString()
{
    MutexType::MutexLockGuard lock(m_mutex); 
    YAML::Node node;
    node["type"] = "FileLog";
    node["file"] = m_fileName;
    if (m_level != LogLevel::UNKNOW)
        node["level"] = LogLevel::ToString(m_level);
    if (m_hasFormatter && m_formatter)
    {
        node["fomatter"] = m_formatter->getPattern();
    }
    std::stringstream ss;
    ss << node;
    return ss.str();
}

LogFormatter::LogFormatter(const std::string& pattern)
    : m_pattern(pattern)
{
    init();
}

std::string LogFormatter::format(LogLevel::Level level, Ref<Logger>& logger, Ref<LogEvent>& event)
{
    std::stringstream ss;
    for (auto& item : m_items)
    {
        item->format(ss, level, logger, event);
    }
    return ss.str();
}

class MessageFormatItem : public LogFormatter::FormatItem
{
public:
    MessageFormatItem(const std::string& fmt)
        : FormatItem(fmt) {}
    void format(std::ostream& os, LogLevel::Level level, Ref<Logger>& logger, Ref<LogEvent>& event) override
    {
        os << event->getContent();
    }
};

class LevelFormatItem : public LogFormatter::FormatItem
{
public:
    LevelFormatItem(const std::string& fmt)
        : FormatItem(fmt) {}
    void format(std::ostream& os, LogLevel::Level level, Ref<Logger>& logger, Ref<LogEvent>& event) override
    {
        os << LogLevel::ToString(level);
    }
};

class ElapseFormatItem : public LogFormatter::FormatItem
{
public:
    ElapseFormatItem(const std::string& fmt)
        : FormatItem(fmt) {}
    void format(std::ostream& os, LogLevel::Level level, Ref<Logger>& logger, Ref<LogEvent>& event) override
    {
        os << event->getElapse();
    }
};

class LoggerNameFormatItem : public LogFormatter::FormatItem
{
public:
    LoggerNameFormatItem(const std::string& fmt)
        : FormatItem(fmt) {}
    void format(std::ostream& os, LogLevel::Level level, Ref<Logger>& logger, Ref<LogEvent>& event) override
    {
        os << event->getLogger()->getName();
    }
};

class ThreadIdFormatItem : public LogFormatter::FormatItem
{
public:
    ThreadIdFormatItem(const std::string& fmt)
        : FormatItem(fmt) {}
    void format(std::ostream& os, LogLevel::Level level, Ref<Logger>& logger, Ref<LogEvent>& event) override
    {
        os << event->getThreadId();
    }
};

class FiberIdFormatItem : public LogFormatter::FormatItem
{
public:
    FiberIdFormatItem(const std::string& fmt)
        : FormatItem(fmt) {}
    void format(std::ostream& os, LogLevel::Level level, Ref<Logger>& logger, Ref<LogEvent>& event) override
    {
        os << event->getcFiberId();
    }
};

class DataTimeFormatItem : public LogFormatter::FormatItem
{
public:
    DataTimeFormatItem(const std::string& fmt = "%Y-%m-%d %H:%M:%S")
        : m_format(fmt) 
    {
        if (m_format.empty())
            m_format = "%Y-%m-%d %H:%M:%S";
    }
    void format(std::ostream& os, LogLevel::Level level, Ref<Logger>& logger, Ref<LogEvent>& event) override
    {
        struct tm tm;
        time_t time = event->getTime();
        localtime_r(&time, &tm);
        char buf[64];
        strftime(buf, sizeof(buf), m_format.c_str(), &tm);
        os << buf;
    }
private:
    std::string m_format;
};

class FileNameFormatItem : public LogFormatter::FormatItem
{
public:
    FileNameFormatItem(const std::string& fmt)
        : FormatItem(fmt) {}
    void format(std::ostream& os, LogLevel::Level level, Ref<Logger>& logger, Ref<LogEvent>& event) override
    {
        os << event->getFile();
    }
};

class LineFormatItem : public LogFormatter::FormatItem
{
public:
    LineFormatItem(const std::string& fmt)
        : FormatItem(fmt) {}
    void format(std::ostream& os, LogLevel::Level level, Ref<Logger>& logger, Ref<LogEvent>& event) override
    {
        os << event->getLine();
    }
};

class NewLineFormatItem : public LogFormatter::FormatItem
{
public:
    NewLineFormatItem(const std::string& fmt)
        : FormatItem(fmt) {}
    void format(std::ostream& os, LogLevel::Level level, Ref<Logger>& logger, Ref<LogEvent>& event) override
    {
        os << std::endl;
    }
};

class StringFormatItem : public LogFormatter::FormatItem
{
public:
    StringFormatItem(const std::string& str)
        : m_string(str) {}
    void format(std::ostream& os, LogLevel::Level level, Ref<Logger>& logger, Ref<LogEvent>& event) override
    {
        os << m_string;
    }

private:
    std::string m_string;
};

class TabFormatItem : public LogFormatter::FormatItem
{
public:
    TabFormatItem(const std::string& fmt)
        : FormatItem(fmt) {}
    void format(std::ostream& os, LogLevel::Level level, Ref<Logger>& logger, Ref<LogEvent>& event) override
    {
        os << '\t';
    }
};

//%d [%p] %f:%l %m
void LogFormatter::init() 
{
    //str, format, type
    std::vector<std::tuple<std::string, std::string, int>> vec;
    std::string nstr;
    for(size_t i = 0; i < m_pattern.size(); ++i) 
    {
        if(m_pattern[i] != '%') 
        {
            nstr.append(1, m_pattern[i]);
            continue;
        }

        if((i + 1) < m_pattern.size()) 
        {
            if(m_pattern[i + 1] == '%') 
            {
                nstr.append(1, '%');
                continue;
            }
        }

        size_t n = i + 1;
        int fmt_status = 0;
        size_t fmt_begin = 0;

        std::string str;
        std::string fmt;
        while(n < m_pattern.size()) 
        {
            if(!fmt_status && (!isalpha(m_pattern[n]) && m_pattern[n] != '{'
                    && m_pattern[n] != '}')) 
            {
                str = m_pattern.substr(i + 1, n - i - 1);
                break;
            }
            if(fmt_status == 0) 
            {
                if(m_pattern[n] == '{') {
                    str = m_pattern.substr(i + 1, n - i - 1);
                    //std::cout << "*" << str << std::endl;
                    fmt_status = 1; //解析格式
                    fmt_begin = n;
                    ++n;
                    continue;
                }
            } 
            else if(fmt_status == 1) 
            {
                if(m_pattern[n] == '}') 
                {
                    fmt = m_pattern.substr(fmt_begin + 1, n - fmt_begin - 1);
                    //std::cout << "#" << fmt << std::endl;
                    fmt_status = 0;
                    ++n;
                    break;
                }
            }
            ++n;
            if(n == m_pattern.size()) 
            {
                if(str.empty()) 
                {
                    str = m_pattern.substr(i + 1);
                }
            }
        }

        if(fmt_status == 0) 
        {
            if(!nstr.empty()) 
            {
                vec.push_back(std::make_tuple(nstr, std::string(), 0));
                nstr.clear();
            }
            vec.push_back(std::make_tuple(str, fmt, 1));
            i = n - 1;
        } 
        else if(fmt_status == 1) 
        {
            std::cout << "pattern parse error: " << m_pattern << " - " << m_pattern.substr(i) << std::endl;
            vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 0));
            m_error = true;
        }
    }

    if(!nstr.empty()) 
    {
        vec.push_back(std::make_tuple(nstr, "", 0));
    }
    static std::map<std::string, std::function<Ref<FormatItem>(const std::string& str)> > s_format_items = {
#define XX(str, C) \
        {#str, [](const std::string& fmt) { return Ref<FormatItem>(new C(fmt));}}

        XX(m, MessageFormatItem),
        XX(p, LevelFormatItem),
        XX(r, ElapseFormatItem),
        XX(c, LoggerNameFormatItem),
        XX(t, ThreadIdFormatItem),
        XX(n, NewLineFormatItem),
        XX(d, DataTimeFormatItem),
        XX(f, FileNameFormatItem),
        XX(l, LineFormatItem),
        XX(T, TabFormatItem),
        XX(F, FiberIdFormatItem),
#undef XX
    };

    for(auto& i : vec) 
    {
        if(std::get<2>(i) == 0) 
        {
            m_items.push_back(Ref<FormatItem>(new StringFormatItem(std::get<0>(i))));
        } 
        else 
        {
            auto it = s_format_items.find(std::get<0>(i));  //找到符合的FormatClass
            if(it == s_format_items.end()) 
            {
                m_items.push_back(Ref<FormatItem>(new StringFormatItem("<<error_format %" + std::get<0>(i) + ">>")));
                m_error = true;
            } 
            else 
            {
                m_items.push_back(it->second(std::get<1>(i)));
            }
        }
        //std::cout << "{" << std::get<0>(i) << "} - {" << std::get<1>(i) << "} - }" << std::get<2>(i) << "}" << std::endl;
    }
}

LoggerManager::LoggerManager()
{
    m_root.reset(new Logger());
    m_root->addAppender(Ref<ConsoleLog>(new ConsoleLog()));
    m_loggers[m_root->m_name] = m_root;
}

Ref<ConfigVar<std::set<LogDefine>>> log_config = Config::Lookup("logs", std::set<LogDefine>(), "defalut logs config");

struct LogIniter
{
    LogIniter()
    {
        log_config->setCallBack(0xF1E231, [](const std::set<LogDefine>& old_value, const std::set<LogDefine>& new_value){
            TINY_LOG_INFO(TINY_LOG_ROOT) << "ON Log Config Change Event";
            for (auto& item : new_value)
            {
                auto iter = old_value.find(item);
                Ref<Logger> logger;
                if (iter == old_value.end())
                {
                    //新增日志配置
                    logger = TINY_LOG_NAME(item.name);
                }
                else
                {
                    if (!(*iter == item))
                    {
                        //修改日志配置
                        logger = TINY_LOG_NAME(item.name);
                    }

                }
                logger->setLevel(item.level);
                if (!item.formatter.empty())
                {
                    logger->setFormatter(item.formatter);
                }
                logger->clearAppenders();
                for (auto& ap : item.appenders)
                {
                    Ref<LogAppender> appender;
                    if (ap.type == 1)
                    {
                        appender.reset(new FileLog(ap.file));
                    }
                    else if (ap.type == 2)
                    {
                        appender.reset(new ConsoleLog());
                    }
                    appender->setLevel(ap.level);
                    if (!ap.formatter.empty())
                    {
                        Ref<LogFormatter> fmt(new LogFormatter(ap.formatter));
                        if (!fmt->isError())
                        {
                            appender->setFormatter(fmt);
                        }
                        else
                        {
                            std::cout << "appender type = " << ap.type << "formatter " << ap.formatter << " is invalid" << std::endl;
                        }
                    }
                    //appender->setFormatter(logger->getFormatter());
                    logger->addAppender(appender);
                }
            }

            for (auto& item : old_value)
            {
                auto iter = new_value.find(item);
                if (iter == new_value.end())
                {
                    //新数据里没有而老数据里面有，触发删除
                    auto logger = TINY_LOG_NAME(item.name);
                    logger->setLevel((LogLevel::Level)100);
                    logger->clearAppenders();
                }
            }
        });
    }
};

//日志配置文件先于main函数初始化
//构造函数不带参数的类在初始化不能加括号
static LogIniter __log_init;

void LoggerManager::init()
{

}

std::string LoggerManager::toYamlString()
{
    MutexType::MutexLockGuard lock(m_mutex); 
    YAML::Node node;
    for (auto& item : m_loggers)
    {
        node.push_back(YAML::Load(item.second->toYamlString()));
    }
    std::stringstream ss;
    ss << node;
    return ss.str();
}

}//namespace
