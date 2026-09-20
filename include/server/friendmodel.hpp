#ifndef _INCLUDE_SERVER_FRIENDMODEL_H_
#define _INCLUDE_SERVER_FRIENDMODEL_H_

#include "user.hpp"
#include <vector>

class FriendModel
{
public:
    void insert(int userId, int friendId);

    std::vector<User> query(int userId);
};

#endif