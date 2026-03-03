add_library(cppweather-networks
    GeoNetwork.cpp
    GeoNetwork.h
)

target_include_directories(cppweather-networks
    PUBLIC
    ${PROJECT_SOURCE_DIR}/src
    ${CMAKE_BINARY_DIR}
)

target_link_libraries(cppweather-networks
    PUBLIC
    cppweather-utility
    httplib::httplib
)
