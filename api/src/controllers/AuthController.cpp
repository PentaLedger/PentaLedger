#include "controllers/AuthController.h"
#include "database/DbManager.h"
#include "utils/PasswordHasher.h"
#include "utils/JsonResponse.h"
#include <drogon/orm/DbClient.h>
#include <drogon/orm/Result.h>
#include <drogon/HttpTypes.h>
#include <stdexcept>
#include <jwt-cpp/jwt.h>
#include <random>
#include <sstream>
#include <iomanip>
#include <cstring>

void AuthController::registerUser(const drogon::HttpRequestPtr &req,
                                  std::function<void(const drogon::HttpResponsePtr &)> &&callback)
{
    auto json = req->getJsonObject();
    if (!json || !json->isMember("username") || !json->isMember("email") || !json->isMember("password"))
    {
        callback(JsonResponse::error("Missing required fields: username, email, password"));
        return;
    }

    std::string username = (*json)["username"].asString();
    std::string email = (*json)["email"].asString();
    std::string password = (*json)["password"].asString();

    if (password.length() < 8)
    {
        callback(JsonResponse::error("Password must be at least 8 characters"));
        return;
    }

    std::string passwordHash = PasswordHasher::hash(password);

    auto client = DbManager::getClient();
    if (!client)
    {
        callback(JsonResponse::error("Database connection failed", 500));
        return;
    }

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

    std::string query = "INSERT INTO users (id, username, email, password_hash) VALUES ($1, $2, $3, $4) RETURNING id, username, email";
    
    client->execSqlAsync(
        query,
        [callback](const drogon::orm::Result &result) {
            if (result.size() > 0)
            {
                Json::Value user;
                user["id"] = result[0]["id"].as<std::string>();
                user["username"] = result[0]["username"].as<std::string>();
                user["email"] = result[0]["email"].as<std::string>();
                callback(JsonResponse::success(user));
            }
            else
            {
                callback(JsonResponse::error("Registration failed", 500));
            }
        },
        [callback]() {
            callback(JsonResponse::error("Registration failed", 500));
        },
        std::string(uuidStr), username, email, passwordHash
    );
}

void AuthController::login(const drogon::HttpRequestPtr &req,
                          std::function<void(const drogon::HttpResponsePtr &)> &&callback)
{
    auto json = req->getJsonObject();
    if (!json || !json->isMember("username") || !json->isMember("password"))
    {
        callback(JsonResponse::error("Missing username or password"));
        return;
    }

    std::string username = (*json)["username"].asString();
    std::string password = (*json)["password"].asString();

    auto client = DbManager::getClient();
    if (!client)
    {
        callback(JsonResponse::error("Database connection failed", 500));
        return;
    }

    std::string query = "SELECT id, username, email, password_hash FROM users WHERE username = $1";
    
    client->execSqlAsync(
        query,
        [callback, password](const drogon::orm::Result &result) {
            if (result.size() == 0)
            {
                callback(JsonResponse::unauthorized("Invalid username or password"));
                return;
            }

            std::string userId = result[0]["id"].as<std::string>();
            std::string username = result[0]["username"].as<std::string>();
            std::string storedHash = result[0]["password_hash"].as<std::string>();

            if (!PasswordHasher::verify(password, storedHash))
            {
                callback(JsonResponse::unauthorized("Invalid username or password"));
                return;
            }

            AuthController controller;
            std::string token = controller.generateToken(userId, username);

            Json::Value response;
            response["token"] = token;
            response["user"] = Json::Value();
            response["user"]["id"] = userId;
            response["user"]["username"] = username;
            response["user"]["email"] = result[0]["email"].as<std::string>();

            callback(JsonResponse::success(response));
        },
        [callback]() {
            callback(JsonResponse::error("Database error", 500));
        },
        username
    );
}

void AuthController::refresh(const drogon::HttpRequestPtr &req,
                            std::function<void(const drogon::HttpResponsePtr &)> &&callback)
{
    // Extract token from Authorization header
    std::string authHeader = req->getHeader("Authorization");
    if (authHeader.empty() || authHeader.find("Bearer ") != 0)
    {
        callback(JsonResponse::unauthorized("Missing or invalid token"));
        return;
    }

    std::string token = authHeader.substr(7);
    
    try
    {
        auto decoded = jwt::decode(token);
        auto verifier = jwt::verify()
            .allow_algorithm(jwt::algorithm::hs256{std::getenv("JWT_SECRET") ? std::getenv("JWT_SECRET") : "default-secret-key-change-in-production"})
            .with_issuer("pentaledger");
        
        verifier.verify(decoded);
        
        auto userIdClaim = decoded.get_payload_claim("user_id");
        auto usernameClaim = decoded.get_payload_claim("username");
        std::string userId = userIdClaim.as_string();
        std::string username = usernameClaim.as_string();

        std::string newToken = generateToken(userId, username);

        Json::Value response;
        response["token"] = newToken;
        callback(JsonResponse::success(response));
    }
    catch (const std::exception &e)
    {
        callback(JsonResponse::unauthorized("Invalid token"));
    }
}

std::string AuthController::generateToken(const std::string &userId, const std::string &username)
{
    std::string secret = std::getenv("JWT_SECRET") ? std::getenv("JWT_SECRET") : "default-secret-key-change-in-production";
    
    auto token = jwt::create()
        .set_issuer("pentaledger")
        .set_type("JWT")
        .set_payload_claim("user_id", jwt::claim(userId))
        .set_payload_claim("username", jwt::claim(username))
        .set_issued_at(std::chrono::system_clock::now())
        .set_expires_at(std::chrono::system_clock::now() + std::chrono::hours{24})
        .sign(jwt::algorithm::hs256{secret});

    return token;
}

