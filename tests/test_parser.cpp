#include "http/http.h"
#include "http/http_parser.h"
#include "log.h"
using namespace TinyServer;

static Ref<Logger> logger = TINY_LOG_ROOT;

const char test_request_data[] = "GET / HTTP/1.1\r\n"
                                "Host: www.sylar.top\r\n"
                                "Content-Length: 10\r\n\r\n"
                                "1234567890";

//16 21 22 10 = 69
void test()
{
    http::HttpRequestParser parser;
    std::string tmp = test_request_data;
    TINY_LOG_INFO(logger) << sizeof(test_request_data);
    size_t res = parser.execute(&tmp[0], tmp.size());
    TINY_LOG_INFO(logger) << "execute res = " << res << " has_error = " << parser.hasError()
                          << " is_finished = " << parser.isFinished();  
    TINY_LOG_INFO(logger) << parser.getData()->toString();
}

int main()
{
    test();
    return 0;
}