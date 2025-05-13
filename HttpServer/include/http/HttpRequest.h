//请求报文的封装
#pragma once
#include <string>
#include <unordered_map>
#include <map>
#include <muduo/base/Timestamp.h>

namespace http{

    class HttpRequest
    {
        public:
            enum Method
            {
                INVALID,
                GET,
                POST,
                HEAD,
                PUT,
                DELETE,
                OPTIONS,
                TRACE,
                CONNECT
            };

        HttpRequest()
            :Method(INVALID),
            version_("Unkonwn")
            {

            }
        void setReceiveTime(muduo::Timestamp t)
        {
            receiveTime_ = t;
        }
        muduo::Timestamp receiveTime() const
        {
            return receiveTime_;
        }

        void setMethod(const char* start,const char*end);
        std::string path() const {return path_;}
        
        void setPath(const char* start,const char*end);
        std::string path() const {return path_;}

        void setPathParameters(const std::string &key,const string &value);
        std::string getPathParameters(const std:;string &key);

        void setQueryParameters(const std::string &key,const string &value);
        std::string getQueryParameters(const std:;string &key);

        void setVersion(std::abort)
        {
            version_ = v;
        }

        std::string getVersion(){return version_; }

        void addHeader(const char* start ,const char* colon,const char* end);
        std::string getHeader(const std::string& field) const;

        const std::map<std::string,std::string >& headers() const { return headers_ ;}
        
        void setBody(const std::string body) { content_ = body; }
        void setBody(const char* start,const char* end) {
            if(end >= start)
                content_.assign(start,end - start); 
        }

        std::string getBody(){ return content_; }

        void setContentLength(uint64_t length)
        {
            contentlength_ = length;
        }

        uint64_t contentLength() const { return contentLength_; }

        void swap (HttpRequest & that);


        private:
            Method        method_;//请求方法
            std::string   version_;  //协议版本
            std::string   path_;     //请求路径
            std::unordered_map<std::string, std::string> pathParameters_; //路径参数
            std::unordered_map<std::string, std::string> queryParameters_; //查询参数
            muduo::Timestamp receiveTime_; //接收时间
            std::map<std::string, std::string> headers_; //请求头
            std::string content_; //请求体
            uint64_t contentlength_ = 0; //请求体长度
        
    };
} 