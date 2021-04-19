#pragma once
#include "http.h"
#include "http11_parser.h"
#include "httpclient_parser.h"
#include "log.h"

namespace TinyServer
{
namespace http
{
class HttpRequestParser
{
public:
    HttpRequestParser();

    size_t execute(const char *data, size_t len, size_t off);
    int isFinished() const;
    int hasError() const;

private:
    http_parser m_parser;
    Ref<HttpResponse> m_data;
    int m_error;
};


class HttpResponseParser
{
public:
    HttpResponseParser();

    size_t execute(const char *data, size_t len, size_t off);
    int isFinished() const;
    int hasError() const;

private:
    httpclient_parser m_parser;
    Ref<HttpResponse> m_data;
    int m_error;
};

}
}