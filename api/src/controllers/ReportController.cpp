#include "controllers/ReportController.h"
#include "database/DbManager.h"
#include "utils/JsonResponse.h"
#include <drogon/orm/DbClient.h>
#include <json/json.h>
#include <stdexcept>

void ReportController::balanceSheet(const drogon::HttpRequestPtr &req,
                                    std::function<void(const drogon::HttpResponsePtr &)> &&callback)
{
    auto client = DbManager::getClient();
    if (!client)
    {
        callback(JsonResponse::error("Database connection failed", 500));
        return;
    }

    // Get date parameter (default to today)
    std::string date = req->getParameter("date");
    if (date.empty())
    {
        // Use current date
        date = "CURRENT_DATE";
    }

    std::string query = R"(
        WITH account_balances AS (
            SELECT 
                a.id,
                a.code,
                a.name,
                a.type,
                COALESCE(SUM(jel.debit - jel.credit), 0) as balance
            FROM accounts a
            LEFT JOIN journal_entry_lines jel ON a.id = jel.account_id
            LEFT JOIN journal_entries je ON jel.journal_entry_id = je.id
            WHERE a.type IN ('ASSET', 'LIABILITY', 'EQUITY')
                AND (je.date IS NULL OR je.date <= $1::date)
            GROUP BY a.id, a.code, a.name, a.type
        )
        SELECT 
            type,
            json_agg(
                json_build_object(
                    'code', code,
                    'name', name,
                    'balance', balance
                ) ORDER BY code
            ) as accounts,
            SUM(balance) as total
        FROM account_balances
        WHERE balance != 0
        GROUP BY type
        ORDER BY 
            CASE type
                WHEN 'ASSET' THEN 1
                WHEN 'LIABILITY' THEN 2
                WHEN 'EQUITY' THEN 3
            END
    )";

    client->execSqlAsync(
        query,
        [callback](const drogon::orm::Result &result) {
            Json::Value report;
            report["type"] = "balance_sheet";
            Json::Value sections(Json::arrayValue);
            double totalAssets = 0.0;
            double totalLiabilities = 0.0;
            double totalEquity = 0.0;

            for (size_t i = 0; i < result.size(); ++i)
            {
                Json::Value section;
                std::string type = result[i]["type"].as<std::string>();
                section["type"] = type;
                
                Json::Reader reader;
                std::string accountsJson = result[i]["accounts"].as<std::string>();
                Json::Value accounts;
                if (reader.parse(accountsJson, accounts))
                {
                    section["accounts"] = accounts;
                }
                
                double total = result[i]["total"].as<double>();
                section["total"] = total;
                sections.append(section);

                if (type == "ASSET")
                    totalAssets = total;
                else if (type == "LIABILITY")
                    totalLiabilities = total;
                else if (type == "EQUITY")
                    totalEquity = total;
            }

            report["sections"] = sections;
            report["total_assets"] = totalAssets;
            report["total_liabilities"] = totalLiabilities;
            report["total_equity"] = totalEquity;
            report["balance"] = std::abs(totalAssets - (totalLiabilities + totalEquity)) < 0.01;

            callback(JsonResponse::success(report));
        },
        [callback]() {
            callback(JsonResponse::error("Database error", 500));
        },
        date == "CURRENT_DATE" ? "CURRENT_DATE" : date
    );
}

void ReportController::incomeStatement(const drogon::HttpRequestPtr &req,
                                       std::function<void(const drogon::HttpResponsePtr &)> &&callback)
{
    auto client = DbManager::getClient();
    if (!client)
    {
        callback(JsonResponse::error("Database connection failed", 500));
        return;
    }

    // Get date range parameters
    std::string startDate = req->getParameter("start_date");
    std::string endDate = req->getParameter("end_date");
    
    if (startDate.empty())
    {
        // Default to start of current year
        startDate = "DATE_TRUNC('year', CURRENT_DATE)";
    }
    if (endDate.empty())
    {
        endDate = "CURRENT_DATE";
    }

    std::string query = R"(
        WITH account_balances AS (
            SELECT 
                a.id,
                a.code,
                a.name,
                a.type,
                COALESCE(SUM(jel.debit - jel.credit), 0) as balance
            FROM accounts a
            LEFT JOIN journal_entry_lines jel ON a.id = jel.account_id
            LEFT JOIN journal_entries je ON jel.journal_entry_id = je.id
            WHERE a.type IN ('REVENUE', 'EXPENSE')
                AND je.date IS NOT NULL
                AND je.date >= $1::date
                AND je.date <= $2::date
            GROUP BY a.id, a.code, a.name, a.type
        )
        SELECT 
            type,
            json_agg(
                json_build_object(
                    'code', code,
                    'name', name,
                    'balance', balance
                ) ORDER BY code
            ) as accounts,
            SUM(balance) as total
        FROM account_balances
        WHERE balance != 0
        GROUP BY type
        ORDER BY 
            CASE type
                WHEN 'REVENUE' THEN 1
                WHEN 'EXPENSE' THEN 2
            END
    )";

    client->execSqlAsync(
        query,
        [callback](const drogon::orm::Result &result) {
            Json::Value report;
            report["type"] = "income_statement";
            Json::Value sections(Json::arrayValue);
            double totalRevenue = 0.0;
            double totalExpenses = 0.0;

            for (size_t i = 0; i < result.size(); ++i)
            {
                Json::Value section;
                std::string type = result[i]["type"].as<std::string>();
                section["type"] = type;
                
                Json::Reader reader;
                std::string accountsJson = result[i]["accounts"].as<std::string>();
                Json::Value accounts;
                if (reader.parse(accountsJson, accounts))
                {
                    section["accounts"] = accounts;
                }
                
                double total = result[i]["total"].as<double>();
                section["total"] = total;
                sections.append(section);

                if (type == "REVENUE")
                    totalRevenue = total;
                else if (type == "EXPENSE")
                    totalExpenses = total;
            }

            report["sections"] = sections;
            report["total_revenue"] = totalRevenue;
            report["total_expenses"] = totalExpenses;
            report["net_income"] = totalRevenue - totalExpenses;

            callback(JsonResponse::success(report));
        },
        [callback]() {
            callback(JsonResponse::error("Database error", 500));
        },
        startDate == "DATE_TRUNC('year', CURRENT_DATE)" ? "DATE_TRUNC('year', CURRENT_DATE)" : startDate,
        endDate == "CURRENT_DATE" ? "CURRENT_DATE" : endDate
    );
}

