#include "user.hpp"

User::User (int id, std::string name, std::string password, std::string state)
    : id_ (id)
    , name_(name)
    , password_(password)
    , state_(state)
{}