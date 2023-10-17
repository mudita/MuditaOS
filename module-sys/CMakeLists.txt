module_is_test_entity()

add_subdirectory(common)
add_subdirectory(PhoneModes)
add_subdirectory(Service)
add_subdirectory(SystemManager)
add_subdirectory(SystemWatchdog)

add_library(module-sys INTERFACE)

target_link_libraries(module-sys
    INTERFACE
        sys-common
        sys-service
        sys-phonemodes
        sys-watchdog
        sys-manager
        pure-core
    )
