#include "http.h"
#include <iostream>


namespace TinyServer
{
namespace http
{
bool CaseInsensitiveLess::operator()(const std::string& lhs, const std::string& rhs) const
{
    return strcasecmp(lhs.c_str(), rhs.c_str()) < 0;
}

HttpMethod StringToHttpMethod(const std::string& n)
{
#define XX(num, name, string) \
    if (strcmp(name, n.c_str()) == 0) \
    { \
        return HttpMethod::name; \
    } \
    HTTP_METHOD_MAP(XX);
#undef XX
    return HttpMethod::HTTP_METHOD_INVALID;
}

HttpMethod CharsToHttpMethod(const char* n)
{
#define XX(num, name, string) \
    if (strncmp(#string, n, strlen(#string)) == 0) \
    { \
        return HttpMethod::name; \
    }   //此处不要加 \ 否则宏展开会出错
    HTTP_METHOD_MAP(XX);
#undef XX
    return HttpMethod::HTTP_METHOD_INVALID;
}


static const char* s_method_string[] = 
{
#define XX(num, name, string) #string,
    HTTP_METHOD_MAP(XX)
#undef XX
};

const char* HttpMethodToString(const HttpMethod& hm)
{
    uint32_t idx = (uint32_t)hm;
    if (idx >= (sizeof(s_method_string) / sizeof(s_method_string[0])))
        return "<unknow method>";
    return s_method_string[idx];
}

const char* HttpStatusToString(const HttpStatus& hs)
{
    switch (hs)
    {
#define XX(code, name, msg) \
    case HttpStatus::name: \
        return #msg;
    HTTP_STATUS_MAP(XX)
#undef XX
    default:
        return "<unknow status>";
    }
}

HttpRequest::HttpRequest(uint8_t version, bool close)
    : m_method(HttpMethod::GET), m_version(version), m_close(close), m_path("/")
{

}

std::string HttpRequest::getHeader(const std::string& key, const std::string& def) const
{
    auto iter = m_headers.find(key);
    return iter == m_headers.end() ? def : iter->second;
}

std::string HttpRequest::getParma(const std::string& key, const std::string& def) const
{
    auto iter = m_params.find(key);
    return iter == m_params.end() ? def : iter->second;
}

std::string HttpRequest::getCookie(const std::string& key, const std::string& def) const
{
    auto iter = m_cookies.find(key);
    return iter == m_cookies.end() ? def : iter->second;
}

void HttpRequest::setHeader(const std::string& key, const std::string& val)
{
    m_headers[key] = val;
}

void HttpRequest::setParam(const std::string& key, const std::string& val)
{
    m_params[key] = val;
}

void HttpRequest::setCookie(const std::string& key, const std::string& val)
{
    m_cookies[key] = val;
}

void HttpRequest::delHeader(const std::string& key)
{
    m_headers.erase(key);
}

void HttpRequest::delParam(const std::string& key)
{
    m_params.erase(key);
}

void HttpRequest::delCookie(const std::string& key)
{
    m_cookies.erase(key);
}

bool HttpRequest::hasHeader(const std::string& key, std::string* val)
{
    auto iter = m_headers.find(key);
    if (iter == m_headers.end())
    {
        return false;
    }
    else
    {
        *val = iter->second;
    }
    return true;
}

bool HttpRequest::hasParam(const std::string& key, std::string* val)
{
    auto iter = m_params.find(key);
    if (iter == m_params.end())
    {
        return false;
    }
    else
    {
        *val = iter->second;
    }
    return true;
}

bool HttpRequest::hasCookie(const std::string& key, std::string* val)
{
    auto iter = m_cookies.find(key);
    if (iter == m_cookies.end())
    {
        return false;
    }
    else
    {
        *val = iter->second;
    }
    return true;
}

std::ostream& HttpRequest::dump(std::ostream& os) const
{
    //GET /uri HTTP/1.1
    //Host: www.baidu.com
    os << HttpMethodToString(m_method) << " " << m_path << (m_query.empty() ? "" : "?")
       << m_query << (m_fragment.empty() ? "" : "#") << m_fragment
       << " HTTP/" << (uint32_t)(m_version >> 4)  << "." << (uint32_t)(m_version & 0x0F)
       << "\r\n";
    os << "connection: " << (m_close ? "close" : "keep-alive") << "\r\n";
    for (auto& item : m_headers)
    {
        if (strcasecmp(item.first.c_str(), "connection") == 0)
            continue;
        os << item.first << ": " << item.second << "\r\n";
    }
    if (!m_body.empty())
    {
        os << "content-length: " << m_body.size() << "\r\n\r\n" << m_body;
    }
    else
        os << "\r\n";
    return os;
}

std::string HttpRequest::toString() const
{
    std::stringstream ss;
    dump(ss);
    return ss.str();
}

HttpResponse::HttpResponse(uint8_t version, bool close)
    : m_status(HttpStatus::OK), m_version(version), m_close(close)
{

}

std::string HttpResponse::getHeader(const std::string& key, const std::string& def)
{
    auto iter = m_headers.find(key);
    return iter == m_headers.end() ? def : iter->second;
}

void HttpResponse::setHeader(const std::string& key, const std::string& val)
{
    m_headers[key] = val;
}

void HttpResponse::delHeader(const std::string& key)
{
    m_headers.erase(key);
}

std::ostream& HttpResponse::dump(std::ostream& os) const
{
    os << "HTTP/" << ((uint32_t)(m_version >> 4)) << "." << ((uint32_t)(m_version & 0x0f)) << " "
       << (uint32_t)m_status << " " << (m_reason.empty() ? HttpStatusToString(m_status) : m_reason)
       << "\r\n";

    for (auto& item : m_headers)
    {
        if (strcasecmp(item.first.c_str(), "connection") == 0)
        {
            continue;
        }
        os << item.first << ": " << item.second << "\r\n";
    }
    os << "connection: " << (m_close ? "close" : "keep-alive") << "\r\n";

    if (!m_body.empty())
        os << "content-length: " << m_body.size() << "\r\n\r\n" << m_body;
    else
        os << "\r\n";
    return os;
}

std::string HttpResponse::toString() const
{
    std::stringstream ss;
    dump(ss);
    return ss.str();
}

}
}