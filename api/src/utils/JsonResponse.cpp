#include "utils/JsonResponse.h"
#include <drogon/HttpTypes.h>

namespace JsonResponse
{
    void addCorsHeaders(drogon::HttpResponsePtr &resp)
    {
        resp->addHeader("Access-Control-Allow-Origin", "*");
        resp->addHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS, PATCH");
        resp->addHeader("Access-Control-Allow-Headers", "Content-Type, Authorization, X-Requested-With");
    }

    drogon::HttpResponsePtr success(const Json::Value &data)
    {
        Json::Value response;
        response["success"] = true;
        if (!data.isNull())
        {
            response["data"] = data;
        }
        auto resp = drogon::HttpResponse::newHttpJsonResponse(response);
        addCorsHeaders(resp);
        return resp;
    }

    drogon::HttpResponsePtr error(const std::string &message, int statusCode)
    {
        Json::Value response;
        response["success"] = false;
        response["error"] = message;
        auto resp = drogon::HttpResponse::newHttpJsonResponse(response);
        resp->setStatusCode(static_cast<drogon::HttpStatusCode>(statusCode));
        addCorsHeaders(resp);
        return resp;
    }

    drogon::HttpResponsePtr unauthorized(const std::string &message)
    {
        return error(message, 401);
    }

    drogon::HttpResponsePtr notFound(const std::string &message)
    {
        return error(message, 404);
    }
}

