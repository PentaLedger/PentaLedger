/*
 * PentaLedger CLI
 * Copyright (C) 2025 Joe Turner
 *
 * Command-line utility for connecting to the PentaLedger Neon PostgreSQL database.
 * Requires PENTALEDGER_DATABASE_URL environment variable.
 */

#include <cstdlib>
#include <iostream>
#include <string>

#include <pqxx/pqxx>

namespace {

const char* const ENV_DATABASE_URL = "PENTALEDGER_DATABASE_URL";

std::string get_database_url() {
    const char* url = std::getenv(ENV_DATABASE_URL);
    if (!url || url[0] == '\0') {
        std::cerr << "Error: " << ENV_DATABASE_URL << " is not set.\n"
                  << "Set it to your Neon PostgreSQL connection string, e.g.:\n"
                  << "  export " << ENV_DATABASE_URL
                  << "=\"postgresql://user:password@host/neondb?sslmode=require\"\n";
        std::exit(1);
    }
    return std::string(url);
}

}  // namespace

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    std::string url = get_database_url();

    try {
        pqxx::connection conn(url);

        if (!conn.is_open()) {
            std::cerr << "Error: Failed to open database connection.\n";
            return 1;
        }

        pqxx::work tx(conn);
        pqxx::row r = tx.exec1("SELECT 1 AS ok");
        tx.commit();

        std::cout << "Connected to Neon PostgreSQL successfully. (SELECT 1 = "
                  << r[0].as<int>() << ")\n";

        return 0;
    } catch (const pqxx::sql_error& e) {
        std::cerr << "SQL error: " << e.what() << "\n"
                  << "Query: " << e.query() << "\n";
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
}
