cmake_minimum_required(VERSION 3.13)
project(socket_client_4nios)

set(CMAKE_CXX_STANDARD 11)

add_executable(socket_client_4nios main.cpp)
target_link_libraries(socket_client_4nios -lpthread)