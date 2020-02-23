cmake_minimum_required(VERSION 2.8)
project(uki)
set(CMAKE_BUILD_TYPE Release)

file(GLOB SOURCES "src/*.c")
file(GLOB LIBSOURCES "src/*.c")
list(REMOVE_ITEM LIBSOURCES "src/main.c")

add_library(uki SHARED ${LIBSOURCES})

set(PROJECT_LINK_LIBS libuki.so)
link_directories(${CMAKE_BINARY_DIR})

add_executable(ukitest ${SOURCES})
target_link_libraries(ukitest ${PROJECT_LINK_LIBS})
