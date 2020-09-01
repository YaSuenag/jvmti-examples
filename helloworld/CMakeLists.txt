cmake_minimum_required(VERSION 3.1)
project(HelloWorld C)

find_package(JNI REQUIRED)
find_package(Threads REQUIRED)

include_directories(${JNI_INCLUDE_DIRS})
add_library(helloworld SHARED helloworld.c)
target_link_libraries(helloworld PRIVATE Threads::Threads)
