#pragma once
#include <memory>
#include <functional>
#include "address.h"
#include "socket.h"
#include "iomanager.h"
#include "noncoptable.h"

namespace TinyServer
{
class TCPServer : public std::enable_shared_from_this<TCPServer>, Noncopyable
{
public:
    TCPServer(IOManager* worker = IOManager::GetThis(), IOManager* acceprWorker = IOManager::GetThis());
    ~TCPServer();

    virtual bool bind(Ref<Address> addr);
    virtual bool bind(std::vector<Ref<Address>>& addrs, std::vector<Ref<Address>>& fails);
    virtual bool start();
    virtual void stop();

    uint64_t getRecvTimeout() const { return m_recvTimeout; }
    std::string getName() const { return m_name; }
    void setRecvTimeout(uint64_t v) { m_recvTimeout = v; }
    void setName(const std::string& name) { m_name = name; }
    bool isStop() const { return m_isStop; }

protected:
    virtual void handleClient(Ref<Socket> client);
    virtual void startAccept(Ref<Socket> sock);

private:
    std::vector<Ref<Socket>> m_sockets;
    IOManager* m_worker;    //处理accept状态之后的socket，主要是socket上的读写事件(handleClient)
    IOManager* m_acceptWorker;  //处理accept状态之前的已bind的socket操作(startAccept)
    uint64_t m_recvTimeout;
    std::string m_name;
    bool m_isStop;
};





}