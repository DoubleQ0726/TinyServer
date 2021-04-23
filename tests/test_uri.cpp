#include "uri.h"

using namespace TinyServer;

int main()
{
    //Ref<Uri> uri = Uri::Create("http://www.sylar.top/test/uri?id=100&name=syalr#frag");
    Ref<Uri> uri = Uri::Create("http://www.sylar.top/test/uri?id=100&name=syalr#frag中文");
    std::cout << uri->toString() << std::endl;
    Ref<Address> addr = uri->createAddress();
    std::cout << *addr << std::endl;
    return 0;
}