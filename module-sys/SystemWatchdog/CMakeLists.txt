add_library(sys-watchdog STATIC)

target_sources(sys-watchdog
    PUBLIC
        include/SystemWatchdog/SystemWatchdog.hpp
        include/SystemWatchdog/Watchdog.hpp

    PRIVATE
        SystemWatchdog.cpp
)

target_include_directories(sys-watchdog
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
)

target_link_libraries(sys-watchdog
    PUBLIC
        sys-common

    PRIVATE
        module-bsp
)
