#pragma once

#include <drogon/HttpController.h>
#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>
#include <drogon/HttpFilter.h>
#include "middleware/JwtAuth.h"

class AccountController : public drogon::HttpController<AccountController>
{
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(AccountController::create, "/api/accounts", drogon::Post, "JwtAuth");
        ADD_METHOD_TO(AccountController::list, "/api/accounts", drogon::Get, "JwtAuth");
        ADD_METHOD_TO(AccountController::getById, "/api/accounts/{id}", drogon::Get, "JwtAuth");
        ADD_METHOD_TO(AccountController::update, "/api/accounts/{id}", drogon::Put, "JwtAuth");
        ADD_METHOD_TO(AccountController::remove, "/api/accounts/{id}", drogon::Delete, "JwtAuth");
    METHOD_LIST_END

    void create(const drogon::HttpRequestPtr &req,
               std::function<void(const drogon::HttpResponsePtr &)> &&callback,
               const std::string &id = "");

    void list(const drogon::HttpRequestPtr &req,
             std::function<void(const drogon::HttpResponsePtr &)> &&callback);

    void getById(const drogon::HttpRequestPtr &req,
                std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                const std::string &id);

    void update(const drogon::HttpRequestPtr &req,
               std::function<void(const drogon::HttpResponsePtr &)> &&callback,
               const std::string &id);

    void remove(const drogon::HttpRequestPtr &req,
               std::function<void(const drogon::HttpResponsePtr &)> &&callback,
               const std::string &id);
};

