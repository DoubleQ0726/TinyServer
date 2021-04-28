#pragma once
#include "env.h"
#include "http/http_server.h"
namespace TinyServer
{

class Application
{
public:
    Application();

    static Application* GetInstance() { return s_instance; }

    bool init(int argc, char** argv);
    bool run();

private:
    int main(int argc, char** argv);
    int run_fiber();

private:
    int m_argc = 0;
    char** m_argv = nullptr;

    std::vector<Ref<http::HttpServer>> m_httpservers;
    static Application* s_instance;
};










}