#pragma once

#include <drogon/HttpController.h>
#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>
#include <json/json.h>
#include <jwt-cpp/jwt.h>
#include "middleware/CorsFilter.h"

class AuthController : public drogon::HttpController<AuthController>
{
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(AuthController::registerUser, "/api/auth/register", drogon::Post, "CorsFilter");
        ADD_METHOD_TO(AuthController::login, "/api/auth/login", drogon::Post, "CorsFilter");
        ADD_METHOD_TO(AuthController::refresh, "/api/auth/refresh", drogon::Post, "CorsFilter");
    METHOD_LIST_END

    void registerUser(const drogon::HttpRequestPtr &req,
                     std::function<void(const drogon::HttpResponsePtr &)> &&callback);

    void login(const drogon::HttpRequestPtr &req,
              std::function<void(const drogon::HttpResponsePtr &)> &&callback);

    void refresh(const drogon::HttpRequestPtr &req,
                std::function<void(const drogon::HttpResponsePtr &)> &&callback);

private:
    std::string generateToken(const std::string &userId, const std::string &username);
};

