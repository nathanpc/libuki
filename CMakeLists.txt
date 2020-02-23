### CMakeList.txt
### CMake definitions for the Uki shared library.
###
### Author: Nathan Campos <hi@nathancampos.me>

# Determine the minimum CMake version.
cmake_minimum_required(VERSION 3.0)

# Setup the project.
project(uki VERSION 1.0.0 LANGUAGES C)
set(CMAKE_BUILD_TYPE Debug)

# Setup the files.
file(GLOB SOURCES "src/*.c")
file(GLOB HEADERS "src/*.h")
list(REMOVE_ITEM SOURCES "src/main.c")

# Build our shared library
add_library(${PROJECT_NAME} SHARED ${SOURCES})

# Build our test binary.
#file(GLOB TESTSOURCES "src/*.c")
#set(PROJECT_LINK_LIBS libuki.so)
#link_directories(${CMAKE_BINARY_DIR})
#set(TEST_NAME ${PROJECT_NAME}test)
#add_executable(${TEST_NAME} ${TESTSOURCES})
#target_link_libraries(${TEST_NAME} ${PROJECT_LINK_LIBS})

# Set properties.
set_property(TARGET ${PROJECT_NAME} PROPERTY VERSION ${PROJECT_VERSION})
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${HEADERS}")

# Configure the installation.
install(
  TARGETS ${PROJECT_NAME}
  LIBRARY DESTINATION lib
  PUBLIC_HEADER DESTINATION include/${PROJECT_NAME}
  INCLUDES DESTINATION include/${PROJECT_NAME})
