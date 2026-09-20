#include "friendmodel.hpp"
#include <array>
#include <mysql.hpp>
#include <mysql/mysql.h>

void FriendModel::insert(int userId, int friendId)
{
    std::array<char, 1024> sql;
    std::sprintf(sql.data(), "insert into friend values(%d, %d)", userId, friendId);

    MySQL mysql;
    if (mysql.connect())
    {
        mysql.update(static_cast<std::string>(sql.data()));
    }
}

std::vector<User> FriendModel::query(int userId)
{
    std::array<char, 1024> sql;
    std::sprintf(sql.data(), "select a.id, a.name, a.state from user a inner join friend b on b.friendid = a.id where b.userid = %d", userId);

    MySQL mysql;
    std::vector<User> vec;
    if (mysql.connect())
    {
        MYSQL_RES* res = mysql.query(static_cast<std::string>(sql.data()));
        if (res != nullptr) 
        {
            MYSQL_ROW row;
            while((row = mysql_fetch_row(res)) != nullptr)
            {
                User user;
                user.setId(std::atoi(row[0]));
                user.setName(row[1]);
                user.setState(row[2]);
                vec.push_back(user);
            }
            mysql_free_result(res);
            return vec;
        }
    }
    return vec;
}