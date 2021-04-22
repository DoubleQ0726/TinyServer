#include "http_session.h"
#include "http/http_parser.h"

namespace TinyServer
{
namespace http
{

HttpSession::HttpSession(Ref<Socket> sock, bool owner)
    : SocketStream(sock, owner)
{

}

Ref<HttpRequest> HttpSession::recvRequest()
{
    Ref<HttpRequestParser> parser(new HttpRequestParser);
    uint64_t buffer_size = HttpRequestParser::GetHttpRequestBufferSize();
    std::shared_ptr<char> buffers(new char[buffer_size], [](char* ptr){
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
        size_t nparser = parser->execute(data, len);
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
    return parser->getData();
}

void HttpSession::sendResponse(Ref<HttpResponse> rsp)
{
    std::stringstream ss;
    ss << *rsp;
    std::string data = ss.str();
    writeFixSize(data.c_str(), data.size());
}


}
}