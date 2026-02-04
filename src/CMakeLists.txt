add_executable(cppweather main.cpp
    core/application.cpp)

target_include_directories(cppweather PRIVATE
    ${CMAKE_BINARY_DIR}
    ${PROJECT_SOURCE_DIR}/vendors
    ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(cppweather PRIVATE
    # Always require threads.
    Threads::Threads
)
