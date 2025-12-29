#include "controllers/JournalEntryController.h"
#include "database/DbManager.h"
#include "utils/JsonResponse.h"
#include <drogon/orm/DbClient.h>
#include <random>
#include <stdexcept>
#include <json/json.h>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <thread>
#include <chrono>

void JournalEntryController::create(const drogon::HttpRequestPtr &req,
                                     std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                                     const std::string &id)
{
    auto json = req->getJsonObject();
    if (!json || !json->isMember("date") || !json->isMember("description") || !json->isMember("lines"))
    {
        callback(JsonResponse::error("Missing required fields: date, description, lines"));
        return;
    }

    std::string date = (*json)["date"].asString();
    std::string description = (*json)["description"].asString();
    auto lines = (*json)["lines"];

    if (!lines.isArray() || lines.size() < 2)
    {
        callback(JsonResponse::error("Journal entry must have at least 2 lines"));
        return;
    }

    // Validate balance (debits = credits)
    double totalDebit = 0.0;
    double totalCredit = 0.0;
    for (const auto &line : lines)
    {
        if (!line.isMember("account_id") || (!line.isMember("debit") && !line.isMember("credit")))
        {
            callback(JsonResponse::error("Each line must have account_id and either debit or credit"));
            return;
        }
        if (line.isMember("debit"))
            totalDebit += line["debit"].asDouble();
        if (line.isMember("credit"))
            totalCredit += line["credit"].asDouble();
    }

    if (std::abs(totalDebit - totalCredit) > 0.01)
    {
        callback(JsonResponse::error("Journal entry must balance: debits must equal credits"));
        return;
    }

    std::string userId = req->getHeader("X-User-Id");

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

    // Generate entry number
    std::time_t now = std::time(nullptr);
    std::tm* timeinfo = std::localtime(&now);
    char dateStr[9];
    std::strftime(dateStr, sizeof(dateStr), "%Y%m%d", timeinfo);
    std::ostringstream entryNum;
    entryNum << "JE-" << dateStr << "-" << uuidStr.substr(0, 8);

    auto client = DbManager::getClient();
    if (!client)
    {
        callback(JsonResponse::error("Database connection failed", 500));
        return;
    }

    // Create journal entry
    std::string entryQuery = "INSERT INTO journal_entries (id, entry_number, date, description, created_by) VALUES ($1, $2, $3::date, $4, $5) RETURNING id";
    
    client->execSqlAsync(
        entryQuery,
        [callback, lines, userId, client](const drogon::orm::Result &result) {
            if (result.size() == 0)
            {
                callback(JsonResponse::error("Failed to create journal entry", 500));
                return;
            }

            std::string entryId = result[0]["id"].as<std::string>();

            // Create journal entry lines
            int lineOrder = 0;

            for (const auto &line : lines)
            {
                // Generate UUID for line
                std::random_device lineRd;
                std::mt19937 lineGen(lineRd());
                std::uniform_int_distribution<> lineDis(0, 15);
                std::string lineUuidStr = "xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx";
                for (char &c : lineUuidStr) {
                    if (c == 'x') {
                        c = "0123456789abcdef"[lineDis(lineGen)];
                    } else if (c == 'y') {
                        c = "89ab"[lineDis(lineGen) % 4];
                    }
                }

                std::string accountId = line["account_id"].asString();
                double debit = line.isMember("debit") ? line["debit"].asDouble() : 0.0;
                double credit = line.isMember("credit") ? line["credit"].asDouble() : 0.0;
                std::string lineDesc = line.isMember("description") ? line["description"].asString() : "";

                std::string lineQuery = "INSERT INTO journal_entry_lines (id, journal_entry_id, account_id, debit, credit, description, line_order) VALUES ($1, $2, $3, $4, $5, $6, $7)";
                
                int currentOrder = lineOrder++;
                client->execSqlAsync(
                    lineQuery,
                    [](const drogon::orm::Result &) {},
                    [callback]() {
                        callback(JsonResponse::error("Failed to create journal entry line", 500));
                    },
                    lineUuidStr, entryId, accountId, debit, credit, lineDesc, currentOrder
                );
            }

            // Wait a bit for async inserts to complete, then fetch
            // In production, you'd want to use transactions or a better async pattern
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            
            // Fetch complete journal entry with lines
            std::string fetchQuery = R"(
                SELECT 
                    je.id, je.entry_number, je.date, je.description, je.created_at,
                    COALESCE(json_agg(
                        json_build_object(
                            'id', jel.id,
                            'account_id', jel.account_id,
                            'debit', jel.debit,
                            'credit', jel.credit,
                            'description', jel.description,
                            'line_order', jel.line_order
                        ) ORDER BY jel.line_order
                    ) FILTER (WHERE jel.id IS NOT NULL), '[]'::json) as lines
                FROM journal_entries je
                LEFT JOIN journal_entry_lines jel ON je.id = jel.journal_entry_id
                WHERE je.id = $1
                GROUP BY je.id, je.entry_number, je.date, je.description, je.created_at
            )";

            client->execSqlAsync(
                fetchQuery,
                [callback](const drogon::orm::Result &result) {
                    if (result.size() > 0)
                    {
                        Json::Value entry;
                        entry["id"] = result[0]["id"].as<std::string>();
                        entry["entry_number"] = result[0]["entry_number"].as<std::string>();
                        entry["date"] = result[0]["date"].as<std::string>();
                        entry["description"] = result[0]["description"].as<std::string>();
                        entry["created_at"] = result[0]["created_at"].as<std::string>();
                        // Parse lines JSON
                        std::string linesJson = result[0]["lines"].as<std::string>();
                        Json::Reader reader;
                        Json::Value lines;
                        if (reader.parse(linesJson, lines))
                        {
                            entry["lines"] = lines;
                        }
                        callback(JsonResponse::success(entry));
                    }
                    else
                    {
                        callback(JsonResponse::error("Failed to retrieve created journal entry", 500));
                    }
                },
                [callback]() {
                    callback(JsonResponse::error("Database error", 500));
                },
                entryId
            );
        },
                [callback]() {
                    callback(JsonResponse::error("Database error", 500));
                },
        std::string(uuidStr), entryNum.str(), date, description, userId
    );
}

void JournalEntryController::list(const drogon::HttpRequestPtr &req,
                                   std::function<void(const drogon::HttpResponsePtr &)> &&callback)
{
    auto client = DbManager::getClient();
    if (!client)
    {
        callback(JsonResponse::error("Database connection failed", 500));
        return;
    }

    std::string query = R"(
        SELECT 
            je.id, je.entry_number, je.date, je.description, je.created_at,
            COALESCE(json_agg(
                json_build_object(
                    'id', jel.id,
                    'account_id', jel.account_id,
                    'debit', jel.debit,
                    'credit', jel.credit,
                    'description', jel.description,
                    'line_order', jel.line_order
                ) ORDER BY jel.line_order
            ) FILTER (WHERE jel.id IS NOT NULL), '[]'::json) as lines
        FROM journal_entries je
        LEFT JOIN journal_entry_lines jel ON je.id = jel.journal_entry_id
        GROUP BY je.id, je.entry_number, je.date, je.description, je.created_at
        ORDER BY je.date DESC, je.created_at DESC
    )";

    client->execSqlAsync(
        query,
        [callback](const drogon::orm::Result &result) {
            Json::Value entries(Json::arrayValue);
            Json::Reader reader;
            
            for (size_t i = 0; i < result.size(); ++i)
            {
                Json::Value entry;
                entry["id"] = result[i]["id"].as<std::string>();
                entry["entry_number"] = result[i]["entry_number"].as<std::string>();
                entry["date"] = result[i]["date"].as<std::string>();
                entry["description"] = result[i]["description"].as<std::string>();
                entry["created_at"] = result[i]["created_at"].as<std::string>();
                
                std::string linesJson = result[i]["lines"].as<std::string>();
                Json::Value lines;
                if (reader.parse(linesJson, lines))
                {
                    entry["lines"] = lines;
                }
                entries.append(entry);
            }
            callback(JsonResponse::success(entries));
        },
        [callback]() {
            callback(JsonResponse::error("Database error", 500));
        }
    );
}

void JournalEntryController::getById(const drogon::HttpRequestPtr &req,
                                     std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                                     const std::string &id)
{
    auto client = DbManager::getClient();
    if (!client)
    {
        callback(JsonResponse::error("Database connection failed", 500));
        return;
    }

    std::string query = R"(
        SELECT 
            je.id, je.entry_number, je.date, je.description, je.created_at,
            COALESCE(json_agg(
                json_build_object(
                    'id', jel.id,
                    'account_id', jel.account_id,
                    'debit', jel.debit,
                    'credit', jel.credit,
                    'description', jel.description,
                    'line_order', jel.line_order
                ) ORDER BY jel.line_order
            ) FILTER (WHERE jel.id IS NOT NULL), '[]'::json) as lines
        FROM journal_entries je
        LEFT JOIN journal_entry_lines jel ON je.id = jel.journal_entry_id
        WHERE je.id = $1
        GROUP BY je.id, je.entry_number, je.date, je.description, je.created_at
    )";

    client->execSqlAsync(
        query,
        [callback](const drogon::orm::Result &result) {
            if (result.size() == 0)
            {
                callback(JsonResponse::notFound("Journal entry not found"));
                return;
            }

            Json::Value entry;
            entry["id"] = result[0]["id"].as<std::string>();
            entry["entry_number"] = result[0]["entry_number"].as<std::string>();
            entry["date"] = result[0]["date"].as<std::string>();
            entry["description"] = result[0]["description"].as<std::string>();
            entry["created_at"] = result[0]["created_at"].as<std::string>();
            
            Json::Reader reader;
            std::string linesJson = result[0]["lines"].as<std::string>();
            Json::Value lines;
            if (reader.parse(linesJson, lines))
            {
                entry["lines"] = lines;
            }
            callback(JsonResponse::success(entry));
        },
                [callback]() {
                    callback(JsonResponse::error("Database error", 500));
                },
        id
    );
}

