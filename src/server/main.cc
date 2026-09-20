#include "chatServer.hpp"
#include "chatService.hpp"
#include <muduo/net/EventLoop.h>
#include <signal.h>

void resetHandler(int) 
{
    chatService::instance()->reset();
    exit(0);
}

int main(int argc, char const *argv[])
{
    signal(SIGINT, resetHandler);

    EventLoop loop;
    InetAddress addr("127.0.0.1", 6000);   
    chatServer server(&loop, addr, "ChatServer");
    return 0;
}
