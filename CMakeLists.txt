cmake_minimum_required(VERSION 3.5)
project(HttpServer)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp CrashHandler.cpp CrashHandler.h Acceptor.cpp Acceptor.h util.cpp util.h
        Helper.cpp Helper.h Responser.cpp Responser.h)
add_executable(HttpServer ${SOURCE_FILES})
target_link_libraries(HttpServer ev)