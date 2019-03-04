cmake_minimum_required(VERSION 2.8.9)
project (s)
find_package (Threads)

add_executable(s main.cpp ../lib/udplink.c)
target_link_libraries (s ${CMAKE_THREAD_LIBS_INIT})
