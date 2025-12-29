#include "database/DbManager.h"
#include <drogon/orm/DbClient.h>

drogon::orm::DbClientPtr DbManager::client_ = nullptr;

void DbManager::initialize(const std::string &connectionString)
{
    client_ = drogon::orm::DbClient::newPgClient(connectionString, 1);
}

drogon::orm::DbClientPtr DbManager::getClient()
{
    return client_;
}

