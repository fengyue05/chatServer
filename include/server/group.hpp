#ifndef _INCLUDE_SERVER_GROUP_H_
#define _INCLUDE_SERVER_GROUP_H_

#include <string>
#include <vector>
#include "groupuser.hpp"

class Group
{
public:
    Group(int id = -1, std::string name = "", std::string desc = "")
    {
        this->id = id;
        this->name = name;
        this->desc = desc;
    }

    void setId (int id) { this->id = id;}
    void setName (std::string name) { this->name = name;}
    void setDesc (std::string desc) { this->desc = desc;}
    
    int getId() {return id;}
    std::string getName() {return name;}
    std::string getDesc() {return desc;}
    std::vector<GroupUser> &getUsers() {return users;}
private:
    std::string name;
    int id;
    std::string desc; // 描述
    std::vector<GroupUser> users;
};

#endif
