#include "controllers/AccountController.h"
#include "database/DbManager.h"
#include "utils/JsonResponse.h"
#include <drogon/orm/DbClient.h>
#include <random>
#include <stdexcept>
#include <json/json.h>
#include <numeric>
#include <vector>

void AccountController::create(const drogon::HttpRequestPtr &req,
                              std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                              const std::string &id)
{
    auto json = req->getJsonObject();
    if (!json || !json->isMember("code") || !json->isMember("name") || !json->isMember("type"))
    {
        callback(JsonResponse::error("Missing required fields: code, name, type"));
        return;
    }

    std::string code = (*json)["code"].asString();
    std::string name = (*json)["name"].asString();
    std::string type = (*json)["type"].asString();
    std::string parentId = json->isMember("parent_id") ? (*json)["parent_id"].asString() : "";
    std::string description = json->isMember("description") ? (*json)["description"].asString() : "";

    // Generate UUID using random generator
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    std::string uuidStr = "xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx";
    for (char &c : uuidStr) {
        if (c == 'x') {
            c = "0123456789abcdef"[dis(gen)];
        } else if (c == 'y') {
            c = "89ab"[dis(gen) % 4];
        }
    }

    auto client = DbManager::getClient();
    if (!client)
    {
        callback(JsonResponse::error("Database connection failed", 500));
        return;
    }

    std::string query;
    if (parentId.empty())
    {
        query = "INSERT INTO accounts (id, code, name, type, description) VALUES ($1, $2, $3, $4::account_type, $5) RETURNING id, code, name, type, parent_id, description, is_active";
        client->execSqlAsync(
            query,
            [callback](const drogon::orm::Result &result) {
                if (result.size() > 0)
                {
                    Json::Value account;
                    account["id"] = result[0]["id"].as<std::string>();
                    account["code"] = result[0]["code"].as<std::string>();
                    account["name"] = result[0]["name"].as<std::string>();
                    account["type"] = result[0]["type"].as<std::string>();
                    if (!result[0]["parent_id"].isNull())
                        account["parent_id"] = result[0]["parent_id"].as<std::string>();
                    if (!result[0]["description"].isNull())
                        account["description"] = result[0]["description"].as<std::string>();
                    account["is_active"] = result[0]["is_active"].as<bool>();
                    callback(JsonResponse::success(account));
                }
                else
                {
                    callback(JsonResponse::error("Failed to create account", 500));
                }
            },
            [callback]() {
                callback(JsonResponse::error("Database error", 500));
            },
            std::string(uuidStr), code, name, type, description
        );
    }
    else
    {
        query = "INSERT INTO accounts (id, code, name, type, parent_id, description) VALUES ($1, $2, $3, $4::account_type, $5, $6) RETURNING id, code, name, type, parent_id, description, is_active";
        client->execSqlAsync(
            query,
            [callback](const drogon::orm::Result &result) {
                if (result.size() > 0)
                {
                    Json::Value account;
                    account["id"] = result[0]["id"].as<std::string>();
                    account["code"] = result[0]["code"].as<std::string>();
                    account["name"] = result[0]["name"].as<std::string>();
                    account["type"] = result[0]["type"].as<std::string>();
                    if (!result[0]["parent_id"].isNull())
                        account["parent_id"] = result[0]["parent_id"].as<std::string>();
                    if (!result[0]["description"].isNull())
                        account["description"] = result[0]["description"].as<std::string>();
                    account["is_active"] = result[0]["is_active"].as<bool>();
                    callback(JsonResponse::success(account));
                }
                else
                {
                    callback(JsonResponse::error("Failed to create account", 500));
                }
            },
            [callback]() {
                callback(JsonResponse::error("Database error", 500));
            },
            std::string(uuidStr), code, name, type, parentId, description
        );
    }
}

void AccountController::list(const drogon::HttpRequestPtr &req,
                            std::function<void(const drogon::HttpResponsePtr &)> &&callback)
{
    auto client = DbManager::getClient();
    if (!client)
    {
        callback(JsonResponse::error("Database connection failed", 500));
        return;
    }

    std::string query = "SELECT id, code, name, type, parent_id, description, is_active FROM accounts ORDER BY code";
    
    client->execSqlAsync(
        query,
        [callback](const drogon::orm::Result &result) {
            Json::Value accounts(Json::arrayValue);
            for (size_t i = 0; i < result.size(); ++i)
            {
                Json::Value account;
                account["id"] = result[i]["id"].as<std::string>();
                account["code"] = result[i]["code"].as<std::string>();
                account["name"] = result[i]["name"].as<std::string>();
                account["type"] = result[i]["type"].as<std::string>();
                if (!result[i]["parent_id"].isNull())
                    account["parent_id"] = result[i]["parent_id"].as<std::string>();
                if (!result[i]["description"].isNull())
                    account["description"] = result[i]["description"].as<std::string>();
                account["is_active"] = result[i]["is_active"].as<bool>();
                accounts.append(account);
            }
            callback(JsonResponse::success(accounts));
        },
        [callback]() {
            callback(JsonResponse::error("Database error", 500));
        }
    );
}

void AccountController::getById(const drogon::HttpRequestPtr &req,
                               std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                               const std::string &id)
{
    auto client = DbManager::getClient();
    if (!client)
    {
        callback(JsonResponse::error("Database connection failed", 500));
        return;
    }

    std::string query = "SELECT id, code, name, type, parent_id, description, is_active FROM accounts WHERE id = $1";
    
    client->execSqlAsync(
        query,
        [callback](const drogon::orm::Result &result) {
            if (result.size() == 0)
            {
                callback(JsonResponse::notFound("Account not found"));
                return;
            }

            Json::Value account;
            account["id"] = result[0]["id"].as<std::string>();
            account["code"] = result[0]["code"].as<std::string>();
            account["name"] = result[0]["name"].as<std::string>();
            account["type"] = result[0]["type"].as<std::string>();
            if (!result[0]["parent_id"].isNull())
                account["parent_id"] = result[0]["parent_id"].as<std::string>();
            if (!result[0]["description"].isNull())
                account["description"] = result[0]["description"].as<std::string>();
            account["is_active"] = result[0]["is_active"].as<bool>();
            callback(JsonResponse::success(account));
        },
        [callback]() {
            callback(JsonResponse::error("Database error", 500));
        },
        id
    );
}

void AccountController::update(const drogon::HttpRequestPtr &req,
                              std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                              const std::string &id)
{
    auto json = req->getJsonObject();
    if (!json)
    {
        callback(JsonResponse::error("Invalid request body"));
        return;
    }

    auto client = DbManager::getClient();
    if (!client)
    {
        callback(JsonResponse::error("Database connection failed", 500));
        return;
    }

    // Extract values from JSON
    std::string name = json->isMember("name") ? (*json)["name"].asString() : "";
    std::string type = json->isMember("type") ? (*json)["type"].asString() : "";
    std::string parentId = json->isMember("parent_id") ? (*json)["parent_id"].asString() : "";
    std::string description = json->isMember("description") ? (*json)["description"].asString() : "";
    bool hasIsActive = json->isMember("is_active");
    bool isActive = hasIsActive ? (*json)["is_active"].asBool() : false;

    // Build update query with only provided fields
    std::vector<std::string> updates;
    std::vector<std::string> params;
    int paramNum = 1;

    if (!name.empty()) {
        updates.push_back("name = $" + std::to_string(paramNum++));
        params.push_back(name);
    }
    if (!type.empty()) {
        updates.push_back("type = $" + std::to_string(paramNum++) + "::account_type");
        params.push_back(type);
    }
    if (json->isMember("parent_id")) {
        updates.push_back("parent_id = NULLIF($" + std::to_string(paramNum++) + ", '')");
        params.push_back(parentId);
    }
    if (json->isMember("description")) {
        updates.push_back("description = $" + std::to_string(paramNum++));
        params.push_back(description);
    }
    if (hasIsActive) {
        updates.push_back("is_active = $" + std::to_string(paramNum++));
        params.push_back(isActive ? "true" : "false");
    }

    if (updates.empty()) {
        callback(JsonResponse::error("No fields to update"));
        return;
    }

    std::string updateClause = std::accumulate(updates.begin(), updates.end(), std::string(),
        [](const std::string &a, const std::string &b) {
            return a.empty() ? b : a + ", " + b;
        });

    std::string query = "UPDATE accounts SET " + updateClause + " WHERE id = $" + std::to_string(paramNum) + " RETURNING id, code, name, type, parent_id, description, is_active";
    params.push_back(id);

    // For simplicity, update all fields using COALESCE
    std::string fullQuery = "UPDATE accounts SET name = COALESCE(NULLIF($1, ''), name), type = COALESCE(NULLIF($2, '')::account_type, type), parent_id = NULLIF($3, ''), description = COALESCE(NULLIF($4, ''), description), is_active = COALESCE($5::boolean, is_active) WHERE id = $6 RETURNING id, code, name, type, parent_id, description, is_active";
    
    client->execSqlAsync(
        fullQuery,
        [callback](const drogon::orm::Result &result) {
            if (result.size() == 0) {
                callback(JsonResponse::notFound("Account not found"));
                return;
            }
            Json::Value account;
            account["id"] = result[0]["id"].as<std::string>();
            account["code"] = result[0]["code"].as<std::string>();
            account["name"] = result[0]["name"].as<std::string>();
            account["type"] = result[0]["type"].as<std::string>();
            if (!result[0]["parent_id"].isNull())
                account["parent_id"] = result[0]["parent_id"].as<std::string>();
            if (!result[0]["description"].isNull())
                account["description"] = result[0]["description"].as<std::string>();
            account["is_active"] = result[0]["is_active"].as<bool>();
            callback(JsonResponse::success(account));
        },
        [callback]() {
            callback(JsonResponse::error("Database error", 500));
        },
        name.empty() ? std::string() : name,
        type.empty() ? std::string() : type,
        parentId,
        description,
        hasIsActive ? (isActive ? "true" : "false") : std::string(),
        id
    );
}

void AccountController::remove(const drogon::HttpRequestPtr &req,
                              std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                              const std::string &id)
{
    auto client = DbManager::getClient();
    if (!client)
    {
        callback(JsonResponse::error("Database connection failed", 500));
        return;
    }

    std::string query = "DELETE FROM accounts WHERE id = $1 RETURNING id";
    
    client->execSqlAsync(
        query,
        [callback](const drogon::orm::Result &result) {
            if (result.size() == 0)
            {
                callback(JsonResponse::notFound("Account not found"));
                return;
            }
            callback(JsonResponse::success());
        },
        [callback]() {
            callback(JsonResponse::error("Database error", 500));
        },
        id
    );
}

