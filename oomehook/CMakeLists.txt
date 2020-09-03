cmake_minimum_required(VERSION 3.1)
project(OOME CXX)

find_package(JNI REQUIRED)
find_package(Threads REQUIRED)

include_directories(${JNI_INCLUDE_DIRS})
add_library(oomehook SHARED oomehook)
target_link_libraries(oomehook PRIVATE Threads::Threads)
