#include "http_connection.h"
#include "http/http_parser.h"

namespace TinyServer
{
namespace http
{

HttpConnection::HttpConnection(Ref<Socket> sock, bool owner)
    : SocketStream(sock, owner)
{

}

Ref<HttpResponse> HttpConnection::recvResponse()
{
    Ref<HttpResponseParser> parser(new HttpResponseParser);
    uint64_t buffer_size = HttpResponseParser::GetHttpResponseBufferSize();
    std::shared_ptr<char> buffers(new char[buffer_size], [](char* ptr){
        delete[] ptr;
    });
    char* data = buffers.get();
    size_t offset = 0;
    do
    {
        int len = read(data + offset, buffer_size - offset);
        if (len <= 0)
            return nullptr;
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
                return nullptr;
        }
        parser->getData()->setBody(body);
    }
    return parser->getData();
}

void HttpConnection::sendRequest(Ref<HttpRequest> rsp)
{
    std::stringstream ss;
    ss << *rsp;
    std::string data = ss.str();
    writeFixSize(data.c_str(), data.size());
}


}
}