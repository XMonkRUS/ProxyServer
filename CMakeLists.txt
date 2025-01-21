cmake_minimum_required(VERSION 3.10)

project(ProxyServer)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
    src/main.cpp
    src/ProxyServer.cpp
    src/Logger.cpp
    src/PostgreSQLParser.cpp
)

add_executable(ProxyServer ${SOURCE_FILES})