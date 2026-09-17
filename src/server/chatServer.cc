#include "chatServer.hpp"
#include <functional>


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
        conn->shutdown();
    }
}

void chatServer::onMessage(const TcpConnectionPtr&, Buffer*, Timestamp)
{

}