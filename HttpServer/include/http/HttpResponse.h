#pragma once
#include "muduo/net/TcpServer.h"

namespace http
{
    
    class HttpResponse
    {
        public:
            enum HttpStatusCode
            {
                Unknown,
                Ok = 200,
                NoContent = 204,
                301MovedPermanently = 301,
                400BadRequest = 400,
                401Unauthorized = 401,
                403Forbidden = 403,
                404NotFound = 404,
                409Conflict = 409,
                500InternalServerError = 500,
            };

        HttpResponse(bool close true):
        statusCode_(Unknown),
        closeConnection_(close)
        {}
        
        void setVersion(std::string version)
        { httpVersion_ = version; }
        void setStatusCode(HttpStatusCode code)
        { statusCode_ = code; }

        HttpStatusCode getStatusCode() const
        { return statusCode_; }

        void setStatusMessage(const std::string message)
        { statusMessage_ = message; }

        void setCloseConnection(bool on)
        { closeConnection_ = on; }

        bool closeConnection() const
        { return closeConnection_; }

        void setContentType(const std::string& contentType)
        { addHeader("Content-Type", contentType); }

        void setContentLength(uint64_t length)
        { addHeader("Content-Length", std::to_string(length)); }

        void addHeader(const std::string& key, const std::string& value)
        { headers_[key] = value; }
        
        void setBody(const std::string& body)
        { 
            body_ = body;
            // body_ += "\0";
        }

        void setStatusLine(const std::string& version,
            HttpStatusCode statusCode,
            const std::string& statusMessage);

        void setErrorHeader(){}

        void appendToBuffer(muduo::net::Buffer* outputBuf) const;

        private:
            std::string httpVersion_; //http协议版本
            HttpStatusCode statusCode_; //状态码
            std::string statusMessage_; //状态码信息
            bool closeConnection_; //是否关闭连接
            std::map<std::string,std::string> headers_;//响应头
            std::strign body_;//响应体
            bool isFile_; //是否是文件
    };

}