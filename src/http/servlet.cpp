#include "http/servlet.h"
#include <fnmatch.h>

namespace TinyServer
{
namespace http
{

FunctionServlet::FunctionServlet(Callback cb)
    : Servlet("FunctionServlet"), m_cb(cb)
{

}

int32_t FunctionServlet::handle(Ref<HttpRequest> request, Ref<HttpResponse> response, Ref<HttpSession> session)
{
    return m_cb(request, response, session);
}
        
ServletDispatch::ServletDispatch()
    : Servlet("ServletDispatch")
{
    m_default.reset((new NotFountServlet()));
}

int32_t ServletDispatch::handle(Ref<HttpRequest> request, Ref<HttpResponse> response, Ref<HttpSession> session)
{
    auto slt = getMatchedServlet(request->getPath());
    if (slt)
    {
        slt->handle(request, response, session);
    }
    return 0;
}

void ServletDispatch::addServlet(const std::string& uri, Ref<Servlet> slt)
{
    RWMutexType::WriteLockGuard lock(m_mutex);
    m_datas[uri] = slt;
}

void ServletDispatch::addServlet(const std::string& uri, FunctionServlet::Callback cb)
{
    RWMutexType::WriteLockGuard lock(m_mutex);
    m_datas[uri].reset(new FunctionServlet(cb));
}

void ServletDispatch::addGlobServlet(const std::string& uri, Ref<Servlet> slt)
{
    RWMutexType::WriteLockGuard lock(m_mutex);
    for (auto iter = m_globs.begin(); iter != m_globs.end(); ++iter)
    {
        if (iter->first == uri)
        {
            m_globs.erase(iter);
            break;
        }
    }
    m_globs.push_back(std::make_pair(uri, slt));
}

void ServletDispatch::addGlobServlet(const std::string& uri, FunctionServlet::Callback cb)
{
    addGlobServlet(uri, Ref<FunctionServlet>(new FunctionServlet(cb)));
}

void ServletDispatch::delServlet(const std::string& uri)
{
    RWMutexType::WriteLockGuard lock(m_mutex);
    m_datas.erase(uri);
}

void ServletDispatch::delGlobServlet(const std::string& uri)
{
    RWMutexType::WriteLockGuard lock(m_mutex);
    for (auto iter = m_globs.begin(); iter != m_globs.end(); ++iter)
    {
        if (iter->first == uri)
        {
            m_globs.erase(iter);
            break;
        }
    }
}

Ref<Servlet> ServletDispatch::getServlet(const std::string& uri)
{
    RWMutexType::ReadLockGuard lock(m_mutex);
    auto iter = m_datas.find(uri);
    return iter == m_datas.end() ? nullptr : iter->second;
}

Ref<Servlet> ServletDispatch::getGlobServlet(const std::string& uri)
{
    RWMutexType::ReadLockGuard lock(m_mutex);
    for (auto iter = m_globs.begin(); iter != m_globs.end(); ++iter)
    {
        if (iter->first == uri)
            return iter->second;
    }
    return nullptr;
}

Ref<Servlet> ServletDispatch::getMatchedServlet(const std::string& uri)
{
    RWMutexType::ReadLockGuard lock(m_mutex);
    auto iter = m_datas.find(uri);
    if (iter != m_datas.end())
        return iter->second;
    for (auto iter = m_globs.begin(); iter != m_globs.end(); ++iter)
    {
        if (!fnmatch(iter->first.c_str(), uri.c_str(), 0))
            return iter->second;
    }
    return m_default;
}

NotFountServlet::NotFountServlet()
    : Servlet("NotFountServlet")
{

}

int32_t NotFountServlet::handle(Ref<HttpRequest> request, Ref<HttpResponse> response, Ref<HttpSession> session)
{
    static const std::string& RSP_BODY = "<html><head><title>404 Not Found"
        "</title></head><body><center><h1>404 Not Found</h1></center>"
        "<hr><center>TinyServer/1.0.0</center></body></html>";
    response->setStatus(HttpStatus::NOT_FOUND);
    response->setHeader("Server", "TinyServer/1.0.0");
    response->setHeader("Content-Type", "text/html");
    response->setBody(RSP_BODY);
    return 0;
}

}
}