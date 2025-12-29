#pragma once

#include <drogon/HttpController.h>
#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>
#include "middleware/CorsFilter.h"
#include "middleware/JwtAuth.h"

class JournalEntryController : public drogon::HttpController<JournalEntryController>
{
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(JournalEntryController::create, "/api/journal-entries", drogon::Post, "CorsFilter", "JwtAuth");
        ADD_METHOD_TO(JournalEntryController::list, "/api/journal-entries", drogon::Get, "CorsFilter", "JwtAuth");
        ADD_METHOD_TO(JournalEntryController::getById, "/api/journal-entries/{id}", drogon::Get, "CorsFilter", "JwtAuth");
    METHOD_LIST_END

    void create(const drogon::HttpRequestPtr &req,
               std::function<void(const drogon::HttpResponsePtr &)> &&callback,
               const std::string &id = "");

    void list(const drogon::HttpRequestPtr &req,
             std::function<void(const drogon::HttpResponsePtr &)> &&callback);

    void getById(const drogon::HttpRequestPtr &req,
                std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                const std::string &id);
};

