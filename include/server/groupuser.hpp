#ifndef _INCLUDE_SERVER_GROUPUSERS_H_
#define _INCLUDE_SERVER_GROUPUSERS_H_

#include "user.hpp"
class GroupUser : public User
{
public:
    void setRole(std::string role) {this->role = role;}
    std::string getRole() {return this->role;}
private:
    std::string role;
};

#endif