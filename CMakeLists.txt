cmake_minimum_required(VERSION 3.10)

# Project name and version
project(kilo VERSION 0.0.1 LANGUAGES C)

# Set C standard
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Set compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pedantic")

# Include directories
include_directories(include)

# Source files
file(GLOB SRC src/*.c)

# Create executable
add_executable(kilo ${SRC})