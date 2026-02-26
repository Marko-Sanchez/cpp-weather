add_library(cppweather-core
    application.cpp
    application.h
    network.cpp
    network.h
)

target_include_directories(cppweather-core
    PUBLIC
    ${PROJECT_SOURCE_DIR}/src
    ${CMAKE_BINARY_DIR}
)

target_link_libraries(cppweather-core
    PUBLIC
        cppweather-layers
        cppweather-utility
        # Always require threads.
        Threads::Threads
        httplib::httplib
)
