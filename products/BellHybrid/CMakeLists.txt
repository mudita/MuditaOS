add_executable(BellHybrid)

target_link_directories(BellHybrid PRIVATE ${PROJECT_LIB_DIRECTORY})

target_compile_definitions(BellHybrid
    PRIVATE
        ${PROJECT_CONFIG_DEFINITIONS}
        ${PROJECT_TARGET}
    )

target_compile_options(BellHybrid
    PRIVATE
        $<$<COMPILE_LANGUAGE:C>:-Wno-discarded-qualifiers>
        "-Wno-error=deprecated-declarations"
    )


target_sources(BellHybrid
    PRIVATE
        BellHybridMain.cpp
        PlatformFactory.cpp
        EinkSentinelBell.cpp
    )

target_include_directories(BellHybrid
    PRIVATE
        ${TARGET_DIR_INCLUDES}
        ${PROJECT_INCLUDES}
        ${CMAKE_BINARY_DIR}
)

set_target_properties(BellHybrid
    PROPERTIES
        SUFFIX ".elf"
        LINK_FLAGS "-Xlinker -Map=${CMAKE_BINARY_DIR}/BellHybrid.map "
    )

target_link_libraries(BellHybrid
    PRIVATE
        appmgr
        bell::app-alarm
        bell::app-onboarding
        bell::app-background-sounds
        bell::app-common
        bell::app-main
        bell::app-powernap
        bell::app-meditation-timer
        appmgr
        bell::app-settings
        bell::db
        evtmgr
        log
        messagetype
        module-bsp
        module-vfs
        service-audio
        service-desktop
        service-time
        sys
        bell::time
        platform
        version-header
        "$<$<STREQUAL:${PROJECT_TARGET},TARGET_Linux>:iosyscalls>"
        "$<$<STREQUAL:${PROJECT_TARGET},TARGET_RT1051>:CrashCatcher::CrashCatcher>"
    )

if (${PROJECT_TARGET} STREQUAL "TARGET_Linux")
    add_dependencies(BellHybrid service_renderer)
endif()

set_source_files_properties(BellHybridMain.cpp PROPERTIES COMPILE_DEFINITIONS "${ENABLED_APPS_DEFINES}")

strip_executable(BellHybrid)

include(AddBootBin)
add_boot_bin(BellHybrid)
add_image(
    PRODUCT BellHybrid
    SYSROOT sysroot
    ASSETS  assets
)

include(AddHexTarget)
add_hex_target(BellHybrid)

include(AddVersionJson)
add_version_json(BellHybrid)

add_standalone_image(BellHybrid)
add_update_package(BellHybrid)

add_subdirectory(alarms)
add_subdirectory(apps)
add_subdirectory(keymap)
add_subdirectory(services)
add_subdirectory(sys)
