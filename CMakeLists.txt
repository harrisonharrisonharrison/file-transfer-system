cmake_minimum_required(VERSION 3.15)
project(FileTransferServer CXX)

# Enforce C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Create the executable from main.cpp
add_executable(server main.cpp)

# cpp-httplib requires threading support under the hood
find_package(Threads REQUIRED)
target_link_libraries(server PRIVATE Threads::Threads)

# Windows-specific networking library (Winsock2)
if(WIN32)
    target_link_libraries(server PRIVATE ws2_32)
endif()