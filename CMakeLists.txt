cmake_minimum_required(VERSION 3.14.0)

# Get the CPPWEATHER_VERSION value from header file.
file(STRINGS cppweather.h _raw_version_string REGEX "CPPWEATHER_VERSION \"([0-9]+\\.[0-9]+\\.[0-9]+)\"")

# Extracts version number from _raw_version_string, since the above regex grabs the entire code line-statement.
string(REGEX MATCH "([0-9]+\\.?)+" _cppweather_version "${_raw_string_version}")

project(cppweather
    VERSION ${_cppweather_version}
    LANGUAGES CXX
    DESCRIPTION "A C++ desktop weather application."
    HOMEPAGE_URL "https://github.com/Marko-Sanchez/cpp-weather"
)

# Set CPP standard.
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Threads needed; Use pthreads if possible,
# finds preferred thread library of the system ensuring project builds correctly across different platforms.
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

add_executable(${PROJECT_NAME} src/main.cpp)

target_link_libraries(${PROJECT_NAME} PUBLIC
    # Always require threads.
    Threads::Threads
)
