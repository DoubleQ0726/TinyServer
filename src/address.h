#pragma once
#include <memory>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include "log.h"
#include <vector>
#include <map>

namespace TinyServer
{
class IPAddress;
class Address
{
public:
    virtual ~Address();
    static Ref<Address> Create(const sockaddr* addr, socklen_t addrlen);
    static bool Lookup(std::vector<Ref<Address>>& result, const std::string& host, int family = AF_INET, int type = 0, int protocol = 0);
    static Ref<Address> LookupAny(const std::string& host, int family = AF_INET, int type = 0, int protocol = 0);
    static Ref<IPAddress> LookupIPAddress(const std::string& host, int family = AF_INET, int type = 0, int protocol = 0);

    static bool GetInterfaceAddress(std::multimap<std::string, std::pair<Ref<Address>, uint32_t>>& result,
        int family = AF_INET);
    
    static bool GetInterfaceAddress(std::vector<std::pair<Ref<Address>, uint32_t>>& result, const std::string& ifcae, int family = AF_INET);

    int getFamily() const;

    virtual sockaddr* getAddr() = 0;
    virtual const sockaddr* getAddr() const = 0;
    virtual const socklen_t getAddrLen() const = 0;

    virtual std::ostream& insert(std::ostream& os) const = 0;
    std::string toString() const;

    bool operator<(const Address& rhs) const;
    bool operator==(const Address& rhs) const;
    bool operator!=(const Address& rhs) const;
};

class IPAddress : public Address
{
public:
    static Ref<IPAddress> Create(const char* address, uint16_t port = 0); 

    virtual Ref<IPAddress> broadcastAddress(uint32_t prefix_len) = 0;
    virtual Ref<IPAddress> netWordAddress(uint32_t prefix_len) = 0;
    virtual Ref<IPAddress> subnetMask(uint32_t prefix_len) = 0;

    virtual uint32_t getPort() const = 0;
    virtual void setPort(uint16_t port) = 0;
};

class IPv4Address : public IPAddress
{
public:
    IPv4Address(uint32_t address = INADDR_ANY, uint16_t port = 0);
    IPv4Address(const sockaddr_in& address);

    static Ref<IPv4Address> Create(const char* address, uint16_t port = 0);

    sockaddr* getAddr() override;
    const sockaddr* getAddr() const override;
    const socklen_t getAddrLen() const override;

    std::ostream& insert(std::ostream& os) const override;

    Ref<IPAddress> broadcastAddress(uint32_t prefix_len) override;
    Ref<IPAddress> netWordAddress(uint32_t prefix_len) override;
    Ref<IPAddress> subnetMask(uint32_t prefix_len) override;

    uint32_t getPort() const override;
    void setPort(uint16_t port) override;

private:
    sockaddr_in m_addr;
};

class IPv6Address : public IPAddress
{
public:
    IPv6Address();
    IPv6Address(const uint8_t address[16], uint16_t port = 0);
    IPv6Address(const sockaddr_in6& address);

    static Ref<IPv6Address> Create(const char* address, uint16_t port = 0);

    sockaddr* getAddr() override;
    const sockaddr* getAddr() const override;
    const socklen_t getAddrLen() const override;

    std::ostream& insert(std::ostream& os) const override;

    Ref<IPAddress> broadcastAddress(uint32_t prefix_len) override;
    Ref<IPAddress> netWordAddress(uint32_t prefix_len) override;
    Ref<IPAddress> subnetMask(uint32_t prefix_len) override;

    uint32_t getPort() const override;
    void setPort(uint16_t port) override;

private:
    sockaddr_in6 m_addr;
};

class UnixAddress : public Address
{
public:
    UnixAddress();
    UnixAddress(const std::string& path);

    sockaddr* getAddr() override;
    const sockaddr* getAddr() const override;
    const socklen_t getAddrLen() const override;
    void setAddrLen(uint32_t value);
    std::ostream& insert(std::ostream& os) const override;

private:
    struct sockaddr_un m_addr;
    socklen_t m_length;
};

class UnknowAddress : public Address
{
public:
    UnknowAddress();
    UnknowAddress(int family);
    UnknowAddress(const sockaddr& address);
    sockaddr* getAddr() override;
    const sockaddr* getAddr() const override;
    const socklen_t getAddrLen() const override;

    std::ostream& insert(std::ostream& os) const override;

private:
    sockaddr m_addr;
};

std::ostream& operator<<(std::ostream& os, const Address& addr);

}