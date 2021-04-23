#pragma once
#include "socket_stream.h"
#include "http/http.h"
#include "uri.h"
#include "thread.h"
#include <memory>

namespace TinyServer
{
namespace http
{
struct HttpResult
{
    HttpResult(int res, Ref<HttpResponse> rsp, std::string er)
        : result(res), response(rsp), error(er) {}
    enum class Error
    {
        OK = 0,
        INVALID_URI = 1,
        INVALID_HOST = 2,
        CREATE_SOCK_ERROR = 3,
        CONNECT_FAIL = 4,
        SEND_CLOSED_BY_PEER = 5,
        SEND_SOCKET_ERROR = 6,
        TIMEOUT = 7,
        POOL_GET_CONNECT = 8,
        POOL_INVALID_CONNECT = 9
    };
    std::string toString() const;
    int result;
    Ref<HttpResponse> response;
    std::string error;
};

class HttpConnection : public SocketStream
{
friend class HttpConnectionPool;
public:
    HttpConnection(Ref<Socket> sock, bool owner = true);
    ~HttpConnection();
    Ref<HttpResponse> recvResponse();
    int sendRequest(Ref<HttpRequest> req);

    static Ref<HttpResult> DoGet( 
                                    const std::string& url, 
                                    uint64_t timeout_ms, 
                                    const std::map<std::string, std::string> header = {}, 
                                    const std::string& body = "");

    static Ref<HttpResult> DoGet( 
                                    Ref<Uri> uri, 
                                    uint64_t timeout_ms, 
                                    const std::map<std::string, std::string> header = {}, 
                                    const std::string& body = "");

    static Ref<HttpResult> DoPost( 
                                    const std::string& url, 
                                    uint64_t timeout_ms, 
                                    const std::map<std::string, std::string> header = {}, 
                                    const std::string& body = "");

    static Ref<HttpResult> DoPost( 
                                    Ref<Uri> uri, 
                                    uint64_t timeout_ms, 
                                    const std::map<std::string, std::string> header = {}, 
                                    const std::string& body = "");

    static Ref<HttpResult> DoRequest(HttpMethod method, 
                                    const std::string& url, 
                                    uint64_t timeout_ms, 
                                    const std::map<std::string, std::string> header = {}, 
                                    const std::string& body = "");
    
    static Ref<HttpResult> DoRequest(HttpMethod method, 
                                    Ref<Uri> uri, 
                                    uint64_t timeout_ms, 
                                    const std::map<std::string, std::string> header = {}, 
                                    const std::string& body = "");
    
    static Ref<HttpResult> DoRequest(Ref<HttpRequest> req, 
                                    Ref<Uri> uri, 
                                    uint64_t timeout_ms);
private:
    uint64_t m_createTime = 0;
    uint64_t m_request = 0;
};

class HttpConnectionPool
{
public:
    typedef MutexLock MutexType;
    HttpConnectionPool(const std::string& host, const std::string& vhost, uint32_t port, uint32_t maxSize,
                        uint32_t maxAliveTime, uint32_t maxRequest);


    Ref<HttpConnection> getConnection();

    Ref<HttpResult> doGet(const std::string& url, 
                          uint64_t timeout_ms, 
                          const std::map<std::string, std::string> header = {}, 
                          const std::string& body = "");

    Ref<HttpResult> doGet(Ref<Uri> uri, 
                          uint64_t timeout_ms, 
                          const std::map<std::string, std::string> header = {}, 
                          const std::string& body = "");

    Ref<HttpResult> doPost(const std::string& url, 
                           uint64_t timeout_ms, 
                           const std::map<std::string, std::string> header = {}, 
                           const std::string& body = "");

    Ref<HttpResult> doPost(Ref<Uri> uri, 
                           uint64_t timeout_ms, 
                           const std::map<std::string, std::string> header = {}, 
                           const std::string& body = "");

    Ref<HttpResult> doRequest(HttpMethod method, 
                              const std::string& url, 
                              uint64_t timeout_ms, 
                              const std::map<std::string, std::string> header = {}, 
                              const std::string& body = "");
    
    Ref<HttpResult> doRequest(HttpMethod method, 
                              Ref<Uri> uri, 
                              uint64_t timeout_ms, 
                              const std::map<std::string, std::string> header = {}, 
                              const std::string& body = "");
    
    Ref<HttpResult> doRequest(Ref<HttpRequest> req, uint64_t timeout_ms);

private:
    static void ReleasePtr(HttpConnection* ptr, HttpConnectionPool* pool);

private:
    std::string m_host;
    std::string m_vhost;
    uint32_t m_port;
    uint32_t m_maxSize;
    uint32_t m_maxAliveTime;
    uint32_t m_maxRequest;

    MutexType m_mutex;
    std::list<HttpConnection*> m_conns;
    std::atomic<uint32_t> m_total = {0};   
};



}
}

