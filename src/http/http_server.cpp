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
        m_dispatch->handle(req, rsp, session);
        //rsp->setBody("hello sylar");
        //TINY_LOG_INFO(logger) << "req = \n" << *req;
        //TINY_LOG_INFO(logger) << "rsp = \n" << *rsp;
        session->sendResponse(rsp);
    } while (m_isKeepalive);
    
}


}
}