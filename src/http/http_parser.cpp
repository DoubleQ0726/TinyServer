#include "http_parser.h"

namspace TinyServer
{
namspcae http
{

HttpRequestParser::HttpRequestParser()
{
    m_data.reset(new HttpRequest);
    http_parser_init(m_parser);


}



}

}