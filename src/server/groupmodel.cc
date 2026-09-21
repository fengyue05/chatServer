#include "groupmodel.hpp"
#include "mysql.hpp"
#include <array>
#include <mysql/mysql.h>
#include <string>

/*
allGroup表 记录的是群的id，群的名字，群的描述
groupUsers表 记录的是群的id，组员的id，组内的角色
*/


bool GroupModel::createGroup(Group& group)
{
    std::array<char, 1024> sql;
    // 新创建了一个群也就是变相把这个群的信息放入了allgroup的数据表里面
    std::sprintf(sql.data(), "insert into allgroup(groupname, groupdesc) values('%s', '%s')", 
        group.getName().c_str(), group.getDesc().c_str());

    MySQL mysql;
    if (mysql.connect())
    {
        if (mysql.update(static_cast<std::string>(sql.data())))
        {
            group.setId(mysql_insert_id(mysql.getConnection()));
            return true;
        }
    }
    return false;
}

void GroupModel::addGroup(int userId, int groupId, std::string role)
{
    std::array<char, 1024> sql;
    std::sprintf(sql.data(), "insert into groupuser values(%d, %d, '%s')", 
            groupId, userId, role.c_str());
    MySQL mysql;
    if (mysql.connect())
    {
        mysql.update(static_cast<std::string>(sql.data()));
    }
}

std::vector<Group> GroupModel::queryGroups(int userId)
{
    /*
    现根据userid在groupuser表里面查出该用户所属的群信息
    再根据群信息，查询属于该群组的所有用户的userId，并且和user表进行多表联合查询，查出用户的详细信息 
    */
    std::array<char, 1024> sql;
    std::sprintf(sql.data(), "select a.id, a.groupname, a.groupdesc from allgroup a inner join \
        groupuser b on a.id = b.groupid where b.userid = %d", userId);

    std::vector<Group> groupVec;

    MySQL mysql;
    if (mysql.connect())
    {
        MYSQL_RES* res = mysql.query(static_cast<std::string>(sql.data()));
        if (res != nullptr) 
        {
            MYSQL_ROW row;
            while((row = mysql_fetch_row(res)) != nullptr)
            {
                Group group;
                group.setId(std::atoi(row[0]));
                group.setName(row[1]);
                group.setDesc(row[2]);
                groupVec.push_back(group);
            }
            mysql_free_result(res);
        }
    }
    return groupVec;
}

std::vector<int> GroupModel::queryGroupUsers(int userId, int groupId)
{
    std::array<char, 1024> sql;
    std::sprintf(sql.data(), "select userid from groupuser where groupid = %d and userid != %d", groupId, userId);

    MySQL mysql;
    std::vector<int> userIdVec;
    if (mysql.connect())
    {
        MYSQL_RES* res = mysql.query(static_cast<std::string>(sql.data()));
        if (res != nullptr)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                userIdVec.push_back(std::atoi(row[0]));
            }
            mysql_free_result(res);
        }
    }
    return userIdVec;
}