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

    size_t execute(char *data, size_t len);
    int isFinished();
    int hasError();
    void setError(int v) { m_error = v; }
    uint64_t getContentLength();

    Ref<HttpRequest> getData() const { return m_data; }
    const http_parser& getParser() const { return m_parser; }

public:
    static uint64_t GetHttpRequestBufferSize();
    static uint64_t GetHttpRequestMaxBodyLength();

private:
    http_parser m_parser;
    Ref<HttpRequest> m_data;
    //1000:invalid request method
    //1001:invalid http version
    //1002:invalid http field
    int m_error;
};


class HttpResponseParser
{
public:
    HttpResponseParser();

    size_t execute(char *data, size_t len, bool chunck);
    int isFinished();
    int hasError();
    Ref<HttpResponse> getData() const { return m_data; }
    void setError(int v) { m_error = v; }

    uint64_t getContentLength();
    const httpclient_parser& getClientParser() const { return m_parser; }
public:
    static uint64_t GetHttpResponseBufferSize();
    static uint64_t GetHttpResponseMaxBodyLength();

private:
    httpclient_parser m_parser;
    Ref<HttpResponse> m_data;
    int m_error;
};

}
}