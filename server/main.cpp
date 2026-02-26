#include <cstdlib>
#include <string>
#include <httplib.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace {

std::string get_address(int argc, char* argv[]) {
    std::string address = "0.0.0.0";
    const char* env = std::getenv("PENTALEDGER_ADDRESS");
    if (env && env[0] != '\0') {
        address = env;
    }
    for (int i = 1; i < argc - 1; ++i) {
        if (std::string(argv[i]) == "--address") {
            address = argv[i + 1];
            break;
        }
    }
    return address;
}

int get_port(int argc, char* argv[]) {
    int port = 8080;
    const char* env = std::getenv("PENTALEDGER_PORT");
    if (env && env[0] != '\0') {
        try {
            port = std::stoi(env);
        } catch (...) {
            // keep default on parse error
        }
    }
    for (int i = 1; i < argc - 1; ++i) {
        if (std::string(argv[i]) == "--port") {
            try {
                port = std::stoi(argv[i + 1]);
            } catch (...) {
                // keep previous value on parse error
            }
            break;
        }
    }
    return port;
}

}  // namespace

int main(int argc, char* argv[]) {
    std::string address = get_address(argc, argv);
    int port = get_port(argc, argv);

    spdlog::set_default_logger(spdlog::stdout_color_mt("server"));
    spdlog::set_level(spdlog::level::info);

    httplib::Server svr;

    svr.Get("/", [](const httplib::Request&, httplib::Response& res) {
        res.set_content("{\"status\":\"ok\"}", "application/json");
    });

    svr.Get("/v0/healthcheck", [](const httplib::Request&, httplib::Response& res) {
        res.set_content("{\"status\":\"ok\"}", "application/json");
    });

    spdlog::info("Server starting on {}:{}", address, port);

    if (!svr.listen(address.c_str(), port)) {
        spdlog::error("Failed to listen on {}:{}", address, port);
        return 1;
    }

    spdlog::info("Server stopped");
    return 0;
}
