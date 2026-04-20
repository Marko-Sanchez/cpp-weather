add_subdirectory(utility)
add_subdirectory(render)
add_subdirectory(layers)
add_subdirectory(networks)
add_subdirectory(core)

add_executable(cppweather
    main.cpp)

target_include_directories(cppweather PRIVATE
    ${CMAKE_BINARY_DIR}
)

target_link_libraries(cppweather PRIVATE
    cppweather-core
)
