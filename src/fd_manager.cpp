#include "fd_manager.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>

namespace TinyServer
{

FdCtx::FdCtx(int fd)
    : m_isInit(false), m_isSocket(false), m_sysNonblock(false),
      m_userNonblock(false), m_isClose(false), m_fd(fd),
      m_recvTimeout(-1), m_sendTimeout(-1)
{
    init();
}

FdCtx::~FdCtx()
{

}

bool FdCtx::init()
{
    if (m_isInit)
        return true;
    m_recvTimeout = -1;
    m_sendTimeout = -1;
    struct stat fd_stat;
    if (-1 == fstat(m_fd, &fd_stat))
    {
        m_isInit = false;
    }
    else
    {
        m_isInit = true;
        m_isSocket = S_ISSOCK(fd_stat.st_mode);
    }
    if (m_isSocket)
    {
        int flag = fcntl(m_fd, F_GETFL, 0);
        if (!(flag & O_NONBLOCK))
        {
            fcntl(m_fd, F_SETFL, O_NONBLOCK);
        }
        m_sysNonblock = true;
    }
    else
    {
        m_sysNonblock = false;
    }
    m_userNonblock = false;
    m_isClose = false;
    return m_isInit;
}

void FdCtx::setTimeout(int type, uint64_t v)
{
    if (type == SO_RCVTIMEO)
        m_recvTimeout = v;
    else
        m_sendTimeout = v;
}

uint64_t FdCtx::getTimeout(int type)
{
    if (type == SO_RCVTIMEO)
        return m_recvTimeout;
    else
        return m_sendTimeout;
}

FdManager::FdManager()
{
    m_datas.resize(64);
}

Ref<FdCtx> FdManager::get(int fd, bool auto_create)
{
    RWMutexType::ReadLockGuard lock(m_mutex);
    if ((int)m_datas.size() <= fd)
    {
        if (!auto_create)
            return nullptr;
    }
    else
    {
        if (m_datas[fd] || !auto_create)
            return m_datas[fd];
    }
    lock.unlock();

    RWMutexType::WriteLockGuard lock2(m_mutex);
    Ref<FdCtx> ctx(new FdCtx(fd));
    m_datas[fd] = ctx;
    return ctx;
}

void FdManager::del(int fd)
{
    RWMutexType::WriteLockGuard lock(m_mutex);
    if ((int)m_datas.size() < fd)
        return;
    m_datas[fd].reset();
}


}