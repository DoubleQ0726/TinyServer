#pragma once
#include "log.h"
#include "address.h"
#include "noncoptable.h"

namespace TinyServer
{
class Socket : public std::enable_shared_from_this<Socket>, Noncopyable
{
public:
    enum Type
    {
        TCP = SOCK_STREAM,
        UDP = SOCK_DGRAM
    };

    enum Family
    {
        IPv4 = AF_INET,
        IPv6 = AF_INET6,
        UNIX = AF_UNIX
    };

    static Ref<Socket> CreateTCP(Ref<Address> address);
    static Ref<Socket> CreateUDP(Ref<Address> address);

    static Ref<Socket> CreateTCPSocket();
    static Ref<Socket> CreateUDPSocket();

    static Ref<Socket> CreateTCPSocket6();
    static Ref<Socket> CreateUDPSocket6();

    static Ref<Socket> CreateUnixTCPSocket();
    static Ref<Socket> CreateUnixUDPSocket();

    Socket(int family, int type, int protocol = 0);
    ~Socket();

    int getSendTimeout();
    void setSendTimeout(uint64_t value);

    int getRecvTimeout();
    void setRecvTimeout(uint64_t value);

    bool getOption(int level, int option, void* result, size_t* len);
    template<typename T>
    bool getOption(int level, int option, T& result, size_t length = sizeof(T))
    {
        return getOption(level, option, &result, &length);
    }

    bool setOption(int level, int option, const void* result, size_t length);
    template<typename T>
    bool setOption(int level, int option, const T& result)
    {
        return setOption(level, option, &result, sizeof(T));
    }

    Ref<Socket> accept();

    bool bind(const Ref<Address> addr);
    bool connect(const Ref<Address>& addr, uint64_t timeout_ms = -1);
    bool listen(int backlog = SOMAXCONN);
    bool close();

    int send(const void* buffer, size_t length, int flags = 0);
    int send(const iovec* buffers, size_t length, int flags = 0);
    int sendTo(const void* buffer, size_t length, const Ref<Address>& to, int flags = 0);
    int sendTo(const iovec* buffers, size_t length, const Ref<Address>& to, int flags = 0);

    int recv(void* buffer, size_t length, int flags = 0);
    int recv(iovec* buffers, size_t length, int flags = 0);
    int recvFrom(void* buffer, size_t length, const Ref<Address>& from, int flags = 0);
    int recvFrom(iovec* buffers, size_t length, const Ref<Address>& from, int flags = 0);

    Ref<Address> getRemoteAddress();
    Ref<Address> getLocalAddress();

    int getFamily() const { return m_family; }
    int getType() const { return m_type; }
    int getProtocol() const { return m_protocol; }

    bool isConnected() const { return m_isConnected; }
    bool isValid() const;
    int getError();

    std::ostream& dump(std::ostream& os) const;
    int getSocket() const { return m_sock; }

   bool cancelRead();
   bool cancelWrite();
   bool cancelAccept();
   bool cancelAll();

private:
    bool init(int sock);
    void initSock();
    void newSock();

private:
    int m_sock;

    int m_family;   //协议簇 AF_INTE/AF_INTE6
    int m_type;     //服务类型 数据报/流
    int m_protocol; //默认为0

    bool m_isConnected;

    Ref<Address> m_localAddress;
    Ref<Address> m_remoteAddress;
};





}