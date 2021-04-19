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
void test_request()
{
    http::HttpRequestParser parser;
    std::string tmp = test_request_data;
    size_t res = parser.execute(&tmp[0], tmp.size());
    TINY_LOG_INFO(logger) << "execute res = " << res << " has_error = " << parser.hasError()
                          << " is_finished = " << parser.isFinished() << " total = " 
                          << tmp.size() << " content-length = " << parser.getContentLength();  
    tmp.resize(tmp.size() - res);
    TINY_LOG_INFO(logger) << parser.getData()->toString();
    TINY_LOG_INFO(logger) << tmp;
}

const char test_response_data[] = "HTTP/1.1 200 OK\r\n"
        "Date: Tue, 04 Jun 2019 15:43:56 GMT\r\n"
        "Server: Apache\r\n"
        "Last-Modified: Tue, 12 Jan 2010 13:48:00 GMT\r\n"
        "ETag: \"51-47cf7e6ee8400\"\r\n"
        "Accept-Ranges: bytes\r\n"
        "Content-Length: 81\r\n"
        "Cache-Control: max-age=86400\r\n"
        "Expires: Wed, 05 Jun 2019 15:43:56 GMT\r\n"
        "Connection: Close\r\n"
        "Content-Type: text/html\r\n\r\n"
        "<html>\r\n"
        "<meta http-equiv=\"refresh\" content=\"0;url=http://www.baidu.com/\">\r\n"
        "</html>\r\n";

void test_response()
{
    http::HttpResponseParser parser;
    std::string tmp = test_response_data;
    size_t res = parser.execute(&tmp[0], tmp.size());
     TINY_LOG_INFO(logger) << "execute res = " << res << " has_error = " << parser.hasError()
                           << " is_finished = " << parser.isFinished() << " total = " 
                           << tmp.size() << " content-length = " << parser.getContentLength()
                           << " tmp[res] = " << tmp[res];
    tmp.resize(tmp.size() - res);
    TINY_LOG_INFO(logger) << parser.getData()->toString();
    TINY_LOG_INFO(logger) << tmp;
}

int main()
{
    test_request();
    TINY_LOG_INFO(logger) << "----------------------";
    test_response();
    return 0;
}