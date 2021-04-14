#pragma once
#include "thread.h"
#include "log.h"
#include "Singleton.h"

namespace TinyServer
{

class FdCtx : public std::enable_shared_from_this<FdCtx>
{
public:
    FdCtx(int fd);
    ~FdCtx();

    bool init();
    bool isInit() const { return m_isInit; }
    bool isSocket() const { return m_isSocket; }
    bool isClose() const { return m_isClose; }

    bool close();

    void setUserNonblock(bool v) { m_userNonblock = v; }
    bool getUserNonblock() { return m_userNonblock; }

    void setSysNonblock(bool v) { m_sysNonblock = v; }
    bool getSysNoneblock() { return m_sysNonblock; }

    void setTimeout(int type, uint64_t v);
    uint64_t getTimeout(int type);

private:
    bool m_isInit;
    bool m_isSocket;
    bool m_sysNonblock;
    bool m_userNonblock;
    bool m_isClose;
    int m_fd;
    uint64_t m_recvTimeout;
    uint64_t m_sendTimeout;
};

class FdManager
{
public:
    typedef RWLock RWMutexType;
    FdManager();

    Ref<FdCtx> get(int fd, bool auto_create = false);
    void del(int fd);

private:
    RWMutexType m_mutex;
    std::vector<Ref<FdCtx>> m_datas;
};

typedef Singleton<FdManager> FdMgr;

}