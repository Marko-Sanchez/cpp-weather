add_library(cppweather-networks
    geonetwork.cpp
    geonetwork.h
    weathernetwork.cpp
    weathernetwork.h
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
