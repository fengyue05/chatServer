#ifndef _INCLUDE_SERRVER_OFFLINEMESSAGEMODEL_H_
#define _INCLUDE_SERRVER_OFFLINEMESSAGEMODEL_H_

#include <vector>
#include <string>

class offlineMessageModele
{
public:
    void insert (int userId, std::string msg);

    void remove (int userId);

    std::vector<std::string> query(int userId);
};


#endif