#pragma once
#include "stream.h"
#include "socket.h"

namespace TinyServer
{

class SocketStream : public Stream
{
public:
    SocketStream(Ref<Socket> sock, bool owner = true);
    ~SocketStream();

    int read(void* buffer, size_t length) override;

    int read(Ref<ByteArray> ba, size_t length) override;

    int write(const void* buffer, size_t length) override;

    int write(Ref<ByteArray> ba, size_t length) override;

    void close() override;

    Ref<Socket> getSocket() const { return m_socket; }

    bool isConnected();

private:
    Ref<Socket> m_socket;
    bool m_owner;
};









}
