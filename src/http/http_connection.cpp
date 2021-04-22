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
    std::shared_ptr<char> buffers(new char[buffer_size + 1], [](char* ptr){
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
        data[len] = '\0';
        size_t nparser = parser->execute(data, len, false);
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
    //此处必须为引用，赋值逻辑上错误
    auto& client_parser = parser->getClientParser();
    if (client_parser.chunked)
    {
        std::string body;
        int len = offset;
        do
        {
            do
            {
                int res = read(data + len, buffer_size - len);
                if (res <= 0)
                {
                    close();
                    return nullptr;
                }
                len += res;
                data[len] = '\0';
                size_t nparse = parser->execute(data, len, true);
                if (parser->hasError())
                    return nullptr;
                len -= nparse;
                if (len == (int)buffer_size)
                    return nullptr;
            } while (!parser->isFinished());
            len -= 2;
            if (client_parser.content_len <= len)
            {
                body.append(data, client_parser.content_len);//????
                memmove(data, data + client_parser.content_len, len - client_parser.content_len);
                len -= client_parser.content_len;
            }
            else
            {
                body.append(data, len);
                int left = client_parser.content_len - len;
                while (left > 0)
                {
                    int res = read(data, left > (int)buffer_size ? (int)buffer_size : left);
                    if (res <= 0)
                    {
                        close();
                        return nullptr;
                    }
                    body.append(data, res);
                    left -= res;
                }
                len = 0;
            }
        } while (!client_parser.chunks_done);  
        parser->getData()->setBody(body);
    }
    else
    {
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