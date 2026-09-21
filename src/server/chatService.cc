#include "chatService.hpp"
#include <muduo/base/Logging.h>
#include <mutex>
#include <vector>
#include "groupmodel.hpp"
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
            response["name"] = user.getName();

            std::vector<std::string> vec = offlineMessageModel_.query(id);
            if (!vec.empty())
            {
                response["offlinemsg"] = vec;
                offlineMessageModel_.remove(id);
            }

            std::vector<User> vecFriend = friendModel_.query(id);
            if (!vecFriend.empty())
            {
                std::vector<std::string> vec2;
                for (User& user : vecFriend)
                {
                    json js;
                    js["id"] = user.getId();
                    js["name"] = user.getName();
                    js["state"] = user.getState();
                    vec2.push_back(js.dump());
                }
                response["friends"] = vec2;
            }

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
    msgHandlerMap_.insert({static_cast<int>(MsgType::MSG_ONE_CHAT), std::bind(&chatService::oneChat, this
        , std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)});
    msgHandlerMap_.insert({static_cast<int>(MsgType::MSG_ADD_FRIEND), std::bind(&chatService::oneChat, this
        , std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)});
}

void chatService::clientCloseException(const TcpConnectionPtr& conn)
{
    User user;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto it = userConnMap_.begin(); it != userConnMap_.end(); it++)
        {
            if (it->second == conn)
            {
                user.setId(it->first);
                userConnMap_.erase(it);
                break;
            }
        }
    }

    if (user.getId() == -1)
    {
        user.setState("offline");
        userModel_.updateState(user);
    }
}

void chatService::oneChat(TcpConnectionPtr conn, json& js, Timestamp time)
{
    int toid = js["to"].get<int>();
    
    {
        // 在线的情况
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = userConnMap_.find(toid);
        if (it != userConnMap_.end())
        {
            it->second->send(js.dump());
            return;
        }
    }

    // 离线的情况
    /*
    这个代码的运行逻辑是：当对方的用户在离线的情况之下，我们会把信息放在我们创建的离线数据库下面，当对方登录的时候，会触发onMessage的回调函数
    这个时候onMessage的回调函数里面有：auto handler = chatService::instance()->getHandler(js["msgid"].get<int>());
    这个代码返回的是handler，这是一个函数，具体是什么函数根据msgid来决定，如果是登录，log，那么就会在登录之后获取离线数据库里面对应的信息
    最后和resonse一起返回给用户
    然后删除这个用户在离线数据库里面的信息
    */
    offlineMessageModel_.insert(toid, js.dump());
    return;
}

void chatService::reset()
{
    userModel_.reset();
}

void chatService::addFriend(TcpConnectionPtr conn, json& js, Timestamp time)
{
    int userId = js["id"].get<int>();
    int friendId = js["friendId"].get<int>();

    friendModel_.insert(userId, friendId);   
}

void chatService::createGroup(TcpConnectionPtr conn, json& js, Timestamp time)
{
    int userId = js["id"].get<int>();
    std::string name = js["groupname"];
    std::string desc = js["groupdesc"];

    Group group(-1, name, desc);
    if (groupModel_.createGroup(group))
    {
        groupModel_.addGroup(userId, group.getId(), "creator");
    }
}

void chatService::addGroup(TcpConnectionPtr conn, json& js, Timestamp time)
{
    int userId = js["id"].get<int>();
    int groupId = js["groupid"].get<int>();

    groupModel_.addGroup(userId, groupId, "nomal");
}

void chatService::groupChat(TcpConnectionPtr conn, json& js, Timestamp time)
{
    int userId = js["id"].get<int>();
    int groupId = js["groupid"].get<int>();
    std::vector<int> userIdVec = groupModel_.queryGroupUsers(userId, groupId);
    std::lock_guard<std::mutex> lock(mutex_);
    for (int id : userIdVec)
    {
        auto it = userConnMap_.find(id);
        if (it != userConnMap_.end())
        {
            it->second->send(js.dump());
        }
        // 离线
        else  
        {
            offlineMessageModel_.insert(id, js.dump());
        }
    }
}