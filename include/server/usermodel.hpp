#ifndef _INCLUDE_SERVER_USERMODEL_H_
#define _INCLUDE_SERVER_USERMODEL_H_

#include "user.hpp"

class userModel
{
public:
    bool insert(User& user);

    User query(int id);

    bool updateState(User& user);
private:
    
};

#endif