add_library(desktop STATIC)

target_sources(desktop
    PRIVATE
        DeveloperModeHelper.cpp
    PUBLIC
        include/desktop/DeveloperModeHelper.hpp
)

target_include_directories(desktop
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(desktop
    PRIVATE
        service-desktop
)
