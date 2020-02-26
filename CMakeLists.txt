### CMakeList.txt
### CMake definitions for the Uki shared library.
###
### Author: Nathan Campos <hi@nathancampos.me>

# Determine the minimum CMake version.
cmake_minimum_required(VERSION 3.0)

# Setup the project.
project(libuki C)
set(CMAKE_BUILD_TYPE Debug)

# If we are in Debug mode, make install will put everything inside the dist/.
if(CMAKE_BUILD_TYPE MATCHES Debug)
  message("Build Type: Debug")
  set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/dist)
endif(CMAKE_BUILD_TYPE MATCHES Debug)

# Compile the shared library.
add_subdirectory(src ${CMAKE_BINARY_DIR}/lib)
add_subdirectory(examples ${CMAKE_BINARY_DIR}/examples)
