#ifndef _INCLUDE_DB_MYSQL_H_
#define _INCLUDE_DB_MYSQL_H_

#include <muduo/base/Logging.h>
#include <mysql/mysql.h>
#include <string>

static std::string dbname = "chat";
static std::string password = "yu070505";
static std::string server = "127.0.0.1";
static std::string user = "root";

class MySQL
{
public:
    MySQL()
    {
        conn_ = mysql_init(nullptr);
    }

    ~MySQL()
    {
        if (conn_ != nullptr) 
        {
            mysql_close(conn_);
        }
    }

    bool connect()
    {
        MYSQL* p = mysql_real_connect(conn_, server.c_str(), user.c_str(),
         password.c_str(), dbname.c_str(), 3306, nullptr, 0);
        if (p != nullptr) 
        {
            // 只有连接成功，才执行 SQL，将当前连接的客户端、连接和结果字符集设置为 gbk。这不会修改数据库表本身的编码
            mysql_query(conn_, "set names gbk");
            LOG_INFO << "connect mysql success!";
        }
        else 
        {
            LOG_INFO << "connect mysql failed!";
            return p;
        }
        return true;
    }

    bool update(std::string sql)
    {
        // 这个 if 的条件是 mysql_query(...) 返回非零值，也就是 SQL 执行失败时
        if (mysql_query(conn_, sql.c_str())) 
        {
            LOG_INFO << __FILE__ << ":" << __LINE__ << ":" << sql << "更新失败";
            return false;
        }
        return true;
    }

    MYSQL_RES* query(std::string sql)
    {
        if (mysql_query(conn_, sql.c_str()))
        {
            LOG_INFO << __FILE__ << ":" << __LINE__ << ":" << sql << "查询失败";
            return nullptr;
        }
        return mysql_use_result(conn_);
    }

    MYSQL* getConnection() 
    {
        return conn_;
    }

private:
    MYSQL* conn_;
};

#endif