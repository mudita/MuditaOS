add_executable(PurePhone)

add_custom_target(Pure ALL
    DEPENDS PurePhone PurePhone.img
    )


target_link_directories(PurePhone PUBLIC ${PROJECT_LIB_DIRECTORY})

target_compile_definitions(PurePhone
    PRIVATE
        ${PROJECT_CONFIG_DEFINITIONS}
        ${PROJECT_TARGET}
    )

define_serial(PurePhone)

target_compile_options(PurePhone
    PRIVATE
        $<$<COMPILE_LANGUAGE:C>:-Wno-discarded-qualifiers>
        "-Wno-error=deprecated-declarations"
        "-Wno-prio-ctor-dtor"
    )

target_sources(PurePhone
    PRIVATE
        ${TARGET_SOURCES}
        PurePhoneMain.cpp
        PlatformFactory.cpp
        EinkSentinelPure.cpp
        init_prof.cpp
    )

target_include_directories(PurePhone
    PRIVATE
        ${TARGET_DIR_INCLUDES}
        ${PROJECT_INCLUDES}
        ${CMAKE_BINARY_DIR}
    )

set_target_properties(PurePhone
    PROPERTIES
        SUFFIX ".elf"
        LINK_FLAGS "-Xlinker -Map=${CMAKE_BINARY_DIR}/PurePhone.map "
    )

target_link_libraries(PurePhone
    PRIVATE
        app
        $<$<BOOL:${ENABLE_APP_ANTENNA}>:application-antenna>
        application-calculator
        application-calendar
        application-call
        application-calllog
        application-desktop
        application-meditation
        application-messages
        application-music-player
        application-notes
        application-phonebook
        application-settings
        application-special-input
        pure::time
        appmgr
        db
        evtmgr
        log
        logdump
        messagetype
        module-apps
        module-bsp
        module-vfs
        platform
        service-audio
        service-bluetooth
        service-desktop
        service-fileindexer
        service-time
        sys
        version-header
        "$<$<STREQUAL:${PROJECT_TARGET},TARGET_Linux>:iosyscalls>"
        "$<$<STREQUAL:${PROJECT_TARGET},TARGET_RT1051>:CrashCatcher::CrashCatcher>"
    )

if (${PROJECT_TARGET} STREQUAL "TARGET_Linux")
    add_dependencies(Pure service_renderer)
endif()

set_source_files_properties(PurePhoneMain.cpp PROPERTIES COMPILE_DEFINITIONS "${ENABLED_APPS_DEFINES}")

strip_executable(PurePhone)

message_serial_status()

include(AddBootBin)
add_boot_bin(PurePhone)

add_image(
    PRODUCT PurePhone
    SYSROOT sysroot
    LUTS Luts.bin
    DEPENDS assets updater.bin-target ecoboot.bin-target PurePhone-boot.bin PurePhone-version.json-target
)

include(AddHexTarget)
add_hex_target(PurePhone)

include(BinaryAssetsVersions.cmake)
include(DownloadAsset)

download_asset_json( json-target
                     ${CMAKE_CURRENT_SOURCE_DIR}/assets.json
                     ${CMAKE_BINARY_DIR}/sysroot/sys/current/
                     MuditaOSAssets
                     ${MUDITA_CACHE_DIR}
                    )
download_asset_release_json(    json-images-target
                                ${CMAKE_CURRENT_SOURCE_DIR}/assets_images.json
                                ${CMAKE_BINARY_DIR}/sysroot/sys/current/
                                MuditaOSPublicAssets
                                0.0.4
                                ${MUDITA_CACHE_DIR}
                            )
download_asset_release(PureUpdater_RT.bin updater.bin PureUpdater ${UPDATER_BIN_VERSION} ${MUDITA_CACHE_DIR})
download_asset_release(ecoboot.bin ecoboot.bin ecoboot ${ECOBOOT_BIN_VERSION} ${MUDITA_CACHE_DIR})

include(AddVersionJson)
add_version_json(PurePhone)

add_standalone_image(PurePhone)
add_update_package(PurePhone)

include(GitHubWorkerMessages)
github_message_standalone()
github_message_update()

add_subdirectory(alarms)
add_subdirectory(apps)
add_subdirectory(services)
add_subdirectory(sys)

if (${ENABLE_TESTS})
    add_subdirectory(test)
endif ()
