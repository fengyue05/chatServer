#include "chatServer.hpp"
#include "chatService.hpp"
#include <nlohmann/json.hpp>
#include <functional>
#include <nlohmann/json_fwd.hpp>
#include <string>

using json = nlohmann::json;

chatServer::chatServer(EventLoop* loop, const InetAddress& listenAddr, const std::string& nameArg)
    : server_(loop, listenAddr, nameArg)
    , loop_(loop)
{
    server_.setConnectionCallback(std::bind(&chatServer::onConnection, this, std::placeholders::_1));
    
    server_.setMessageCallback(std::bind(&chatServer::onMessage, this,
             std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    
    server_.setThreadNum(4);
}

void chatServer::start()
{
    server_.start();
}

void chatServer::onConnection(const TcpConnectionPtr& conn)
{
    // 用户断开连接
    if (!conn->connected())
    {
        chatService::instance()->clientCloseException(conn);
        conn->shutdown();
    }
}

void chatServer::onMessage(const TcpConnectionPtr& conn, Buffer* buffer, Timestamp time)
{
    // 把缓冲区的数据放到字符串里面
    std::string buf = buffer->retrieveAllAsString();
    // 数据反序列化
    json js = json::parse(buf);
    // 这里达到的目的：完全解耦网络模块的代码和业务模块的代码
    // 通过js["msgid"] 获取一个业务handler => conn js time 
    auto handler = chatService::instance()->getHandler(js["msgid"].get<int>());
    handler(conn, js, time);
}