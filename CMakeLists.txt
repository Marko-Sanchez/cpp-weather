cmake_minimum_required(VERSION 3.14.0)

project(cppweather
    VERSION "0.2.1"
    LANGUAGES CXX
    DESCRIPTION "A C++ desktop weather application."
    HOMEPAGE_URL "https://github.com/Marko-Sanchez/cpp-weather"
)

# Set CPP standard.
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Git QUIET)

if(GIT_FOUND)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        OUTPUT_VARIABLE CPPWEATHER_GIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
else()
    set(CPPWEATHER_GIT_HASH "unknown")
endif()

# Threads needed; Use pthreads if possible,
# finds preferred thread library of the system ensuring project builds correctly across different platforms.
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

configure_file(
    ${PROJECT_SOURCE_DIR}/cppweather.h.in
    ${CMAKE_BINARY_DIR}/cppweather/cppweather.h
)

add_subdirectory(src)
