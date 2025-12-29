cmake_minimum_required(VERSION 3.16)
project(PentaLedgerAPI)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(Drogon REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(PostgreSQL REQUIRED)

# Find or include jwt-cpp (header-only library)
find_path(JWT_CPP_INCLUDE_DIR jwt-cpp/jwt.h
    PATHS
    /usr/local/include
    /usr/include
)

if(NOT JWT_CPP_INCLUDE_DIR)
    message(WARNING "jwt-cpp not found, will try to download")
    include(FetchContent)
    FetchContent_Declare(
        jwt-cpp
        GIT_REPOSITORY https://github.com/Thalhammer/jwt-cpp.git
        GIT_TAG v0.7.2
    )
    FetchContent_MakeAvailable(jwt-cpp)
    set(JWT_CPP_INCLUDE_DIR ${jwt-cpp_SOURCE_DIR}/include)
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${JWT_CPP_INCLUDE_DIR})

# Source files
set(SOURCES
    src/main.cpp
    src/middleware/JwtAuth.cpp
    src/controllers/AuthController.cpp
    src/controllers/AccountController.cpp
    src/controllers/JournalEntryController.cpp
    src/controllers/ReportController.cpp
    src/database/DbManager.cpp
    src/utils/PasswordHasher.cpp
    src/utils/JsonResponse.cpp
)

# Header files
set(HEADERS
    include/middleware/JwtAuth.h
    include/controllers/AuthController.h
    include/controllers/AccountController.h
    include/controllers/JournalEntryController.h
    include/controllers/ReportController.h
    include/database/DbManager.h
    include/utils/PasswordHasher.h
    include/utils/JsonResponse.h
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link libraries
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    Drogon::Drogon
    OpenSSL::SSL
    OpenSSL::Crypto
    PostgreSQL::PostgreSQL
)

# Install target
install(TARGETS ${PROJECT_NAME} DESTINATION bin)

