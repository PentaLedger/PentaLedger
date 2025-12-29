#include "utils/JsonResponse.h"
#include <drogon/HttpTypes.h>

namespace JsonResponse
{
    drogon::HttpResponsePtr success(const Json::Value &data)
    {
        Json::Value response;
        response["success"] = true;
        if (!data.isNull())
        {
            response["data"] = data;
        }
        return drogon::HttpResponse::newHttpJsonResponse(response);
    }

    drogon::HttpResponsePtr error(const std::string &message, int statusCode)
    {
        Json::Value response;
        response["success"] = false;
        response["error"] = message;
        auto resp = drogon::HttpResponse::newHttpJsonResponse(response);
        resp->setStatusCode(static_cast<drogon::HttpStatusCode>(statusCode));
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

