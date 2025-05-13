#include <sys/types.h>
#include <unordered_map>

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/Logging.h>

#include "HttpContext.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "../router/Router.h"
#include "../session/SessionManager.h"
#include "../middleware/MiddlewareChain.h"
#include "../middleware/cors/CroMiddleware.h"
#include "../ssl/SslConnection.h"
#include "../ssl/SslContext.h"

class HttpRequest;
class HttpResponse;

namespace http
{
    class HttpServer : muduo::noncopyable
    {
        public:
            using HttpCallback = std::function<void (const http::HttpRequest&,http::HttpResponse*)>;

            //构造函数
            /*
            Muduo 提供了以下选项：
            kNoReusePort：
            不启用端口复用。
            默认行为，适合简单的单线程服务器。
            kReusePort：
            启用 SO_REUSEPORT。
            适合多线程服务器，每个线程可以独立绑定到同一个端口，提升性能。
            */
            HttpServer(int port,
                        const std::string &name,
                        bool useSSL = false,
                        muduo::net::TcpServer::Option option = muduo::net::TcpServer::kNoReusePort);

            void setThreadNum(int numThreads)
            {
                server_.setThreadNum(numThreads);
            }
        
            void start();

            muduo::net::EventLoop* getLoop() const
            {
                return server_.getLoop();
            }

            void setHttpCallback(const HttpCallback & cb)
            {
                httpCallback_ = cb;
            }

            //注册静态路由
            void Get(const std::string & path,cosnt HttpCallback&cb)
            {
                router_.registerCallback(HttpRequest::GET,path,cb);
            }
            //注册静态路由
            void Get(const std::string& path,router::Router::HandlerPtr handler)
            {
                router_.registerHandler(HttpRequest::GET,path,handler);
            }

            void Post(const std::string& path, const HttpCallback& cb)
            {
                router_.registerCallback(HttpRequest::POST, path, cb);
            }

            void Post(const std::string& path, router::Router::HandlerPtr handler)
            {
                router_.registerHandler(HttpRequest::POST, path, handler);
            }

            //注册动态路由表
            void addRoute(HttpRequest::Method method,const std::string &path,const router::Router::HandlerCallback& callback)
            {
                router_.addRegexCallback(method,path,callback);
            }
            // 注册动态路由处理器
            void addRoute(HttpRequest::Method method, const std::string& path, router::Router::HandlerPtr handler)
            {
                router_.addRegexHandler(method, path, handler);
            }
            // 获取会话管理器
            session::SessionManager* getSessionManager() const
            {
                return sessionManager_.get();
            }

            // 添加中间件的方法
            void addMiddleware(std::shared_ptr<middleware::Middleware> middleware) 
            {
                middlewareChain_.addMiddleware(middleware);
            }

            void enableSSL(bool enable) 
            {
                useSSL_ = enable;
            }

            void setSslConfig(const ssl::SslConfig& config);


        private:
            //初始化
            void initialize();
            //连接成功 回调
            void onConnection(const muduo::net::TcpConnectionPtr& conn);
            //获取信息成功回调
            void onMessage(const muduo::net::TcpConnectionPtr& conn,
                           muduo::net::Buffer* buf,
                           muduo::Timestamp receiveTime);
            //获取信息成功回调后回调处理
            void onRequest(const muduo::net::TcpConnectionPtr&, const HttpRequest&);
            // 执行请求对应的路由处理函数
            void handleRequest(const HttpRequest& req, HttpResponse* resp);
        private:
            muduo::net::InetAddress listenAddr_;//监听地址
            muduo::net::TcpServer server_;//
            muduo::net::EventLoop mainLoop_;//主循环
            HttpCallback httpCallback_;//回调函数
            router::Router router_;//路由器
            std::unique_ptr<session::SessionManager> sessionManager_;//会话管理器
            middleware::MiddlewareChain minddlewareChain_;//中间件链
            std::unique_ptr<ssl::SslContext>             sslCtx_; // SSL 上下文
            bool                                         useSSL_; // 是否使用 SSL   
            // TcpConnectionPtr -> SslConnectionPtr 
            std::map<muduo::net::TcpConnectionPtr, std::unique_ptr<ssl::SslConnection>> sslConns_;
    };
}
