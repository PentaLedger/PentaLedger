#pragma once

#include <drogon/HttpController.h>
#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>
#include "middleware/CorsFilter.h"
#include "middleware/JwtAuth.h"

class ReportController : public drogon::HttpController<ReportController>
{
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(ReportController::balanceSheet, "/api/reports/balance-sheet", drogon::Get, "CorsFilter", "JwtAuth");
        ADD_METHOD_TO(ReportController::incomeStatement, "/api/reports/income-statement", drogon::Get, "CorsFilter", "JwtAuth");
    METHOD_LIST_END

    void balanceSheet(const drogon::HttpRequestPtr &req,
                     std::function<void(const drogon::HttpResponsePtr &)> &&callback);

    void incomeStatement(const drogon::HttpRequestPtr &req,
                        std::function<void(const drogon::HttpResponsePtr &)> &&callback);
};

