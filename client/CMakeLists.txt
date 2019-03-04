cmake_minimum_required(VERSION 2.8.9)
project (c)
find_package (Threads)

add_executable(c main.cpp ../lib/udplink.c)
target_link_libraries (c ${CMAKE_THREAD_LIBS_INIT})
