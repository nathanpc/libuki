### CMakeList.txt
### CMake definitions for the Uki shared library.
###
### Author: Nathan Campos <hi@nathancampos.me>

# Determine the minimum CMake version.
cmake_minimum_required(VERSION 3.0)

# Setup the project.
project(uki VERSION 1.0.0 LANGUAGES C)
set(CMAKE_BUILD_TYPE Debug)
add_compile_options(-Wall -Wextra -pedantic)

# Setup the files.
file(GLOB SOURCES "*.c")
file(GLOB HEADERS "*.h")

# Build our shared library
add_library(${PROJECT_NAME} SHARED ${SOURCES})

# Set properties.
set_property(TARGET ${PROJECT_NAME} PROPERTY VERSION ${PROJECT_VERSION})
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${HEADERS}")

# Configure the installation.
install(
  TARGETS ${PROJECT_NAME}
  LIBRARY DESTINATION lib
  PUBLIC_HEADER DESTINATION include/${PROJECT_NAME}
  INCLUDES DESTINATION include/${PROJECT_NAME})
