#include "../../include/http/HttpContext.h"
using namespace muduo;
using namespace moude::net;

namespace http
{
    /*
        POST /path/to/resource?key=value HTTP/1.1
        Host: example.com
        Content-Length: 13

        Hello, world!
    */
   //每个行以\r\n结尾！！
   //buffer->peek()  返回的是一个 const char*，指向缓冲区中当前可读数据的起始地址。
   //retrieveUntil 通常没有返回值（void），它的作用是更新缓冲区的内部状态，标记数据已被读取。
   //buf->findCRLF() 返回一个 const char*，指向缓冲区中 \r\n 的起始位置。如果未找到 \r\n：返回 nullptr，表示缓冲区中没有完整的行结束符。的作用是在缓冲区中查找第一个 \r\n 的位置，并返回一个指向该位置的指针。
   //int stoi(const std::string& str, std::size_t* pos = nullptr, int base = 10); 字符串转化为整数库函数
    bool HttpContext::parasRequest(Buffer *buf ,Timestamp receiveTime)
    {
        bool ok = true;//解析行是否正确标志
        bool hasMore = ture;
        while(hasMore)
        {
            if(state_ == ExpectRequestLine)
            {
                const char* crlf = buf->findCRLF();
                if(crlf)
                {
                    ok = processRequestLine(buf->peek(),crlf);
                    if(ok)
                    {
                        request_.setReceiveTime(receiveTime);
                        buf->retrieveUntil(crlf + 2);
                        state_ = ExpectHeaders;
                    }
                    else{
                        hasMore = false;
                    }
                }else
                {
                    hasMore = false;
                }

                
            }else if(state_ == ExpectHeaders)
            {
                const char* crlf = buf->findCRLF();
                if(crlf)
                {
                    const char* colon = std::find(buf->peek(),crlf,':');
                    if(colon < crlf)
                    {
                        request_.addHeader(buf->peek(),colon,crlf);
                    }
                    else if(buf->peek() == crlf)
                    {
                        //表示空行 结束对header的解析

                        if(request_.method() == HttpRequest::POST || request_.method() == HttpRequest::PUT)
                        {
                            std::string contentLength = request_.getHeader("Content-Length");
                            if(!contentLength.empty())
                            {
                                request_.setContentLength(std::stoi(contentLength));
                                if(request_.ContentLength() > 0)
                                {
                                    state_ = ExpectBody;

                                }
                                else
                                {
                                    state_ = GotAll;
                                    hasMore = false;

                                }
                            }else
                            {
                                // POST/PUT 请求没有 Content-Length，是HTTP语法错误
                                ok = false;
                                hasMore = false;
                            }
                        }else
                        {
                            // GET/HEAD/DELETE 等方法直接完成（没有请求体）
                            state_ = GotAll;
                            hasMore = false;
                        }
                    }else
                    {
                        ok = false;//行解析错误
                        hasMore = false;
                    }
                    buf->retrieveUntil(crlf + 2);//到下一行；
                }
            }
            else if(state_ == ExpectBody)
            {
                //检查buf中是否有足够的数据
                if(buf->readableBytes() < request_.contentLength())
                {
                    hasMore = false;//数据不完整等待更多数据
                    return false;
                }

                std::string body(buf->peek(),buf->peeka() + request_.contentLength());
                request_.setBody(body);
                // 准确移动读指针
                buf->retrieve(request_.contentLength());

                state_ = kGotAll;
                hasMore = false;    
            }
        }
        return ok; // ok为false代表报文语法解析错误
    }

    //GET /path/to/resource?key=value&foo=bar HTTP/1.1
    bool HttpContext::processRequestLine(const char* start,const char* end)
    {
        bool succeed = false;
        const char *start = begin;
        const char *space = std::find(start,end," ");
        if(space != end && request_.setMethod(start,space))
        {
            start = space+1;
            space = std::find(start,end,' ');
            if(space != end)
            {
                const char * argumentStart = std::find(start,space,'?');
                if(argumentStart!= space)//请求带参数
                {
                    request_.setPath(start,argumentStart);//
                    request_.setQueryParameters(argumentStart+1,space);
                }
                else
                {
                    request_.setPath(start,argumentStart);//
                }
            }
            start = space+1;
            succeed = ((end - start == 8) && std::equal(start,end-1,"HTTP/1."));
            if(succeed)
            {
                if(*(end - 1) == '1')
                {
                    request_.setVersion("HTTP/1.1");
                }
                else if(*(end - 1) == '0')
                {
                    request_.setVersion("HTTP/1.0");
                }else{
                    succeed = false;
                }
            }
        }
        return sccceed;
    }
}