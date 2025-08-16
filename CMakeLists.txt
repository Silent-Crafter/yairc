# Minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Project name
project(MyProject)

# Set C standard to C99 (you can adjust it as needed)
set(CMAKE_C_STANDARD 99)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Include directories (if you have other include directories to add)
include_directories(${CMAKE_SOURCE_DIR})

# Define the first target for a.out (main.c and utils.c)
add_executable(irc main.c utils.c)

# Define the second target for b.out (client.c and utils.c)
add_executable(irc-client client.c utils.c)

# Optionally, you can specify any additional flags or options if needed:
# For example, to enable debugging information, you can set:
# set(CMAKE_BUILD_TYPE Debug)