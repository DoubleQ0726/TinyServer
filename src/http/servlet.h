#pragma once
#include <memory>
#include <functional>
#include "http/http.h"
#include "http/http_session.h"
#include "thread.h"
#include <unordered_map>


namespace TinyServer
{
namespace http
{
class Servlet
{
public:
    Servlet(const std::string& name)
        : m_name(name) {}
    virtual ~Servlet() {}

    virtual int32_t handle(Ref<HttpRequest> request, 
        Ref<HttpResponse> response, 
        Ref<HttpSession> session) = 0;

    const std::string& getName() const { return m_name; }
protected:
    std::string m_name;
};

class FunctionServlet : public Servlet
{
public:
    typedef std::function<int32_t(Ref<HttpRequest> request, 
        Ref<HttpResponse> response, 
        Ref<HttpSession> session)> Callback;

    FunctionServlet(Callback cb);
    ~FunctionServlet() {}
    int32_t handle(Ref<HttpRequest> request, 
        Ref<HttpResponse> response, 
        Ref<HttpSession> session) override;
private:
    Callback m_cb;
};

class ServletDispatch : public Servlet
{
public:
    typedef RWLock RWMutexType;
    ServletDispatch();
    int32_t handle(Ref<HttpRequest> request, 
        Ref<HttpResponse> response, 
        Ref<HttpSession> session) override;

    //注册服务端处理对应uri的回调函数
    void addServlet(const std::string& uri, Ref<Servlet> slt);
    void addServlet(const std::string& uri, FunctionServlet::Callback cb);
    void addGlobServlet(const std::string& uri, Ref<Servlet> slt);
    void addGlobServlet(const std::string& uri, FunctionServlet::Callback cb);

    void delServlet(const std::string& uri);
    void delGlobServlet(const std::string& uri);

    Ref<Servlet> getServlet(const std::string& uri);
    Ref<Servlet> getGlobServlet(const std::string& uri);

    Ref<Servlet> getDefault() const { return m_default; }
    void setDefault(Ref<Servlet> v) { m_default = v; }

    Ref<Servlet> getMatchedServlet(const std::string& uri);

private:
    RWMutexType m_mutex;
    //uri(/xxx/xxx) -> servlet
    std::unordered_map<std::string, Ref<Servlet>> m_datas;
    //uri(/xxx/*) ->servlet
    std::vector<std::pair<std::string, Ref<Servlet>>> m_globs;
    //默认servlet，所有路径都没有匹配
    Ref<Servlet> m_default;
};

class NotFountServlet : public Servlet
{
public:
    NotFountServlet();
    int32_t handle(Ref<HttpRequest> request, 
        Ref<HttpResponse> response, 
        Ref<HttpSession> session) override;
};


}
}