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
        ${TARGET_SOURCES}
        BellHybridMain.cpp
        PlatformFactory.cpp
        EinkSentinelBell.cpp
        init_prof.cpp
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
        bell::app-relaxation
        bell::app-bedtime
        bell::app-common
        bell::app-main
        bell::app-meditation-timer
        bell::app-powernap
        bell::app-settings
        bell::db
        bell::audio
        bell::evtmgr
        bell::paths
        log
        logdump
        messagetype
        module-bsp
        module-vfs
        service-desktop
        service-time
        service-fileindexer
        sys
        bell::time
        platform
        version-header
        pure-core
        "$<$<STREQUAL:${PROJECT_TARGET},TARGET_Linux>:iosyscalls>"
        "$<$<STREQUAL:${PROJECT_TARGET},TARGET_RT1051>:CrashCatcher::CrashCatcher>"
    )

if (${PROJECT_TARGET} STREQUAL "TARGET_Linux")
    add_dependencies(BellHybrid service_renderer)
endif()


set_source_files_properties(BellHybridMain.cpp PROPERTIES COMPILE_DEFINITIONS "${ENABLED_APPS_DEFINES}")

strip_executable(BellHybrid)

include(BinaryAssetsVersions.cmake)
include(AddVersionJson)

add_directories(
        TARGET create_user_directories
        PREFIX ${CMAKE_BINARY_DIR}/sysroot/user/media/app
        DEPENDS system_directories_common
        DIRECTORIES relaxation
)

if (${PROJECT_TARGET} STREQUAL "TARGET_RT1051")
    include(AddBootBin)
    add_boot_bin(BellHybrid)
    add_image(
        PRODUCT BellHybrid
        SYSROOT sysroot
        LUTS Luts.bin
        DEPENDS
            install_scripts
            create_product_databases
            create_user_directories
            assets
            updater.bin-target
            ecoboot.bin-target
            BellHybrid-boot.bin
            BellHybrid-version.json-target
    )
    add_version_rt1051_json(BellHybrid)
else()
    add_image(
        PRODUCT BellHybrid
        SYSROOT sysroot
        LUTS ""
        DEPENDS
            install_scripts
            create_product_databases
            create_user_directories
            assets
            BellHybrid-version.json-target
    )
    add_version_linux_json(BellHybrid)
endif()

include(AddHexTarget)
add_hex_target(BellHybrid)

include(DownloadAsset)
# copy all assets required to build catalog under current folder as in json recipe
download_asset_json(json-proprietary-target
                    ${CMAKE_CURRENT_SOURCE_DIR}/assets/assets_proprietary.json
                    ${CMAKE_BINARY_DIR}/sysroot/system_a/
                    MuditaOSAssets
                    ${MUDITA_CACHE_DIR}
    )
download_asset_release_json(json-common-target
                            ${CMAKE_CURRENT_SOURCE_DIR}/assets/assets_common.json
                            ${CMAKE_BINARY_DIR}/sysroot/system_a/
                            MuditaOSPublicAssets
                            0.0.14
                            ${MUDITA_CACHE_DIR}
    )
download_asset_release_json(json-community-target
                            ${CMAKE_CURRENT_SOURCE_DIR}/assets/assets_community.json
                            ${CMAKE_BINARY_DIR}/sysroot/system_a/
                            MuditaOSPublicAssets
                            0.0.14
                            ${MUDITA_CACHE_DIR}
    )
download_asset_json(json-rt1051-target
                            ${CMAKE_CURRENT_SOURCE_DIR}/assets/assets_rt1051.json
                            ${CMAKE_BINARY_DIR}/sysroot/system_a/
                            MuditaOSAssets
                            ${MUDITA_CACHE_DIR}
    )
download_asset_release(PureUpdater_RT.bin updater.bin PureUpdater ${UPDATER_BIN_VERSION}  ${MUDITA_CACHE_DIR})
download_asset_release(ecoboot.bin ecoboot.bin ecoboot ${ECOBOOT_BIN_VERSION}  ${MUDITA_CACHE_DIR})


add_standalone_image(BellHybrid)
add_update_package(BellHybrid)

add_subdirectory(alarms)
add_subdirectory(apps)
add_subdirectory(keymap)
add_subdirectory(services)
add_subdirectory(sys)
add_subdirectory(paths)

option(CONFIG_ENABLE_TEMP "Enable displaying temperature" OFF)
option(CONFIG_SHOW_MEMORY_INFO "Enable displaying memory info" OFF)
configure_file(config/ProductConfig.in.hpp ${CMAKE_BINARY_DIR}/ProductConfig.hpp  @ONLY)
