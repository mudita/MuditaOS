add_library(app-window STATIC)

target_sources(app-window
    PRIVATE
        AppWindow.cpp
    PUBLIC
        include/AppWindow.hpp
)

target_include_directories(app-window
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(app-window
    PRIVATE
        module-gui
)
