#ifndef _INCLUDE_SERVER_CHATSERVICE_H_
#define _INCLUDE_SERVER_CHATSERVICE_H_

// 聊天业务服务类
#include <muduo/base/Timestamp.h>
#include <muduo/net/Callbacks.h>
#include <nlohmann/json_fwd.hpp>
#include <unordered_map>
#include <functional>
#include <muduo/net/TcpConnection.h>
#include <nlohmann/json.hpp>
#include "usermodel.hpp"
#include "friendmodel.hpp"
#include "offlinemessagemodel.hpp"
#include "groupmodel.hpp"
#include <mutex>

using namespace muduo::net;
using json = nlohmann::json;
using namespace muduo;

using MsgHandler = std::function<void(TcpConnectionPtr conn, json& js, Timestamp time)>;

class chatService
{
public:
    static chatService* instance();

    // 登录
    void login(TcpConnectionPtr conn, json& js, Timestamp time);

    // 注册
    void reg(TcpConnectionPtr conn, json& js, Timestamp time);

    MsgHandler getHandler(int msg);

    // 客户异常退出
    void clientCloseException(const TcpConnectionPtr& conn);

    // 一对一聊天
    void oneChat (TcpConnectionPtr conn, json& js, Timestamp time);

    void reset();

    void addFriend(TcpConnectionPtr conn, json& js, Timestamp time);

    void createGroup(TcpConnectionPtr conn, json& js, Timestamp time);

    void addGroup(TcpConnectionPtr conn, json& js, Timestamp time);

    void groupChat(TcpConnectionPtr conn, json& js, Timestamp time);
private:
    chatService();

    // 存储消息id和对应业务的处理方法
    std::unordered_map<int, MsgHandler> msgHandlerMap_;

    std::mutex mutex_;
    std::unordered_map<int, TcpConnectionPtr> userConnMap_;
    // 数据操作类
    userModel userModel_; 

    // 离线信息操作类
    offlineMessageModele offlineMessageModel_;

    // 好友操作类
    FriendModel friendModel_;

    // 群组操作类
    GroupModel groupModel_;
};


#endif