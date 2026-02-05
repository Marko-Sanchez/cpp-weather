add_executable(cppweather main.cpp
    core/application.cpp
    core/network.cpp)

target_include_directories(cppweather PRIVATE
    ${CMAKE_BINARY_DIR}
    ${PROJECT_SOURCE_DIR}/vendors
    ${PROJECT_SOURCE_DIR}/src
)

add_library(nlomann_json INTERFACE)
target_include_directories(nlomann_json INTERFACE ${PROJECT_SOURCE_DIR}/vendors/json/include)

target_link_libraries(cppweather PRIVATE
    # Always require threads.
    Threads::Threads
    # Json library.
    nlomann_json
)
