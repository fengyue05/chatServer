#ifndef _INCLUDE_PUBLIC_H_
#define _INCLUDE_PUBLIC_H_

enum class MsgType 
{
    MSG_LOG = 1,
    MSG_LOG_ACK,
    MSG_REG_ACK,
    MSG_REG,
    MSG_ONE_CHAT,
    MSG_ADD_FRIEND,

    MSG_CREATE_GROUP, // 创建群组
    MSG_ADD_GROUP, // 加入群聊
    MSG_CHAT_MSG,  // 群聊天
};

#endif