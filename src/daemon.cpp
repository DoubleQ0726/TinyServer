#include "daemon.h"
#include <time.h>
#include <string.h>
#include "log.h"
#include "config.h"
#include <sys/types.h>
#include <sys/wait.h>


namespace TinyServer
{
static Ref<Logger> logger = TINY_LOG_NAME("system");
static Ref<ConfigVar<uint32_t>> daemon_restart_interval = 
    Config::Lookup("daemon.restart.interval", (uint32_t)5, "daemon restart interval");

std::string ProcessInfo::toString() const
{
    std::stringstream ss;
    ss << "[ProcessInfo parent_id = " << parent_id
       << " main_id = " << main_id
       << " parent_start_timr = " << TimeToStr(parent_start_time)
       << " main_start_time = " <<TimeToStr(main_start_time)
       << " restart_count = " << restart_count << "]";
    return ss.str();
}

static int real_start(int argc, char** argv, std::function<int(int argc, char** argv)> main_cb)
{
    return main_cb(argc, argv);
}

static int real_daemon(int argc, char** argv, std::function<int(int argc, char** argv)> main_cb)
{
    daemon(1, 0);   //守护进程，关闭终端
    ProcessInfoMgr::GetInstance()->parent_id = getpid();
    ProcessInfoMgr::GetInstance()->parent_start_time = time(0);
    while (true)
    {
        pid_t pid = fork();
        if (pid == 0)
        {
            ProcessInfoMgr::GetInstance()->main_id = getpid();
            ProcessInfoMgr::GetInstance()->main_start_time = time(0);
            TINY_LOG_INFO(logger) << "process start pid = " << getpid();
            return real_start(argc, argv, main_cb);
        }
        else if (pid < 0)
        {
            TINY_LOG_ERROR(logger) << "fork fail return = " << pid << " errno = " << errno
                << "errstr = " << strerror(errno);
            return -1;
        }
        else
        {
            int status = 0;
            waitpid(pid, &status, 0);
            if (status)
            {
                 TINY_LOG_ERROR(logger) << "child crash pid = " << pid << " status = " << status;
            }
            else
            {
                TINY_LOG_INFO(logger) << "child finished pid = " << pid;
                break;
            }
            ProcessInfoMgr::GetInstance()->restart_count += 1;
            sleep(daemon_restart_interval->getValue());
        }
    }
    return 0;
}

int start_daemon(int argc, char** argv, std::function<int(int argc, char** argv)> main_cb, bool is_daemon)
{
    if (!is_daemon)
    {
        return real_start(argc, argv, main_cb);
    }
    return real_daemon(argc, argv, main_cb);
}

}