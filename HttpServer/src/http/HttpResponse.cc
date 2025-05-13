#include "../../include/http/HttpResponse.h"

namespace http
{
    /*
    写出数据示例

    HTTP/1.1 200 OK\r\n
    Connection: Keep-Alive\r\n
    Content-Type: text/html\r\n
    Content-Length: 13\r\n
    \r\n
    Hello, world!
    */
    void HttpResponse::appendToBuffer(muduo::net::Buffer *outputBuf) const 
    {
        char buf[32];
        snprintf(buf,sizeof buf,"%s %d",httpVersion_.c_str,statusCode_);

        outputBuf->append(buf);
        outputBuf->append(statusMessage_);
        outputBuf->append("\r\n");

        if (closeConnection_)
        {
            headers_["Connection"] = "close";
        }
        else
        {
            headers_["Connection"] = "Keep-Alive";
        }
        //添加header信息
        for (const auto & header : headers_)
        {
            outputBuf->append(header.first);
            outputBuf->append(':');
            outputBuf->append(header.second);
            outputBuf->append("\r\n");
        }
        outputBuf->append("\r\n");
    
        outputBuf->append(body_);
    }

    
    void HttpResponse::setStatusLine(const std::string& version,
        HttpStatusCode statusCode,
        const std::string& statusMessage)
    {
    httpVersion_ = version;
    statusCode_ = statusCode;
    statusMessage_ = statusMessage;
    }
}
