cmake_minimum_required(VERSION 2.8)
project(HttpServer)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp server.cpp server.h acceptor.cpp acceptor.h helper.cpp helper.h util.h util.cpp responser.cpp responser.h)
add_executable(final ${SOURCE_FILES})
target_link_libraries(final ev)