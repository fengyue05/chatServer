#include "offlinemessagemodel.hpp"

#include <mysql.hpp>
#include <array>
#include <mysql/mysql.h>

void offlineMessageModele::insert(int userId, std::string msg)
{
    std::array<char, 1024> sql;
    std::sprintf(sql.data(), "insert into offlinemessage values(%d, '%s')", userId, msg.c_str());
    MySQL mysql;

    if (mysql.connect()) 
    {
        if (mysql.update(static_cast<std::string>(sql.data())))
        {
            return;
        }
    }
}

void offlineMessageModele::remove(int userId)
{
    std::array<char, 1024> sql;
    std::sprintf(sql.data(), "delete from offlinemessage where userid = %d", userId);

    MySQL mysql;
    if (mysql.connect())
    {
        if (mysql.update(static_cast<std::string>(sql.data())))
        {
            return;
        }
    }
}

std::vector<std::string> offlineMessageModele::query(int userId)
{
    std::array<char, 1024> sql;
    std::sprintf(sql.data(), "select message from offlinemessage where userid = %d", userId);

    MySQL mysql;
    std::vector<std::string> vec;
    if (mysql.connect())
    {
        MYSQL_RES* res = mysql.query(static_cast<std::string>(sql.data()));
        if (res != nullptr) 
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                vec.push_back(row[0]); // message
            }
            mysql_free_result(res);
            return vec;
        }
    }
    return vec;
}