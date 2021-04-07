#pragma once
#include <string>
#include <memory>
#include <list>
#include <fstream>
#include <vector>
#include <sstream>
#include <iostream>
#include <map>
#include <stdarg.h>
#include "util.h"
#include "Singleton.h"
#include "thread.h"

template<typename T>
using Ref = std::shared_ptr<T>;

#define TINY_LOG_LEVEL(logger, level)\
    if (logger->getLevel() <= level)\
    TinyServer::LogEventWarp(Ref<TinyServer::LogEvent>(new TinyServer::LogEvent(logger, level, __FILE__, __LINE__,\
    0, time(0), TinyServer::GetThreadId(), TinyServer::GetFiberId()))).getSS()

#define TINY_LOG_DEBUG(logger) TINY_LOG_LEVEL(logger, TinyServer::LogLevel::Level::DEBUG)
#define TINY_LOG_INFO(logger) TINY_LOG_LEVEL(logger, TinyServer::LogLevel::Level::INFO)
#define TINY_LOG_WARN(logger) TINY_LOG_LEVEL(logger, TinyServer::LogLevel::Level::WARN)
#define TINY_LOG_ERROR(logger) TINY_LOG_LEVEL(logger, TinyServer::LogLevel::Level::ERROR)
#define TINY_LOG_FATAL(logger) TINY_LOG_LEVEL(logger, TinyServer::LogLevel::Level::FATAL)

#define TINY_LOG_FORMAT_LEVEL(logger, level, fmt, ...)\
    if (logger->getLevel() <= level)\
    TinyServer::LogEventWarp(Ref<TinyServer::LogEvent>(new TinyServer::LogEvent(logger, level, __FILE__, __LINE__,\
    0, time(0), TinyServer::GetThreadId(), TinyServer::GetFiberId()))).getEvent()->format(fmt, ##__VA_ARGS__)

#define TINY_LOG_FORMAT_DEBUG(logger, fmt, ...) TINY_LOG_FORMAT_LEVEL(logger, TinyServer::LogLevel::Level::DEBUG, fmt, ##__VA_ARGS__)
#define TINY_LOG_FORMAT_INFO(logger, fmt, ...) TINY_LOG_FORMAT_LEVEL(logger, TinyServer::LogLevel::Level::INFO, fmt, ##__VA_ARGS__)
#define TINY_LOG_FORMAT_WARN(logger, fmt, ...) TINY_LOG_FORMAT_LEVEL(logger, TinyServer::LogLevel::Level::WARN, fmt, ##__VA_ARGS__)
#define TINY_LOG_FORMAT_ERROR(logger, fmt, ...) TINY_LOG_FORMAT_LEVEL(logger, TinyServer::LogLevel::Level::ERROR, fmt, ##__VA_ARGS__)
#define TINY_LOG_FORMAT_FATAL(logger, fmt, ...) TINY_LOG_FORMAT_LEVEL(logger, TinyServer::LogLevel::Level::FATAL, fmt, ##__VA_ARGS__)

#define TINY_LOG_ROOT TinyServer::LoggerMgr::GetIntance()->getRoot()
#define TINY_LOG_NAME(name) TinyServer::LoggerMgr::GetIntance()->getLogger(name)

namespace TinyServer
{

class Logger;
class LoggerMaager;

class LogLevel
{
public:
    enum Level
    {
        UNKNOW = 0, DEBUG, INFO, WARN, ERROR, FATAL
    };

    static const char* ToString(LogLevel::Level level);
    static LogLevel::Level FromString(const std::string& str);
};

class LogEvent
{
public:
    LogEvent(const Ref<Logger>& logger, LogLevel::Level level, 
        const char* file, uint32_t line, uint32_t elapse, 
        uint32_t time, uint32_t threadId, uint32_t fiberId)
        : m_logger(logger), m_level(level), m_file(file), 
        m_line(line), m_elapse(elapse), m_time(time), 
        m_threadId(threadId), m_fiberId(fiberId) {}

    const char* getFile() const { return m_file; }
    uint32_t getLine() const { return m_line; }
    uint32_t getElapse() const { return m_elapse; }
    uint32_t getTime() const { return m_time; }
    uint32_t getThreadId() const { return m_threadId; }
    uint32_t getcFiberId() const { return m_fiberId; }
    std::string getContent() const { return m_ss.str(); }
    std::stringstream& getSS() { return m_ss; }
    const Ref<Logger>& getLogger() const { return m_logger; }
    LogLevel::Level getLevel() const { return m_level; }

    void format(const char* fmt, ...)
    {
        va_list al;
        va_start(al, fmt);
        format(fmt, al);
        va_end(al);
    }

    void format(const char* fmt, va_list al)
    {
        char* buf = nullptr;
        int len = vasprintf(&buf, fmt, al);
        if (len != -1)
        {
            m_ss << std::string(buf, len);
            free(buf);
        }
    }


private:
    Ref<Logger> m_logger;
    LogLevel::Level m_level;
  
    const char* m_file;         //文件名
    uint32_t m_line;            //行号
    uint32_t m_elapse;          //程序启动到现在的毫秒数
    uint32_t m_time;            //时间戳
    uint32_t m_threadId;        //线程id
    uint32_t m_fiberId;         //协程id
    std::stringstream m_ss;     //消息内容
};

class LogEventWarp
{
public:
    LogEventWarp(const Ref<LogEvent>& event);

    ~LogEventWarp();

    const Ref<LogEvent>& getEvent() const { return m_event; }
    std::stringstream& getSS();

private:
    Ref<LogEvent> m_event;
};

class LogFormatter
{
public:
    LogFormatter(const std::string& pattern);
    std::string format(LogLevel::Level level, Ref<Logger>& logger, Ref<LogEvent>& event);
    void init();

public:
    class FormatItem
    {
    public:
        FormatItem(const std::string& fmt = "") {}
        virtual ~FormatItem() {}
        virtual void format(std::ostream& os, LogLevel::Level level, Ref<Logger>& logger, Ref<LogEvent>& event) = 0;
    };

    bool isError() { return m_error; }
    const std::string getPattern() const { return m_pattern; }

private:
    std::string m_pattern;
    std::vector<Ref<FormatItem>> m_items;
    bool m_error = false;
};

//Log Output
class LogAppender
{
friend class Logger;
public:
    typedef MutexLock MutexType;
    virtual void log(LogLevel::Level level, Ref<Logger>& logger, Ref<LogEvent>& event) = 0;
    virtual std::string toYamlString() = 0;
    virtual ~LogAppender() {}

    Ref<LogFormatter> getFomatter()
    {
        MutexType::MutexLockGuard lock(m_mutex);
        return m_formatter; 
    }
    void setFormatter(const Ref<LogFormatter>& fomratter)
    {
        MutexType::MutexLockGuard lock(m_mutex);    //加互斥锁
        m_formatter = fomratter;
        if (m_formatter)
            m_hasFormatter = true;
        else
            m_hasFormatter = false;
    }
    LogLevel::Level getLevel() const { return m_level; }
    void setLevel(LogLevel::Level level) { m_level = level; }
protected:
    LogLevel::Level m_level = LogLevel::DEBUG;
    Ref<LogFormatter> m_formatter;
    MutexType m_mutex;
    bool m_hasFormatter = false;
};

//Logger
//Output to Console Appender
class ConsoleLog : public LogAppender
{
public:
    void log(LogLevel::Level level, Ref<Logger>& logger, Ref<LogEvent>& event) override;
    std::string toYamlString() override;

private:
};

//Output to File Appender
class FileLog : public LogAppender
{
public:
    FileLog(const std::string& fileName);
    void log(LogLevel::Level level, Ref<Logger>& logger, Ref<LogEvent>& event) override;
    std::string toYamlString() override;
    bool reopen();
    //~FileLog() { m_fileStream.close(); }

private:
    std::string m_fileName;
    std::ofstream m_fileStream;
    uint64_t m_lastTime = 0;
};


class Logger : public std::enable_shared_from_this<Logger>
{
friend class LoggerManager;
public:
    typedef MutexLock MutexType;
    Logger(const std::string& name = "root");

    //Start Logger
    void log(LogLevel::Level level, Ref<LogEvent>& event);

    void debug(Ref<LogEvent>& event);

    void info(Ref<LogEvent>& event);

    void warn(Ref<LogEvent>& event);

    void error(Ref<LogEvent>& event);

    void fatal(Ref<LogEvent>& event);

    LogLevel::Level getLevel() const { return m_level; }
    void setLevel(LogLevel::Level level) { m_level = level; } 

    void addAppender(const Ref<LogAppender>& appender);
    void removeAppender(Ref<LogAppender>& appender);
    void clearAppenders();
    std::string toYamlString();

    void setFormatter(Ref<LogFormatter> val)
    {
        MutexType::MutexLockGuard lock(m_mutex);
        m_formatter = val;

        for (auto& item : m_appenders)
        {
            MutexType::MutexLockGuard la(item->m_mutex); //防止可能其他地方在读取appender的formatter
            if (!item->m_hasFormatter)
            {
                item->m_formatter = m_formatter;
            }
        }
    }

    void setFormatter(std::string fmt)
    {
        //Bug here setFormatter()锁了两次导致程序死锁
        //即单线程重复申请锁
        //MutexType::MutexLockGuard lock(m_mutex);
        Ref<LogFormatter> new_val(new LogFormatter(fmt));
        if (new_val->isError())
        {
            std::cout << "Logger setFormatter name = " << m_name
                      << " value = " << fmt <<" invalid formatter" << std::endl;
            return;
        }
        setFormatter(new_val);
    }

    Ref<LogFormatter> getFormatter() 
    {
        MutexType::MutexLockGuard lock(m_mutex); 
        return m_formatter; 
    }

    std::string getName() const { return m_name; }

private:
    std::string m_name;
    LogLevel::Level m_level;
    MutexType m_mutex;
    Ref<LogFormatter> m_formatter;
    std::list<Ref<LogAppender>> m_appenders;
    Ref<Logger> m_root;
};

class LoggerManager
{
public:
    typedef MutexLock MutexType;
    LoggerManager();

    Ref<Logger> getLogger(std::string name)
    {
        MutexType::MutexLockGuard lock(m_mutex); 
        auto iter = m_loggers.find(name);
        if (iter != m_loggers.end())
            return iter->second;
        Ref<Logger> logger(new Logger(name));
        logger->m_root = m_root;
        m_loggers[name] = logger;
        return logger;
    }

    void init();

    std::string toYamlString();

    Ref<Logger> getRoot() const { return m_root; }

private:
    MutexType m_mutex;
    Ref<Logger> m_root;
    std::map<std::string, Ref<Logger>> m_loggers;
};

using LoggerMgr = TinyServer::SingletonPtr<LoggerManager>;

struct LogAppenderDefine
{
    int type = 0; //1==>File, 2==>Console
    LogLevel::Level level = LogLevel::UNKNOW;
    std::string formatter;
    std::string file;

    bool operator==(const LogAppenderDefine& oth) const 
    {
        return type == oth.type
            && level == oth.level
            && formatter == oth.formatter
            && file == oth.file;
    }
};

struct LogDefine
{
    std::string name;
    LogLevel::Level level = LogLevel::UNKNOW;
    std::string formatter;
    std::vector<LogAppenderDefine> appenders;

    bool operator==(const LogDefine& oth) const
    {
        return name == oth.name
            && level == oth.level
            && formatter == oth.formatter
            && appenders == oth.appenders;
    }

    bool operator<(const LogDefine& oth) const
    {
        return name < oth.name;
    }
};

}

