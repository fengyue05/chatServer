#include "chatServer.hpp"
#include <muduo/net/EventLoop.h>

int main(int argc, char const *argv[])
{
    EventLoop loop;
    InetAddress addr("127.0.0.1", 6000);   
    chatServer server(&loop, addr, "ChatServer");
    return 0;
}
