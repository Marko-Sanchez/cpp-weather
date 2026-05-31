add_library(cppweather-utility
    weatherparser.cpp
    weatherparser.h
    commandlineparser.cpp
    commandlineparser.h
    logfile.h
    weatherdata.h
    threadsafeslot.h
    appsingleton.h
    iresult.h
    errorhandling.h
    signal.h
)

target_include_directories(cppweather-utility
    PUBLIC
    ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(cppweather-utility
    PUBLIC
    nlohmann_json::nlohmann_json
)
