add_library(cppweather-layers
    layer.cpp
    layer.h
    titlelayer.cpp
    titlelayer.h
    aboutmelayer.h
    aboutmelayer.cpp
)

target_include_directories(cppweather-layers
    PUBLIC
    ${PROJECT_SOURCE_DIR}/src
    ${CMAKE_BINARY_DIR}
)

target_link_libraries(cppweather-layers
    PUBLIC
        raylib
)
