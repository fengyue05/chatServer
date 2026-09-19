#include "chatService.hpp"
#include <muduo/base/Logging.h>
#include "mysql.hpp"
#include "public.hpp"
#include "user.hpp"

chatService* chatService::instance()
{
    static chatService service;
    return &service;
}

void chatService::login(TcpConnectionPtr conn, json& js, Timestamp time)
{
    int id = js["id"];
    std::string pwd = js["password"];

    User user = userModel_.query(id);

    if (id != -1 && pwd == user.getPwd())
    {
        if (user.getState() == "online")
        {
            json response;
            response["msgid"] = MsgType::MSG_LOG_ACK;
            response["id"] = user.getId();
            response["errno"] = 2;
            response["errmsg"] = "重复登陆，用户已经在线";
            conn->send(response.dump());          
        }
        else 
        {
            user.setState("online");
            userModel_.updateState(user);

            {
                std::lock_guard<std::mutex> lock(mutex_);
                userConnMap_.insert({id, conn});
            }

            json response;
            response["msgid"] = MsgType::MSG_LOG_ACK;
            response["id"] = user.getId();
            response["errno"] = 0;
            conn->send(response.dump());     
        }
    }
    else 
    {
        json response;
        response["msgid"] = MsgType::MSG_LOG_ACK;
        response["errno"] = 1;
        response["errmsg"] = "用户密码错误";
        conn->send(response.dump()); 
    }
}

void chatService::reg(TcpConnectionPtr conn, json& js, Timestamp time)
{
    std::string name = js["name"];
    std::string pwd = js["password"];

    User user;
    user.setName(name);
    user.setPwd(password);
    bool state = userModel_.insert(user);

    if (state)
    {
        json response;
        response["msgid"] = MsgType::MSG_REG_ACK;
        response["id"] = user.getId();
        response["errno"] = 0;
        conn->send(response.dump());
    }
    else 
    {
        json response;
        response["msgid"] = MsgType::MSG_REG_ACK;
        response["errno"] = 1;
        conn->send(response.dump()); 
    }
}

MsgHandler chatService::getHandler(int msgid)
{
    auto it = msgHandlerMap_.find(msgid);
    if (it == msgHandlerMap_.end())
    {
        return [&](TcpConnectionPtr conn, json& js, Timestamp time){
            LOG_ERROR << "msgid = " << msgid << " can not find";
        };
    }
    else 
    {
       return msgHandlerMap_[msgid]; 
    }
}

chatService::chatService()
{
    msgHandlerMap_.insert({static_cast<int>(MsgType::MSG_LOG), std::bind(&chatService::login, this
        , std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)});
    msgHandlerMap_.insert({static_cast<int>(MsgType::MSG_REG), std::bind(&chatService::reg, this
        , std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)});    
}