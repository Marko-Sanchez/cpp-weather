add_library(cppweather-render INTERFACE
    weathericonatlas.h
)

target_include_directories(cppweather-render
    INTERFACE
        ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(cppweather-render
    INTERFACE
        cppweather-utility
        raylib
)
