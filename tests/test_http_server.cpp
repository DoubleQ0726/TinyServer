#include "http/http_server.h"
using namespace TinyServer;

static Ref<Logger> logger = TINY_LOG_ROOT;

void run()
{
    Ref<http::HttpServer> server(new http::HttpServer);
    Ref<Address> addr = Address::LookupIPAddress("0.0.0.0:8020");
    while (!server->bind(addr))
    {
        sleep(2);
    }
    server->getDispatch()->addServlet("/TinyServer/xx", [](Ref<http::HttpRequest> req, 
    Ref<http::HttpResponse> rsp, Ref<http::HttpSession> session){
        rsp->setBody(req->toString());
        return 0;
    });

    server->getDispatch()->addGlobServlet("/TinyServer/*", [](Ref<http::HttpRequest> req, 
    Ref<http::HttpResponse> rsp, Ref<http::HttpSession> session){
        rsp->setBody("Glob:\r\n" + req->toString());
        return 0;
    });


    server->start();
}

int main()
{
    IOManager iom(2);
    iom.schedule(&run);
    return 0;
}