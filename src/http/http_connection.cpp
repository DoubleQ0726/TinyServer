#include "http_connection.h"
#include "http/http_parser.h"
#include "log.h"
#include <functional>

namespace TinyServer
{
namespace http
{
static Ref<Logger> logger = TINY_LOG_NAME("system");

std::string HttpResult::toString() const
{
    std::stringstream ss;
    ss << "[HttpResult result = " << result << " error = " 
       << error << " response = " << (response ? response->toString() : "nullptr") << "]";
    return ss.str();
}

HttpConnection::HttpConnection(Ref<Socket> sock, bool owner)
    : SocketStream(sock, owner)
{
}

HttpConnection::~HttpConnection()
{
    TINY_LOG_DEBUG(logger) << "HttpConnection::~HttpConnection";
}

Ref<HttpResponse> HttpConnection::recvResponse()
{
    Ref<HttpResponseParser> parser(new HttpResponseParser);
    uint64_t buffer_size = HttpResponseParser::GetHttpResponseBufferSize();
    std::shared_ptr<char> buffers(new char[buffer_size + 1], [](char* ptr){
        delete[] ptr;
    });
    char* data = buffers.get();
    size_t offset = 0;
    do
    {
        int len = read(data + offset, buffer_size - offset);
        if (len <= 0)
        {
            close();
            return nullptr;
        }
        len += offset;
        data[len] = '\0';
        size_t nparser = parser->execute(data, len, false);
        if (parser->hasError())
            return nullptr;
        offset = len - nparser;
        if (offset == buffer_size)
            return nullptr;
        if (parser->isFinished())
        {
            break;
        }
    } while (true);
    //此处必须为引用，赋值逻辑上错误
    auto& client_parser = parser->getClientParser();
    if (client_parser.chunked)
    {
        std::string body;
        int len = offset;
        do
        {
            do
            {
                int res = read(data + len, buffer_size - len);
                if (res <= 0)
                {
                    close();
                    return nullptr;
                }
                len += res;
                data[len] = '\0';
                size_t nparse = parser->execute(data, len, true);
                if (parser->hasError())
                    return nullptr;
                len -= nparse;
                if (len == (int)buffer_size)
                    return nullptr;
            } while (!parser->isFinished());
            len -= 2;
            if (client_parser.content_len <= len)
            {
                body.append(data, client_parser.content_len);
                memmove(data, data + client_parser.content_len, len - client_parser.content_len);
                len -= client_parser.content_len;
            }
            else
            {
                body.append(data, len);
                int left = client_parser.content_len - len;
                while (left > 0)
                {
                    int res = read(data, left > (int)buffer_size ? (int)buffer_size : left);
                    if (res <= 0)
                    {
                        close();
                        return nullptr;
                    }
                    body.append(data, res);
                    left -= res;
                }
                len = 0;
            }
        } while (!client_parser.chunks_done);  
        parser->getData()->setBody(body);
    }
    else
    {
        int length = parser->getContentLength();
        if (length > 0)
        {
            std::string body;
            body.resize(length);
            int len = 0;
            if (length >= (int)offset)
            {
                memcpy(&body[0], data, offset);
                len = offset;
            }
            else
            {
                memcpy(&body[0], data, length);
                len = length;
            }
            length -= offset;
            if (length > 0)
            {
                if (readFixSize(&body[len], length) <= 0)
                {
                    close();
                    return nullptr;
                }
            }
            parser->getData()->setBody(body);
        }
    }
    
    return parser->getData();
}

int HttpConnection::sendRequest(Ref<HttpRequest> req)
{
    std::stringstream ss;
    ss << *req;
    std::string data = ss.str();
    return writeFixSize(data.c_str(), data.size());
}


Ref<HttpResult> HttpConnection::DoGet( 
                                const std::string& url, 
                                uint64_t timeout_ms, 
                                const std::map<std::string, std::string> header, 
                                const std::string& body)
{
    Ref<Uri> uri = Uri::Create(url);
    if (!uri)
    {
        return std::make_shared<HttpResult>((int)HttpResult::Error::INVALID_URI, nullptr, "invalid url: " + url);
    }
    return DoGet(uri, timeout_ms, header, body);
}

Ref<HttpResult> HttpConnection::DoGet(
                                Ref<Uri> uri, 
                                uint64_t timeout_ms, 
                                const std::map<std::string, std::string> header, 
                                const std::string& body)
{
    return DoRequest(HttpMethod::GET, uri, timeout_ms, header, body);
}   

Ref<HttpResult> HttpConnection::DoPost(
                                const std::string& url, 
                                uint64_t timeout_ms, 
                                const std::map<std::string, std::string> header, 
                                const std::string& body)
{
    Ref<Uri> uri = Uri::Create(url);
    if (!uri)
    {
        return std::make_shared<HttpResult>((int)HttpResult::Error::INVALID_URI, nullptr, "invalid url: " + url);
    }
    return DoPost(uri, timeout_ms, header, body);
}   

Ref<HttpResult> HttpConnection::DoPost( 
                                Ref<Uri> uri, 
                                uint64_t timeout_ms, 
                                const std::map<std::string, std::string> header, 
                                const std::string& body)
{
    return DoRequest(HttpMethod::POST, uri, timeout_ms, header, body);
}   

Ref<HttpResult> HttpConnection::DoRequest(HttpMethod method, 
                                const std::string& url, 
                                uint64_t timeout_ms, 
                                const std::map<std::string, std::string> header, 
                                const std::string& body)
{
    Ref<Uri> uri = Uri::Create(url);
    if (!uri)
    {
        return std::make_shared<HttpResult>((int)HttpResult::Error::INVALID_URI, nullptr, "invalid url: " + url);
    }
    return DoRequest(method, uri, timeout_ms, header, body);
}                                

Ref<HttpResult> HttpConnection::DoRequest(HttpMethod method, 
                                Ref<Uri> uri, 
                                uint64_t timeout_ms, 
                                const std::map<std::string, std::string> header, 
                                const std::string& body)
{
    Ref<HttpRequest> req = std::make_shared<HttpRequest>();
    req->setPath(uri->getPath());
    req->setQuery(uri->getQuery());
    req->setFragment(uri->getFragment());
    req->setMethod(method);
    bool has_host = false;
    for (auto& item : header)
    {
        if (strcasecmp(item.first.c_str(), "connection") == 0)
        {
            if (strcasecmp(item.second.c_str(), "keep-alive") == 0)
            {
                req->setClose(false);
            }
            continue;
        }
        if (!has_host && strcasecmp(item.first.c_str(), "host") == 0)
        {
            has_host = !item.second.empty();
        }
        req->setHeader(item.first, item.second);
    }
    if (!has_host)
    {
        req->setHeader("Host", uri->getHost());
    }
    req->setBody(body);
    return DoRequest(req, uri, timeout_ms);
}

Ref<HttpResult> HttpConnection::DoRequest(Ref<HttpRequest> req, 
                                Ref<Uri> uri, 
                                uint64_t timeout_ms)
{
    Ref<Address> addr = uri->createAddress();
    if (!addr)
    {
        return std::make_shared<HttpResult>((int)HttpResult::Error::INVALID_HOST, nullptr, 
            "invalid host: " + uri->getHost());
    }
    Ref<Socket> sock = Socket::CreateTCP(addr);
    if (!sock)
    {
        return std::make_shared<HttpResult>((int)HttpResult::Error::CREATE_SOCK_ERROR, nullptr, 
            "connect fail: " + addr->toString() + " errno = " + std::to_string(errno) + " errstr" + strerror(errno));
    }
    if (!sock->connect(addr))
    {
        return std::make_shared<HttpResult>((int)HttpResult::Error::CONNECT_FAIL, nullptr, 
            "connect fail: " + addr->toString());
    }
    sock->setRecvTimeout(timeout_ms);
    Ref<HttpConnection> conn = std::make_shared<HttpConnection>(sock);
    int res = conn->sendRequest(req);
    if (res == 0)
    {
        return std::make_shared<HttpResult>((int)HttpResult::Error::SEND_CLOSED_BY_PEER, nullptr, 
            "send closed by peer: " + addr->toString());
    }
    if (res < 0)
    {
        return std::make_shared<HttpResult>((int)HttpResult::Error::SEND_SOCKET_ERROR, nullptr, 
            "send request socket error errno = " + std::to_string(errno) + " errstr = " + strerror(errno));
    }
    auto rsp = conn->recvResponse();
    if (!rsp)
    {
        return std::make_shared<HttpResult>((int)HttpResult::Error::TIMEOUT, nullptr, 
            "recv response timeout: " + addr->toString() + " timeout_ms = " + std::to_string(timeout_ms));
    }
    return std::make_shared<HttpResult>((int)HttpResult::Error::OK, rsp, "ok");
}


HttpConnectionPool::HttpConnectionPool(const std::string& host, const std::string& vhost, 
                                        uint32_t port, uint32_t maxSize, 
                                        uint32_t maxAliveTime, uint32_t maxRequest)
    : m_host(host), m_vhost(vhost), m_port(port), m_maxSize(maxSize), m_maxAliveTime(maxAliveTime), m_maxRequest(maxRequest)                                    
{

}


Ref<HttpConnection> HttpConnectionPool::getConnection()
{
    uint64_t currentMS = GetCurrentMs();
    std::vector<HttpConnection*> invalid_conns;
    HttpConnection* ptr = nullptr;
    MutexType::MutexLockGuard lock(m_mutex);
    while (!m_conns.empty())
    {
        auto conn = *m_conns.begin();
        m_conns.pop_front();
        if (!conn->isConnected())
        {
            invalid_conns.push_back(conn);
            continue;
        }
        if (conn->m_createTime >= currentMS)
        {
            invalid_conns.push_back(conn);
            continue;
        }
        ptr = conn;
        break;
    }
    lock.unlock();
    for (auto& item : invalid_conns)
    {
        delete item;
    }
    m_total -= invalid_conns.size();
    if (!ptr)
    {
        Ref<IPAddress> addr = Address::LookupIPAddress(m_host);
        if (!addr)
        {
            TINY_LOG_ERROR(logger) << "get addr fail: " << m_host;
            return nullptr;
        }
        addr->setPort(m_port);
        Ref<Socket> sock = Socket::CreateTCP(addr);
        if (!sock)
        {
            TINY_LOG_ERROR(logger) << "create socket fail: " << *addr;
            return nullptr;
        }
        if (!sock->connect(addr))
        {
            TINY_LOG_ERROR(logger) << "sock connect fail" << *addr;
            return nullptr;
        }
        ptr = new HttpConnection(sock);
        ++m_total;
    }
    return Ref<HttpConnection>(ptr, std::bind(&HttpConnectionPool::ReleasePtr, std::placeholders::_1, this));
}

void HttpConnectionPool::ReleasePtr(HttpConnection* ptr, HttpConnectionPool* pool)
{
    ++ptr->m_request;
    //Bug: 最大存活时间无效
    if (!ptr->isConnected() || ((ptr->m_createTime + pool->m_maxAliveTime) >= GetCurrentMs())
        || ptr->m_request > pool->m_maxRequest)
    {
        delete ptr;
        --pool->m_total;
        return;
    }
    MutexType::MutexLockGuard lock(pool->m_mutex);
    pool->m_conns.push_back(ptr);
}

Ref<HttpResult> HttpConnectionPool::doGet(const std::string& url, 
                                          uint64_t timeout_ms, 
                                          const std::map<std::string, std::string> header, 
                                          const std::string& body)
{
    return doRequest(HttpMethod::GET, url, timeout_ms, header, body);
}                                          

Ref<HttpResult> HttpConnectionPool::doGet(Ref<Uri> uri, 
                                          uint64_t timeout_ms, 
                                          const std::map<std::string, std::string> header, 
                                          const std::string& body)
{
    std::stringstream ss;
    ss << uri->getPath()
       << (uri->getQuery().empty() ? "" : "?")
       << uri->getQuery()
       << (uri->getFragment().empty() ? "" : "#")
       << uri->getFragment();
    return doGet(ss.str(), timeout_ms, header, body);
}                                          

Ref<HttpResult> HttpConnectionPool::doPost(const std::string& url, 
                                           uint64_t timeout_ms, 
                                           const std::map<std::string, std::string> header, 
                                           const std::string& body)
{
    return doRequest(HttpMethod::POST, url, timeout_ms, header, body);
}                                           

Ref<HttpResult> HttpConnectionPool::doPost(Ref<Uri> uri, 
                                           uint64_t timeout_ms, 
                                           const std::map<std::string, std::string> header, 
                                           const std::string& body)
{
    std::stringstream ss;
    ss << uri->getPath()
       << (uri->getQuery().empty() ? "" : "?")
       << uri->getQuery()
       << (uri->getFragment().empty() ? "" : "#")
       << uri->getFragment();
    return doPost(ss.str(), timeout_ms, header, body);
}                                           

Ref<HttpResult> HttpConnectionPool::doRequest(HttpMethod method, 
                                              const std::string& url, 
                                              uint64_t timeout_ms, 
                                              const std::map<std::string, std::string> header, 
                                              const std::string& body)
{
    Ref<HttpRequest> req = std::make_shared<HttpRequest>();
    req->setPath(url);
    req->setMethod(method);
    req->setClose(false);
    bool has_host = false;
    for (auto& item : header)
    {
        if (strcasecmp(item.first.c_str(), "connection") == 0)
        {
            if (strcasecmp(item.second.c_str(), "keep-alive") == 0)
            {
                req->setClose(false);
            }
            continue;
        }
        if (!has_host && strcasecmp(item.first.c_str(), "host") == 0)
        {
            has_host = !item.second.empty();
        }
        req->setHeader(item.first, item.second);
    }
    if (!has_host)
    {
        if (m_vhost.empty())
            req->setHeader("Host", m_host);
        else
            req->setHeader("Host", m_vhost);
    }
    req->setBody(body);
    return doRequest(req, timeout_ms);
}                                              
    
Ref<HttpResult> HttpConnectionPool::doRequest(HttpMethod method, 
                                              Ref<Uri> uri, 
                                              uint64_t timeout_ms, 
                                              const std::map<std::string, std::string> header, 
                                              const std::string& body)
{
    std::stringstream ss;
    ss << uri->getPath()
       << (uri->getQuery().empty() ? "" : "?")
       << uri->getQuery()
       << (uri->getFragment().empty() ? "" : "#")
       << uri->getFragment();
    return doRequest(method, ss.str(), timeout_ms, header, body);
}                                              
    
Ref<HttpResult> HttpConnectionPool::doRequest(Ref<HttpRequest> req, uint64_t timeout_ms)
{
    auto conn = getConnection();
    if (!conn)
    {
        return std::make_shared<HttpResult>((int)HttpResult::Error::POOL_GET_CONNECT, nullptr, 
            "pool host: " + m_host + " port = " + std::to_string(m_port));
    }
    auto sock = conn->getSocket();
    if (!sock)
    {
        return std::make_shared<HttpResult>((int)HttpResult::Error::POOL_INVALID_CONNECT, nullptr, 
            "pool host: " + m_host + " port = " + std::to_string(m_port));
    }
    int res = conn->sendRequest(req);
    if (res == 0)
    {
        return std::make_shared<HttpResult>((int)HttpResult::Error::SEND_CLOSED_BY_PEER, nullptr, 
            "send closed by peer: " + sock->getRemoteAddress()->toString());
    }
    if (res < 0)
    {
        return std::make_shared<HttpResult>((int)HttpResult::Error::SEND_SOCKET_ERROR, nullptr, 
            "send request socket error errno = " + std::to_string(errno) + " errstr = " + strerror(errno));
    }
    auto rsp = conn->recvResponse();
    if (!rsp)
    {
        return std::make_shared<HttpResult>((int)HttpResult::Error::TIMEOUT, nullptr, 
            "recv response timeout: " + sock->getRemoteAddress()->toString() + " timeout_ms = " + std::to_string(timeout_ms));   
    }
    return std::make_shared<HttpResult>((int)HttpResult::Error::OK, rsp, "ok");
}                                              


}
}