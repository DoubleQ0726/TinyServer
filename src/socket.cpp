#include <netinet/tcp.h>
#include "socket.h"
#include "fd_manager.h"
#include "macro.h"
#include "hook.h"
#include "iomanager.h"

namespace TinyServer
{
static Ref<Logger> logger = TINY_LOG_NAME("system");

Ref<Socket> Socket::CreateTCP(Ref<Address> address)
{
    Ref<Socket> sock(new Socket(address->getAddr()->sa_family, TCP, 0));
    return sock;
}

Ref<Socket> Socket::CreateUDP(Ref<Address> address)
{
    Ref<Socket> sock(new Socket(address->getAddr()->sa_family, UDP, 0));
    return sock;
}

Ref<Socket> Socket::CreateTCPSocket()
{
    Ref<Socket> sock(new Socket(IPv4, TCP, 0));
    return sock;
}

Ref<Socket> Socket::CreateUDPSocket()
{
    Ref<Socket> sock(new Socket(IPv4, UDP, 0));
    return sock;
}

Ref<Socket> Socket::CreateTCPSocket6()
{
    Ref<Socket> sock(new Socket(IPv6, TCP, 0));
    return sock;
}

Ref<Socket> Socket::CreateUDPSocket6()
{
    Ref<Socket> sock(new Socket(IPv6, UDP, 0));
    return sock;
}

Ref<Socket> Socket::CreateUnixTCPSocket()
{
    Ref<Socket> sock(new Socket(UNIX, TCP, 0));
    return sock;
}

Ref<Socket> Socket::CreateUnixUDPSocket()
{
    Ref<Socket> sock(new Socket(UNIX, UDP, 0));
    return sock;
}

Socket::Socket(int family, int type, int protocol)
    : m_sock(-1), m_family(family), m_type(type), m_protocol(protocol), m_isConnected(false)
{

}

Socket::~Socket()
{
    close();
}

int Socket::getSendTimeout()
{
    Ref<FdCtx> ctx = FdMgr::GetInstance()->get(m_sock);
    if (ctx)
        return ctx->getTimeout(SO_SNDTIMEO);
    return -1;
}

void Socket::setSendTimeout(uint64_t value)
{
    struct timeval tv;
    tv.tv_sec = (int)(value / 1000);
    tv.tv_usec = (int)(value % 1000) * 1000;
    setOption(SOL_SOCKET, SO_SNDTIMEO, tv);
    return;
}

int Socket::getRecvTimeout()
{
    Ref<FdCtx> ctx = FdMgr::GetInstance()->get(m_sock);
    if (ctx)
        return ctx->getTimeout(SO_RCVTIMEO);
    return -1;
}

void Socket::setRecvTimeout(uint64_t value)
{
    struct timeval tv;
    tv.tv_sec = (int)(value / 1000);
    tv.tv_usec = (int)(value % 1000) * 1000;
    setOption(SOL_SOCKET, SO_RCVTIMEO, tv);
    return;
}

bool Socket::getOption(int level, int option, void* result, size_t* len)
{
    int res = getsockopt(m_sock, level, option, result, (socklen_t*)len);
    if (res)
    {
        TINY_LOG_DEBUG(logger) << "getOption sock = " << m_sock << " level = " << level
            << " option = " << option << " errno = " << errno << "errstr = " << strerror(errno);
        return false;
    }
    return true;
}

bool Socket::setOption(int level, int option, const void* result, size_t length)
{
    int  res = setsockopt(m_sock, level, option, result, length);
    if (res)
    {
        TINY_LOG_DEBUG(logger) << "setOption sock = " << m_sock << " level = " << level
            << " option = " << option << " errno = " << errno << " errstr = " << strerror(errno);
        return false;
    }
    return true;
}

Ref<Socket> Socket::accept()
{
    Ref<Socket> sock(new Socket(m_family, m_type, m_protocol));
    int newsock = ::accept(m_sock, nullptr, nullptr);
    if (newsock == -1)
    {
        TINY_LOG_ERROR(logger) << "accept(" << m_sock << ") errno = " << errno << " errstr = " << strerror(errno);
        return nullptr;
    }
    sock->init(newsock);
    return sock;
}

bool Socket::init(int sock)
{
    Ref<FdCtx> ctx = FdMgr::GetInstance()->get(sock);
    if (ctx && ctx->isSocket() && !ctx->isClose())
    {
        m_sock = sock;
        m_isConnected = true;
        getLocalAddress();
        getRemoteAddress();
        return true;
    }
    return false;
}

bool Socket::bind(const Ref<Address> addr)
{
    if (TINY_UNLICKLY(!isValid()))
    {
        newSock();
        if (TINY_UNLICKLY(!isValid()))
            return false;
    }
    if (TINY_UNLICKLY(addr->getFamily() != m_family))
    {
        TINY_LOG_ERROR(logger) << "bind sock.family(" << m_family << ") addr.family(" 
            << addr->getFamily() << ") not equal, addr = " << addr->toString();
        return false;
    }
    if (::bind(m_sock, addr->getAddr(), addr->getAddrLen()))
    {
        TINY_LOG_ERROR(logger) << "bind error errno = " << errno << " strerr = " << strerror(errno);
        return false;
    }
    getLocalAddress();
    return true;
}

bool Socket::connect(const Ref<Address>& addr, uint64_t timeout_ms)
{
    if (!isValid())
    {
        newSock();
        if (TINY_UNLICKLY(!isValid()))
            return false;
    }
    if (TINY_UNLICKLY(addr->getFamily() != m_family))
    {
        TINY_LOG_ERROR(logger) << "connect sock.family(" << m_family << ") addr.family(" 
            << addr->getFamily() << ") not equal, addr = " << addr->toString();
        return false;
    }
    if (timeout_ms == (uint64_t)-1)
    {
        if (::connect(m_sock, addr->getAddr(), addr->getAddrLen()))
        {
            TINY_LOG_ERROR(logger) << "sock = " << m_sock << " connect(" << addr->toString()
                << ") error errno = " << errno << "errstr = " << strerror(errno);
            close();
            return false;
        }
    }
    else
    {
        if (::connect_with_timeout(m_sock, addr->getAddr(), addr->getAddrLen(), timeout_ms))
        {
            TINY_LOG_ERROR(logger) << "sock = " << m_sock << " connect(" << addr->toString()
                << " ) timeout = " << timeout_ms << " error errno = " << errno << "errstr = " << strerror(errno);
            close();
            return false;
        }
    }
    m_isConnected = true;
    getRemoteAddress();
    getLocalAddress();
    return true;
}

bool Socket::listen(int backlog)
{
    if (!isValid())
    {
        TINY_LOG_ERROR(logger) << "listen error sock = -1";
        return false;
    }
    if (::listen(m_sock, backlog))
    {
        TINY_LOG_ERROR(logger) << "listen error errno"  << errno << "errstr = " << strerror(errno);
        //close();
        return false;
    }
    return true;
}

bool Socket::close()
{
    if (!m_isConnected && m_sock == -1)
        return true;
    m_isConnected = false;
    if (m_sock != -1)
    {
        ::close(m_sock);
        m_sock = -1;
    }
    return false;
}

int Socket::send(const void* buffer, size_t length, int flags)
{
    if (isConnected())
        return ::send(m_sock, buffer, length, flags);
    return -1;
}

int Socket::send(const iovec* buffers, size_t length, int flags)
{
    if (isConnected())
    {
        msghdr msg;
        memset(&msg, 0, sizeof(msg));
        msg.msg_iov = (iovec*)buffers;
        msg.msg_iovlen = length;
        return ::sendmsg(m_sock, &msg, flags);
    }
    return -1;
}

int Socket::sendTo(const void* buffer, size_t length, const Ref<Address>& to, int flags)
{
    if (isConnected())
    {
        return ::sendto(m_sock, buffer, length, flags, to->getAddr(), to->getAddrLen());
    }
    return -1;
}

int Socket::sendTo(const iovec* buffers, size_t length, const Ref<Address>& to, int flags)
{
    if (isConnected())
    {
        msghdr msg;
        memset(&msg, 0, sizeof(msg));
        msg.msg_iov = (iovec*)buffers;
        msg.msg_iovlen = length;
        msg.msg_name = to->getAddr();
        msg.msg_namelen = to->getAddrLen();
        return ::sendmsg(m_sock, &msg, flags);
    }
    return -1;
}

int Socket::recv(void* buffer, size_t length, int flags)
{
    if (isConnected())
    {
        return ::recv(m_sock, buffer, length, flags);
    }
    return -1;
}

int Socket::recv(iovec* buffers, size_t length, int flags)
{
    if (isConnected())
    {
        msghdr msg;
        memset(&msg, 0, sizeof(msg));
        msg.msg_iov = (iovec*)buffers;
        msg.msg_iovlen = length;
        return ::recvmsg(m_sock, &msg, flags);
    }
    return -1;
}

int Socket::recvFrom(void* buffer, size_t length, const Ref<Address>& from, int flags)
{
    if (isConnected())
    {
        socklen_t len = from->getAddrLen();
        return ::recvfrom(m_sock, buffer, length, flags, from->getAddr(), &len);
    }
    return -1;
}

int Socket::recvFrom(iovec* buffers, size_t length, const Ref<Address>& from, int flags)
{
    if (isConnected())
    {
        msghdr msg;
        memset(&msg, 0, sizeof(msg));
        msg.msg_iov = (iovec*)buffers;
        msg.msg_iovlen = length;
        msg.msg_name = from->getAddr();
        msg.msg_namelen = from->getAddrLen();
        return ::recvmsg(m_sock, &msg, flags);
    }
    return -1;
}

Ref<Address> Socket::getRemoteAddress()
{
    if (m_remoteAddress)
        return m_remoteAddress;
    Ref<Address> result;
    switch (m_family)
    {
    case AF_INET:
        result.reset(new IPv4Address());
        break;
    case AF_INET6:
        result.reset(new IPv6Address());
        break;
    case AF_UNIX:
        result.reset(new UnixAddress());
        break;
    default:
        result.reset(new UnknowAddress(m_family));
        break;
    }
    socklen_t addrLen = result->getAddrLen();
    if (getpeername(m_sock, result->getAddr(), &addrLen))
    {
        TINY_LOG_ERROR(logger) << "getpeername error sock = " << m_sock << " errno = "
            << errno <<" errstr = " << strerror(errno);
        return Ref<UnknowAddress>(new UnknowAddress(m_family)); 
    }
    if (m_family == AF_UNIX)
    {
        Ref<UnixAddress> addr = std::dynamic_pointer_cast<UnixAddress>(result);
        addr->setAddrLen(addrLen);
    }
    m_remoteAddress = result;
    return m_remoteAddress;
}

Ref<Address> Socket::getLocalAddress()
{
    if (m_localAddress)
        return m_localAddress;
    Ref<Address> result;
    switch (m_family)
    {
    case AF_INET:
        result.reset(new IPv4Address());
        break;
    case AF_INET6:
        result.reset(new IPv6Address());
        break;
    case AF_UNIX:
        result.reset(new UnixAddress());
        break;
    default:
        result.reset(new UnknowAddress(m_family));
        break;
    }
    socklen_t addrLen = result->getAddrLen();
    if (getsockname(m_sock, result->getAddr(), &addrLen))   //通过当前的socket句柄得到其地址和长度信息
    {
        TINY_LOG_ERROR(logger) << "getpeername error sock = " << m_sock << " errno = "
            << errno <<" errstr = " << strerror(errno);
        return Ref<UnknowAddress>(new UnknowAddress(m_family)); 
    }
    if (m_family == AF_UNIX)
    {
        Ref<UnixAddress> addr = std::dynamic_pointer_cast<UnixAddress>(result);    
        addr->setAddrLen(addrLen);
    }
    m_localAddress = result;
    return m_localAddress;
}

bool Socket::isValid() const
{
    return m_sock != -1;
}

int Socket::getError()
{
    int error = 0;
    size_t len = sizeof(error);
    if (!getOption(SOL_SOCKET, SO_ERROR, &error, &len))
    {
        return -1;
    }
    return error;
}


std::ostream& Socket::dump(std::ostream& os) const
{
    os << "[Socket sock = " << m_sock << " is_connected = " << m_isConnected
       << " family = " << m_family << " type = " << m_type
       << " protocol = " << m_protocol;
    if (m_localAddress)
    {
        os << " local_address = " << m_localAddress->toString();
    }
    if (m_remoteAddress)
    {
        os <<" remote_address = " << m_remoteAddress->toString();
    }
    os << "]";
    return os;
}

bool Socket::cancelRead()
{
    return IOManager::GetThis()->cancelEvent(m_sock, IOManager::READ);
}

bool Socket::cancelWrite()
{
    return IOManager::GetThis()->cancelEvent(m_sock, IOManager::WRITE);
}

bool Socket::cancelAccept()
{
    return IOManager::GetThis()->cancelEvent(m_sock, IOManager::READ);
}

bool Socket::cancelAll()
{
    return IOManager::GetThis()->cancelAll(m_sock);
}

void Socket::initSock()
{
    int val = -1;
    setOption(SOL_SOCKET, SO_REUSEADDR, val);
    if (m_type == SOCK_STREAM)
        setOption(IPPROTO_TCP, TCP_NODELAY, val);
}

void Socket::newSock()
{
    m_sock = ::socket(m_family, m_type, m_protocol);
    if (TINY_LICKLY(m_sock != -1))
        initSock();
    else 
    {
        TINY_LOG_ERROR(logger) << "socket(" << m_family << ", " << m_type << ", " << m_protocol
            << ") errno = " << errno << "errstr = " << strerror(errno);
    }
}

std::ostream& operator<<(std::ostream& os, const Socket& sock)
{
    return sock.dump(os);
}

}