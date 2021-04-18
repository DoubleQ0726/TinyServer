#include "http/http.h"
#include "log.h"
using namespace TinyServer;

void test_request()
{
    Ref<http::HttpRequest> req(new http::HttpRequest);
    req->setHeader("host", "www.sylar.top");
    req->setBody("hello sylar");
    req->dump(std::cout) << std::endl;
}

void test_response()
{
    Ref<http::HttpResponse> res(new http::HttpResponse);
    res->setHeader("X-X", "sylar");
    res->setBody("hello sylar");
    res->setStatus(http::HttpStatus(400));
    res->setClose(false);
    res->dump(std::cout) << std::endl;
}

int main()
{
    test_request();
    test_response();
    return 0;
}