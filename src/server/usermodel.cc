#include "usermodel.hpp"
#include <array>
#include <mysql/mysql.h>
#include "mysql.hpp"
#include "user.hpp"

bool userModel::insert(User& user)
{
    // 组装sql语句
    std::array<char, 1024> sql;
    std::sprintf(sql.data(), "insert into User(name, password, state) values('%s', '%s', '%s')", 
        user.getName().c_str(), user.getPwd().c_str(), user.getState().c_str());

    MySQL mysql;
    if (mysql.connect())
    {
        if (mysql.query(static_cast<std::string>((sql.data()))))
        {
            if (mysql.update(static_cast<std::string>(sql.data())))
            {
                user.setId(mysql_insert_id(mysql.getConnection()));
                return true;
            }
        }
    }

    return false;
}

User userModel::query(int id)
{
    std::array<char, 1024> sql;
    std::sprintf(sql.data(), "select * from user where id = %d", id);

    MySQL mysql;
    if (mysql.connect())
    {
        MYSQL_RES* res = mysql.query(static_cast<std::string>(sql.data()));
        if (res != nullptr)
        {
            MYSQL_ROW row = mysql_fetch_row(res);
            if (row != nullptr)
            {
                User user;
                user.setId(std::atoi(row[0]));
                user.setName(row[1]);
                user.setPwd(row[2]);
                user.setState(row[3]);
                return user;
            }
        }
    }

    return User();
}

bool userModel::updateState(User& user)
{
    std::array<char, 1024> sql;
    std::sprintf(sql.data(), "update user set state = '%s' where id is %d", user.getState().c_str(), user.getId());

    MySQL mysql;
    if (mysql.connect())
    {
        if (mysql.update(static_cast<std::string>(sql.data())))
        {
            return true;
        }
    }
    return false;
}