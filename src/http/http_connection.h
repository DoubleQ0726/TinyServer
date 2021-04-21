#pragma once
#include "socket_stream.h"
#include "http/http.h"

namespace TinyServer
{
namespace http
{
class HttpConnection : public SocketStream
{
public:
    HttpConnection(Ref<Socket> sock, bool owner = true);
    Ref<HttpResponse> recvResponse();
    void sendRequest(Ref<HttpRequest> req);

private:
};

}
}

