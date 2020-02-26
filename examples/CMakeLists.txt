### CMakeList.txt
### CMake definitions for the Uki library usaeg examples.
###
### Author: Nathan Campos <hi@nathancampos.me>

# Determine the minimum CMake version.
cmake_minimum_required(VERSION 3.0)

# Setup the project.
project(ukitest C)
set(CMAKE_BUILD_TYPE Debug)
set(PROJECT_LINK_LIBS libuki.so)
add_compile_options(-Wall -Wextra -pedantic)

# Setup the files and directories
file(GLOB SOURCES "*.c")
include_directories(${CMAKE_SOURCE_DIR}/src)
link_directories(${CMAKE_BINARY_DIR}/lib)

# Build our example binary.
add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${PROJECT_LINK_LIBS})

# Configure the installation.
install(
  TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION bin)
