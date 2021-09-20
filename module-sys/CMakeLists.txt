module_is_test_entity()

add_subdirectory(PhoneModes)
add_subdirectory(Service)
add_subdirectory(SystemManager)
add_subdirectory(SystemWatchdog)
add_subdirectory(Timers)

add_library(module-sys INTERFACE)

target_link_libraries(module-sys
    INTERFACE
        sys-service
        sys-timers
        sys-phonemodes
        sys-watchdog
        sys-manager
    )
