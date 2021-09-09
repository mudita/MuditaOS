add_library(app STATIC)

target_sources(app
    PRIVATE
        Application.cpp
    PUBLIC
        include/Application.hpp
)

target_include_directories(app
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(app
    PRIVATE
        apps-common
)

