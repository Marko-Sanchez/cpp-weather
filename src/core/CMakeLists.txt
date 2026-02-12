add_library(cppweather-core
    application.cpp
    network.cpp)

target_include_directories(cppweather-core
    PUBLIC
    ${PROJECT_SOURCE_DIR}/src
    ${CMAKE_BINARY_DIR}
)

add_subdirectory(${PROJECT_SOURCE_DIR}/vendors/json
                ${CMAKE_BINARY_DIR}/nlohmann_json)

add_subdirectory(${PROJECT_SOURCE_DIR}/vendors/cpp-httplib
                ${CMAKE_BINARY_DIR}/cpp-httplib)

target_link_libraries(cppweather-core
    PUBLIC
        # Always require threads.
        Threads::Threads
        nlohmann_json::nlohmann_json
        httplib::httplib
)
