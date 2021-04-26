#include "http_server.h"
#include "log.h"

namespace TinyServer
{
namespace http
{
static Ref<Logger> logger = TINY_LOG_NAME("system");

HttpServer::HttpServer(bool keepalive, IOManager* worker, IOManager* acceptWorker)
    : TCPServer(worker, acceptWorker), m_isKeepalive(keepalive)
{
    m_dispatch.reset(new ServletDispatch());
}

void HttpServer::handleClient(Ref<Socket> client)
{
    Ref<HttpSession> session(new HttpSession(client));
    do
    {
        auto req = session->recvRequest();
        if (!req)
        {
            TINY_LOG_WARN(logger) << "recv http request fail, errno = " << errno
                << " errstr = " << strerror(errno) << " client: " << *client;
            break;
        }
        Ref<HttpResponse> rsp(new HttpResponse(req->getVersion(), req->isClose() || !m_isKeepalive));
        rsp->setHeader("Server", getName());
        m_dispatch->handle(req, rsp, session);
        session->sendResponse(rsp);
        
        if(!m_isKeepalive || req->isClose()) 
        {
            break;
        }
    } while (true);
    
}


}
}