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
        bell::app-common
        bell::appmgr
        bell::alarms
        bell::audio
)

add_subdirectory(application-bell-main)
add_subdirectory(application-bell-onboarding)
add_subdirectory(application-bell-background-sounds)
add_subdirectory(application-bell-alarm)
add_subdirectory(application-bell-bedtime)
add_subdirectory(application-bell-meditation-timer)
add_subdirectory(application-bell-settings)
add_subdirectory(application-bell-powernap)
add_subdirectory(common)
