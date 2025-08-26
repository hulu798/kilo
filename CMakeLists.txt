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
set(SOURCES
    src/main.c
    src/terminal.c
    src/syntax.c
    src/editor_row.c
    src/editor_core.c
    src/editor_input.c
    src/file_io.c
    src/search.c
)

# Create executable
add_executable(kilo ${SOURCES})

# Add compile definitions for Linux
if(UNIX AND NOT APPLE)
    target_compile_definitions(kilo PRIVATE _POSIX_C_SOURCE=200809L)
endif()

# Installation
install(TARGETS kilo DESTINATION bin)

# Optional: Add a target to build from the original single file
add_executable(kilo_original kilo.c)
if(UNIX AND NOT APPLE)
    target_compile_definitions(kilo_original PRIVATE _POSIX_C_SOURCE=200809L)
endif()