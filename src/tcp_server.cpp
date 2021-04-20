#include "tcp_server.h"
#include "config.h"
#include "log.h"

namespace TinyServer
{
static Ref<Logger> logger = TINY_LOG_NAME("system");

static Ref<ConfigVar<uint64_t>> tcp_server_read_timeout = 
    Config::Lookup("tcp_server.read_timeout", 
    (uint64_t)60 * 1000 * 2, "tcp server read timeout");

TCPServer::TCPServer(IOManager* worker, IOManager* acceprWorker)
    : m_worker(worker), m_acceptWorker(acceprWorker), m_recvTimeout(tcp_server_read_timeout->getValue()), m_name("TinyServer/1.0.0")
    , m_isStop(true)
{

}

TCPServer::~TCPServer()
{
    for (auto& item : m_sockets)
    {
        item->close();
    }
    m_sockets.clear();
}

bool TCPServer::bind(Ref<Address> addr)
{
    std::vector<Ref<Address>> addrs;
    std::vector<Ref<Address>> fails;
    addrs.push_back(addr);
    return bind(addrs, fails);
}

bool TCPServer::bind(std::vector<Ref<Address>>& addrs, std::vector<Ref<Address>>& fails)
{
    for (auto& item : addrs)
    {
        Ref<Socket> sock = Socket::CreateTCP(item);
        if (!sock->bind(item))
        {
            TINY_LOG_ERROR(logger) << "bind fail errno = " << errno << " errstr = " << strerror(errno)
                << " addr = [" << item->toString() << "]";
            fails.push_back(item);
            continue;
        }
        if (!sock->listen())
        {
            TINY_LOG_ERROR(logger) << "listen fail errno = " << errno << " errstr = " << strerror(errno)
                << " addr = [" << item->toString() << "]";
            fails.push_back(item);
            continue;
        }
        m_sockets.push_back(sock);
    }
    if (!fails.empty())
    {
        m_sockets.clear();
        return false;
    }
    for (auto& item : m_sockets)
    {
        TINY_LOG_INFO(logger) <<"server bind success: " << *item;
    }
    return true;
}

void TCPServer::startAccept(Ref<Socket> sock)
{
    while (!m_isStop)
    {
        Ref<Socket> client = sock->accept();
        if (client)
        {
            client->setRecvTimeout(m_recvTimeout);
            m_worker->schedule(std::bind(&TCPServer::handleClient, shared_from_this(), client));
        }
        else
        {
            TINY_LOG_ERROR(logger) << "accept errno = " << errno << "errstr = " << strerror(errno);
        }
    }
}

void TCPServer::handleClient(Ref<Socket> client)
{
    TINY_LOG_INFO(logger) << "handleClient: " << *client;
}

bool TCPServer::start()
{
    if (!m_isStop)
        return true;
    m_isStop = false;
    for (auto& sock : m_sockets)
    {
        m_acceptWorker->schedule(std::bind(&TCPServer::startAccept, shared_from_this(), sock));
    }
    return true;
}

void TCPServer::stop()
{
    m_isStop = true;
    auto self = shared_from_this();
    m_acceptWorker->schedule([this, self](){
        for (auto& sock : self->m_sockets)
        {
            sock->cancelAll();
            sock->close();
        }
        self->m_sockets.clear();
    });
}








}