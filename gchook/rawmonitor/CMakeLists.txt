cmake_minimum_required(VERSION 3.1)
project(GCHook CXX)

find_package(JNI REQUIRED)
find_package(Threads REQUIRED)

include_directories(${JNI_INCLUDE_DIRS})
add_library(gchook SHARED gchook)
target_link_libraries(gchook PRIVATE Threads::Threads)
