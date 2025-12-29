#pragma once

#include <drogon/HttpFilter.h>
#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>
#include <drogon/utils/FunctionTraits.h>
#include <jwt-cpp/jwt.h>
#include <string>

class JwtAuth : public drogon::HttpFilter<JwtAuth>
{
public:
    void doFilter(const drogon::HttpRequestPtr &req,
                  drogon::FilterCallback &&fcb,
                  drogon::FilterChainCallback &&fccb) override;

    static std::string getSecret();
    static std::string extractToken(const drogon::HttpRequestPtr &req);
    static bool validateToken(const std::string &token);
    static std::string getUserIdFromToken(const std::string &token);
};

