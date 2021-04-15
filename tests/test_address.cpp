#include "log.h"
#include "address.h"

using namespace TinyServer;

Ref<Logger> logger = TINY_LOG_ROOT;

void test()
{
    std::vector<Ref<Address>> addrs;
    bool res = Address::Lookup(addrs, std::string("www.baidu.com"));
    if (!res)
        TINY_LOG_ERROR(logger) << "lookup fail";
    for (size_t i = 0; i < addrs.size(); ++i)
    {
        TINY_LOG_INFO(logger) << i << " - " << addrs[i]->toString();
    }
}

void test_iface()
{
    std::multimap<std::string, std::pair<Ref<Address>, uint32_t>> addrs;
    bool res = Address::GetInterfaceAddress(addrs);
    if (!res)
        TINY_LOG_ERROR(logger) << "GetInterfaceAddress fail";
    for (auto& item : addrs)
    {
        TINY_LOG_INFO(logger) << item.first << " - " 
            << item.second.first->toString() << " - " << item.second.second;
    }
}

void test_ipv4()
{
    auto addr = IPAddress::Create("www.sylar.top");
    if (addr)
    {   
        TINY_LOG_INFO(logger) << addr->toString();
    }
}

int main()
{
    //test();
    test_iface();
    //test_ipv4();
    return 0;
}