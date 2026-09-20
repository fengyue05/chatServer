#ifndef _INCLUDE_SERVER_GROUPMODEL_H_
#define _INCLUDE_SERVER_GROUPMODEL_H_

#include <string>
#include <group.hpp>

class GroupModel
{
public:
    // 创建群组
    bool createGroup(Group& group);
    // 加入群组
    void addGroup(int userId, int groupId, std::string role);
    // 查询用户所在群组信息
    std::vector<Group> queryGroups(int userId);
    // 根据指定的groupid查询群组用户id表，除userid自己，主要用户群聊业务给群组其他成员群发消息
    std::vector<int> queryGroupUsers(int userId, int groupId);
};

#endif