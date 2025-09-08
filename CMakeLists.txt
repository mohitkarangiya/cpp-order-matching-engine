cmake_minimum_required(VERSION 3.20)
project(OrderMatchingEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROJECT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(src/core)

add_executable(MainProgram src/main.cpp)

target_link_libraries(MainProgram PRIVATE CoreLib)