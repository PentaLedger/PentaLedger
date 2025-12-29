#pragma once

#include <drogon/HttpResponse.h>
#include <json/json.h>

namespace JsonResponse
{
    drogon::HttpResponsePtr success(const Json::Value &data = Json::Value());
    drogon::HttpResponsePtr error(const std::string &message, int statusCode = 400);
    drogon::HttpResponsePtr unauthorized(const std::string &message = "Unauthorized");
    drogon::HttpResponsePtr notFound(const std::string &message = "Not found");
}

