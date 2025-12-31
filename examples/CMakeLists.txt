cmake_minimum_required(VERSION 3.15)

add_executable(pentaledger_example basic_usage.cpp)
target_link_libraries(pentaledger_example PRIVATE pentaledger)

