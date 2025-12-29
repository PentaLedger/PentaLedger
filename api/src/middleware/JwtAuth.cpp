#include "middleware/JwtAuth.h"
#include <drogon/HttpResponse.h>
#include <drogon/HttpTypes.h>
#include <jwt-cpp/jwt.h>
#include <iostream>

void JwtAuth::doFilter(const drogon::HttpRequestPtr &req,
                      drogon::FilterCallback &&fcb,
                      drogon::FilterChainCallback &&fccb)
{
    std::string token = extractToken(req);
    
    if (token.empty() || !validateToken(token))
    {
        Json::Value json;
        json["error"] = "Unauthorized";
        json["message"] = "Invalid or missing token";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(json);
        resp->setStatusCode(drogon::k401Unauthorized);
        // Add CORS headers to error response
        resp->addHeader("Access-Control-Allow-Origin", "*");
        resp->addHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS, PATCH");
        resp->addHeader("Access-Control-Allow-Headers", "Content-Type, Authorization, X-Requested-With");
        fcb(resp);
        return;
    }

    // Add user ID to request for use in controllers (store in custom header)
    std::string userId = getUserIdFromToken(token);
    req->addHeader("X-User-Id", userId);
    
    fccb();
}

std::string JwtAuth::getSecret()
{
    const char *secret = std::getenv("JWT_SECRET");
    if (secret == nullptr)
    {
        return "default-secret-key-change-in-production";
    }
    return std::string(secret);
}

std::string JwtAuth::extractToken(const drogon::HttpRequestPtr &req)
{
    std::string authHeader = req->getHeader("Authorization");
    if (authHeader.empty() || authHeader.find("Bearer ") != 0)
    {
        return "";
    }
    return authHeader.substr(7); // Remove "Bearer " prefix
}

bool JwtAuth::validateToken(const std::string &token)
{
    try
    {
        auto decoded = jwt::decode(token);
        auto verifier = jwt::verify()
            .allow_algorithm(jwt::algorithm::hs256{getSecret()})
            .with_issuer("pentaledger");
        
        verifier.verify(decoded);
        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Token validation error: " << e.what() << std::endl;
        return false;
    }
}

std::string JwtAuth::getUserIdFromToken(const std::string &token)
{
    try
    {
        auto decoded = jwt::decode(token);
        try
        {
            auto claim = decoded.get_payload_claim("user_id");
            return claim.as_string();
        }
        catch (...)
        {
            // Claim doesn't exist
            return "";
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error extracting user ID from token: " << e.what() << std::endl;
    }
    return "";
}

