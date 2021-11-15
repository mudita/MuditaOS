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
        bell::app-bedtime
        bell::app-common
        bell::app-main
        bell::app-meditation-timer
        bell::app-powernap
        bell::app-settings
        bell::db
        bell::audio
        bell::evtmgr
        log
        messagetype
        module-bsp
        module-vfs
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

include(BinaryAssetsVersions.cmake)
include(DownloadAsset)
include(DownloadAsset2)
download_asset2(${CMAKE_CURRENT_SOURCE_DIR}/assets.json ${PROJECT_SOURCE_DIR} ${MUDITA_CACHE_DIR})
download_asset(PureUpdater_RT.bin updater.bin PureUpdater ${UPDATER_BIN_VERSION})
download_asset(ecoboot.bin ecoboot.bin ecoboot ${ECOBOOT_BIN_VERSION})

include(AddVersionJson)
add_version_json(BellHybrid)

add_standalone_image(BellHybrid)
add_update_package(BellHybrid)

add_subdirectory(alarms)
add_subdirectory(apps)
add_subdirectory(keymap)
add_subdirectory(services)
add_subdirectory(sys)
