#ifndef _INCLUDE_SERVER_CHATSERVER_H_
#define _INCLUDE_SERVER_CHATSERVER_H_

#include <muduo/base/Timestamp.h>
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
using namespace muduo;
using namespace muduo::net;

class chatServer
{
public:
    chatServer(EventLoop* loop, const InetAddress& listenAddr, const std::string& nameArg);

    // 启动服务
    void start();

private:
    // 上报连接信息的回调函数
    void onConnection(const TcpConnectionPtr&);
    // 上报读写事件的回调函数
    void onMessage(const TcpConnectionPtr&, Buffer*, Timestamp);

    TcpServer server_; // 组合muduo库，实现服务器功能的类对象
    EventLoop* loop_;  // 指向事件循环对象的指针
};

#endif