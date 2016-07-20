cmake_minimum_required(VERSION 3.5)
project(HttpServer)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp server.cpp server.h acceptor.cpp acceptor.h helper.cpp helper.h util.h util.cpp responser.cpp responser.h)
add_executable(HttpServer ${SOURCE_FILES})
add_definitions(-DCORES=4)
target_link_libraries(HttpServer ev)