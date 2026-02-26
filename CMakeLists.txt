cmake_minimum_required(VERSION 3.15)
project(PentaLedger VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Path to the submodule
set(STDUUID_DIR ${CMAKE_SOURCE_DIR}/third_party/stduuid)
# Include directories
include_directories(include)
include_directories(${STDUUID_DIR}/include)

# Source files
set(SOURCES
    src/storage/data_file.cpp
    src/btree/btree_file.cpp
)

# Header files
set(HEADERS
    include/pentaledger/types.hpp
    include/pentaledger/record_pointer.hpp
    include/pentaledger/data_file.hpp
    include/pentaledger/data_file_header.hpp
    include/pentaledger/btree_file.hpp
    include/pentaledger/btree_file_header.hpp
)

# Create library
add_library(pentaledger STATIC ${SOURCES} ${HEADERS})

target_include_directories(pentaledger PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Enable testing
enable_testing()

# Add test subdirectory
add_subdirectory(tests)
add_subdirectory(apps)
add_subdirectory(server)


# Add examples subdirectory
#add_subdirectory(examples)

# Installation
install(TARGETS pentaledger
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/pentaledger
    DESTINATION include
    FILES_MATCHING PATTERN "*.hpp"
)

