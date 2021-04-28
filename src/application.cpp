#include "application.h"
#include "log.h"
#include "config.h"
#include "daemon.h"

namespace TinyServer
{
static Ref<Logger> logger = TINY_LOG_NAME("system");

static Ref<ConfigVar<std::string>> server_work_path = Config::Lookup("server.work.path", 
    std::string("/home/szq/Work/TinyServer"), "server work path");

static Ref<ConfigVar<std::string>> server_pid_file = Config::Lookup("server.pid.file", 
    std::string("TinyServer.pid"), "server pid path");

struct HttpServerConf
{
    std::vector<std::string> address;
    int keepalive = 0;
    int timeout = 1000 * 2 *60;
    std::string name;

    bool isValid() const
    {
        return !address.empty();
    }

    bool operator==(const HttpServerConf& oth) const
    {
        return address == oth.address
            && keepalive == oth.keepalive
            && timeout == oth.timeout
            && name == oth.name;
    }
};

template<>
class LexicalCast<std::string, HttpServerConf>
{
public:
    HttpServerConf operator()(const std::string& v)
    {
        YAML::Node node = YAML::Load(v);
        HttpServerConf conf;
        conf.keepalive = node["keepalive"].as<int>(conf.keepalive);
        conf.timeout = node["timeout"].as<int>(conf.timeout);
        conf.name = node["name"].as<std::string>(conf.name);
        if (node["address"].IsDefined())
        {
            for (size_t i = 0; i < node["address"].size(); ++i)
            {
                conf.address.push_back(node["address"][i].as<std::string>());
            }
        }
        return conf;
    }
};

template<>
class LexicalCast<HttpServerConf, std::string>
{
public:
    std::string operator()(const HttpServerConf& conf)
    {
        YAML::Node node;
        node["name"] = conf.name;
        node["keepalive"] = conf.keepalive;
        node["timeout"] = conf.timeout;
        for (auto& item : conf.address)
        {
            node["address"].push_back(item);
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};


static Ref<ConfigVar<std::vector<HttpServerConf>>> http_servers_config = Config::Lookup("http_servers", 
    std::vector<HttpServerConf>(), "http servers config");

Application* Application::s_instance = nullptr;

Application::Application()
{
    s_instance = this;
}

bool Application::init(int argc, char** argv)
{
    m_argc = argc;
    m_argv = argv;
    EnvMgr::GetInstance()->addHelp("s", "start with terminal");
    EnvMgr::GetInstance()->addHelp("d", "run as a daemon");
    EnvMgr::GetInstance()->addHelp("c", "config path default: ./config");
    EnvMgr::GetInstance()->addHelp("p", "print help");

    if (!EnvMgr::GetInstance()->init(argc, argv))
    {
        EnvMgr::GetInstance()->printHelp();
        return false;
    }

    if (EnvMgr::GetInstance()->has("p"))
    {
        EnvMgr::GetInstance()->printHelp();
        return false;
    }
    int run_type = 0;
    if (EnvMgr::GetInstance()->has("s"))
        run_type = 1;
    if (EnvMgr::GetInstance()->has("d"))
        run_type = 2;
    if (run_type == 0)
    {
        EnvMgr::GetInstance()->printHelp();
        return false;
    }

    std::string pidfile = server_work_path->getValue() + "/" + server_pid_file->getValue();
    if (FSUtil::IsRunningPidfile(pidfile))
    {
        TINY_LOG_ERROR(logger) << "server is running: " << pidfile;
        return false;
    }

    std::string conf_path = EnvMgr::GetInstance()->getAbsolutionPath(EnvMgr::GetInstance()->get("c", "config"));
    TINY_LOG_INFO(logger) << "load config path: " << conf_path;
    Config::LoadFromConfDir(conf_path);

    if (!FSUtil::Mkdir(server_work_path->getValue()))
    {
        TINY_LOG_FATAL(logger) << "create work path [" << server_work_path->getValue() << " errno = "
            << errno << " errstr = " << strerror(errno);
            return false;
    }
    return true;
}

bool Application::run()
{
    bool is_daemon = EnvMgr::GetInstance()->has("d");
    return start_daemon(m_argc, m_argv, std::bind(&Application::main, this, 
        std::placeholders::_1, std::placeholders::_2), is_daemon);
}

int Application::main(int argc, char** argv)
{
    std::string pidfile = server_work_path->getValue() + "/" + server_pid_file->getValue();
    std::ofstream ofs(pidfile);
    if (!ofs)
    {
        TINY_LOG_ERROR(logger) << "open pidfile " << pidfile <<" failed";
        return false;
    }
    ofs << getpid();

    IOManager iom(1);
    iom.schedule(std::bind(&Application::run_fiber, this));
    iom.stop();
    return true;
}

int Application::run_fiber()
{
    auto http_confs = http_servers_config->getValue();
    for (auto& item : http_confs)
    {
        TINY_LOG_INFO(logger) << LexicalCast<HttpServerConf, std::string>()(item); 
        std::vector<Ref<Address>> addrs;
        for (auto& i : item.address)
        {
            size_t pos = i.find(":");
            if (pos == std::string::npos)
            {
                TINY_LOG_ERROR(logger) << "invalid address: " << i;
                continue;
            }
            auto addr = Address::LookupAny(i);
            if (addr)
            {
                addrs.push_back(addr);
                continue;
            }
            std::vector<std::pair<Ref<Address>, uint32_t>> result;
            if (!Address::GetInterfaceAddress(result, i.substr(0, pos)))
            {
                TINY_LOG_ERROR(logger) << "invalid address: " << i;
                continue;  
            }
            for (auto& x : result)
            {
                auto ipaddr = std::dynamic_pointer_cast<IPAddress>(x.first);
                if (ipaddr)
                {
                    ipaddr->setPort(atoi(i.substr(pos + 1).c_str()));
                }
                addrs.push_back(ipaddr);
            }
        }
        Ref<http::HttpServer> server(new http::HttpServer(item.keepalive));
        std::vector<Ref<Address>> fails;
        if (!server->bind(addrs, fails))
        {
            for (auto& x : fails)
            {
                TINY_LOG_ERROR(logger) <<"bind address fail: " << *x;
            }
            exit(0);
        }
        server->start();
        m_httpservers.push_back(server);
    }
    return 0;
}


}