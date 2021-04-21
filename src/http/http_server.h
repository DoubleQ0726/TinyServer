#pragma once
#include "tcp_server.h"
#include "http/http_session.h"
#include "http/servlet.h"

namespace TinyServer
{
namespace http
{
class HttpServer : public TCPServer
{
public:
    HttpServer(bool keepalive = false, IOManager* worker = IOManager::GetThis(), 
        IOManager* acceptWorker = IOManager::GetThis());
    void handleClient(Ref<Socket> client) override;

    Ref<ServletDispatch> getDispatch() const { return m_dispatch; }
    void setDispatch(Ref<ServletDispatch> v) { m_dispatch = v; }
private:
    bool m_isKeepalive;
    Ref<ServletDispatch> m_dispatch;
};

}
}