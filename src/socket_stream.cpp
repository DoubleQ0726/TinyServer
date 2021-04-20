#include "socket_stream.h"

namespace TinyServer
{

SocketStream::SocketStream(Ref<Socket> sock, bool owner)
    : m_socket(sock), m_owner(owner)
{

}

SocketStream::~SocketStream()
{
    if (m_owner && m_socket)
    {
        m_socket->close();
    }
}

bool SocketStream::isConnected()
{
    return m_socket->isConnected();
}

int SocketStream::read(void* buffer, size_t length)
{
    if (!isConnected())
        return -1;
    return m_socket->recv(buffer, length);
}

int SocketStream::read(Ref<ByteArray> ba, size_t length)
{
    if (!isConnected())
        return -1;
    std::vector<iovec> iovs;
    ba->getWriteBuffers(iovs, length);
    int res = m_socket->recv(&iovs[0], iovs.size());
    if (res > 0)
    {
       ba->setPosition(ba->getPosition() + res); 
    }
    return res;
}

int SocketStream::write(const void* buffer, size_t length)
{
    if (!isConnected())
        return -1;
    return m_socket->send(buffer, length);
}

int SocketStream::write(Ref<ByteArray> ba, size_t length)
{
    if (!isConnected())
        return -1;
    std::vector<iovec> iovs;
    ba->getReadBuffers(iovs, iovs.size());
    int res = m_socket->send(&iovs[0], iovs.size());
    if (res > 0)
    {
        ba->setPosition(ba->getPosition() + res);
    }
    return res;
}

void SocketStream::close()
{
    if (m_socket)
        m_socket->close();
}











}