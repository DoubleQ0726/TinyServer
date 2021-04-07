#include <iostream>
#include "log.h"
#include "util.h"
using namespace TinyServer;
int main()
{
    std::shared_ptr<Logger> logger(new Logger());
    logger->addAppender(Ref<LogAppender>(new ConsoleLog()));
    Ref<LogAppender> file_appender(new FileLog("./log.txt"));
    Ref<LogFormatter> fmt(new LogFormatter("%d%T[%p]%T%m%n"));
    file_appender->setFormatter(fmt);
    file_appender->setLevel(LogLevel::ERROR);
    logger->addAppender(file_appender);

    TINY_LOG_DEBUG(logger) << "Hello TinyServer";
    TINY_LOG_INFO(logger) << "INFO";
    TINY_LOG_ERROR(logger) << "ERROR!";
    TINY_LOG_FORMAT_DEBUG(logger, "Hello %s", "TinyServer");

    auto log = LoggerMgr::GetIntance()->getLogger("");
    TINY_LOG_DEBUG(log) << "Singleton Logger Manager";

    return 0;
}