#pragma once

#include <drogon/orm/DbClient.h>
#include <memory>
#include <string>

class DbManager
{
public:
    static void initialize(const std::string &connectionString);
    static drogon::orm::DbClientPtr getClient();
    
private:
    static drogon::orm::DbClientPtr client_;
};

