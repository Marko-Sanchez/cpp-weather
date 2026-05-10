add_library(cppweather-render
    weathericonatlas.h
    textdecorator.h
    drawablecomponent.h
    imagecomponent.h
    imagecomponent.cpp
    textcomponent.h
    textcomponent.cpp
    paneldecorator.h
    paneldecorator.cpp
    colordecorator.h
    colordecorator.cpp
    cullingdecorator.h
    cullingdecorator.cpp
)

target_include_directories(cppweather-render
    PUBLIC
        ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(cppweather-render
    PUBLIC
        cppweather-utility
        raylib
)
