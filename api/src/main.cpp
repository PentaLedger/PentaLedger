#include <drogon/drogon.h>
#include "database/DbManager.h"
#include "middleware/JwtAuth.h"
#include "controllers/AuthController.h"
#include "controllers/AccountController.h"
#include "controllers/JournalEntryController.h"
#include "controllers/ReportController.h"
#include <iostream>
#include <cstdlib>

int main()
{
    // Get database connection string from environment
    std::string dbHost = std::getenv("DB_HOST") ? std::getenv("DB_HOST") : "localhost";
    std::string dbPort = std::getenv("DB_PORT") ? std::getenv("DB_PORT") : "5432";
    std::string dbName = std::getenv("DB_NAME") ? std::getenv("DB_NAME") : "pentaledger";
    std::string dbUser = std::getenv("DB_USER") ? std::getenv("DB_USER") : "pentaledger";
    std::string dbPassword = std::getenv("DB_PASSWORD") ? std::getenv("DB_PASSWORD") : "pentaledger_dev";

    std::string connectionString = "host=" + dbHost +
                                  " port=" + dbPort +
                                  " dbname=" + dbName +
                                  " user=" + dbUser +
                                  " password=" + dbPassword;

    // Initialize database connection
    DbManager::initialize(connectionString);

    // JWT middleware is auto-registered by Drogon when used in controllers

    // Get port from environment or use default
    std::string portStr = std::getenv("API_PORT") ? std::getenv("API_PORT") : "8080";
    uint16_t port = static_cast<uint16_t>(std::stoi(portStr));

    std::cout << "Starting PentaLedger API server on port " << port << std::endl;
    std::cout << "Database: " << dbHost << ":" << dbPort << "/" << dbName << std::endl;

    // Run the HTTP framework
    drogon::app()
        .setLogLevel(trantor::Logger::kInfo)
        .addListener("0.0.0.0", port)
        .run();

    return 0;
}

