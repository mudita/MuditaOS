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
        LINK_FLAGS "-Xlinker -Map=${CMAKE_BINARY_DIR}/PurePhone.map"
    )

target_link_libraries(PurePhone
    PRIVATE
        app
        $<$<BOOL:${ENABLE_APP_ANTENNA}>:application-antenna>
        $<$<BOOL:${ENABLE_APP_CALCULATOR}>:application-calculator>
        $<$<BOOL:${ENABLE_APP_CALENDAR}>:application-calendar>
        $<$<BOOL:${ENABLE_APP_CALL}>:application-call>
        $<$<BOOL:${ENABLE_APP_CALLLOG}>:application-calllog>
        $<$<BOOL:${ENABLE_APP_DESKTOP}>:application-desktop>
        $<$<BOOL:${ENABLE_APP_MEDITATION}>:application-meditation>
        $<$<BOOL:${ENABLE_APP_MESSAGES}>:application-messages>
        $<$<BOOL:${ENABLE_APP_MUSIC_PLAYER}>:application-music-player>
        $<$<BOOL:${ENABLE_APP_NOTES}>:application-notes>
        $<$<BOOL:${ENABLE_APP_PHONEBOOK}>:application-phonebook>
        $<$<BOOL:${ENABLE_APP_SETTINGS}>:application-settings>
        $<$<BOOL:${ENABLE_APP_SPECIAL_INPUT}>:application-special-input>
        $<$<BOOL:${ENABLE_APP_ONBOARDING}>:application-onboarding>
        $<$<BOOL:${ENABLE_APP_TEST}>:application-test>
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
        $<$<BOOL:${ENABLE_SERVICE_TEST}>:service-test>
        sys
        version-header
        "$<$<STREQUAL:${PROJECT_TARGET},TARGET_Linux>:iosyscalls>"
        "$<$<STREQUAL:${PROJECT_TARGET},TARGET_RT1051>:CrashCatcher::CrashCatcher>"
    )

if (${PROJECT_TARGET} STREQUAL "TARGET_Linux")
    add_dependencies(PurePhone service_renderer)
endif()


# set special main.cpp file properties
set_property(
  SOURCE PurePhoneMain.cpp
  APPEND
  PROPERTY COMPILE_DEFINITIONS
  ${ENABLED_APPS_DEFINES}
  ${ENABLED_SERVICES_DEFINES}
)

strip_executable(PurePhone)

message_serial_status()

include(BinaryAssetsVersions.cmake)
include(AddVersionJson)

add_directories(
        TARGET create_user_directories
        PREFIX ${SYSROOT_PATH}/user
        DEPENDS system_directories_common
        DIRECTORIES data temp media/app/music_player
)

if (${PROJECT_TARGET} STREQUAL "TARGET_RT1051")
    include(AddBootBin)
    add_boot_bin(PurePhone)
    add_image(
        PRODUCT PurePhone
        SYSROOT sysroot
        DEPENDS
            install_scripts
            create_product_databases
            create_user_directories
            assets
            recovery.bin-target
            ecoboot.bin-target
            PurePhone-boot.bin
            PurePhone-version.json-target
    )
    add_version_rt1051_json(PurePhone)
else()
    add_image(
        PRODUCT PurePhone
        SYSROOT sysroot
        DEPENDS
            install_scripts
            create_product_databases
            create_user_directories
            assets
            PurePhone-version.json-target
    )
    add_version_linux_json(PurePhone)
endif()


include(AddHexTarget)
add_hex_target(PurePhone)

include(DownloadAsset)

download_asset_json(json-proprietary-target
                    ${CMAKE_CURRENT_SOURCE_DIR}/assets/assets_proprietary.json
                    ${SYSROOT_PATH}/system_a/
                    MuditaOSAssets
                    ${MUDITA_CACHE_DIR}
    )
download_asset_release_json(json-common-target
                            ${CMAKE_CURRENT_SOURCE_DIR}/assets/assets_common.json
                            ${SYSROOT_PATH}/
                            MuditaOSPublicAssets
                            0.0.9
                            ${MUDITA_CACHE_DIR}
    )
download_asset_release_json(json-community-target
                            ${CMAKE_CURRENT_SOURCE_DIR}/assets/assets_community.json
                            ${SYSROOT_PATH}/system_a/
                            MuditaOSPublicAssets
                            0.0.8
                            ${MUDITA_CACHE_DIR}
    )
download_asset_json(json-rt1051-target
                            ${CMAKE_CURRENT_SOURCE_DIR}/assets/assets_rt1051.json
                            ${SYSROOT_PATH}/system_a/assets
                            MuditaOSAssets
                            ${MUDITA_CACHE_DIR}
    )

download_asset_release(PureRecovery.bin recovery.bin PureRecovery ${RECOVERY_BIN_VERSION} ${MUDITA_CACHE_DIR})
download_asset_release(ecoboot.bin ecoboot.bin ecoboot ${ECOBOOT_BIN_VERSION} ${MUDITA_CACHE_DIR})


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
