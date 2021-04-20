#pragma once
#include "socket_stream.h"
#include "http/http.h"

namespace TinyServer
{
namespace http
{
class HttpSession : public SocketStream
{
public:
    HttpSession(Ref<Socket> sock, bool owner = true);
    Ref<HttpRequest> recvRequest();
    void sendResponse(Ref<HttpResponse> rsp);

private:
};







}
}

