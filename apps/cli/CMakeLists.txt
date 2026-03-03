cmake_minimum_required(VERSION 3.15)

# libpqxx requires libpq (PostgreSQL client). Install via:
#   macOS (Homebrew): brew install libpq
#     If keg-only: cmake -DPostgreSQL_ROOT=/opt/homebrew/opt/libpq ..
#   Ubuntu: apt install libpq-dev
#   Fedora: dnf install libpq-devel
include(FetchContent)
FetchContent_Declare(
    libpqxx
    GIT_REPOSITORY https://github.com/jtv/libpqxx.git
    GIT_TAG 7.10.5
)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(libpqxx)

add_executable(pentaledger_cli main.cpp)

target_link_libraries(pentaledger_cli
    PRIVATE
    libpqxx::pqxx
)
