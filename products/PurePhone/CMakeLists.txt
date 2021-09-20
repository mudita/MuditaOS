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
        "-Wno-error=deprecated-declarations"
    )

if (NOT (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "AppleClang"))
target_compile_options(PurePhone
    PRIVATE
        $<$<COMPILE_LANGUAGE:C>:-Wno-discarded-qualifiers>
    )
endif()

target_sources(PurePhone
    PRIVATE
        PurePhoneMain.cpp
        PlatformFactory.cpp
        EinkSentinelPure.cpp
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
        appmgr
        db
        evtmgr
        messagetype
        module-apps
        module-bsp
        module-vfs
        platform
        service-audio
        service-bluetooth
        service-desktop
        service-fileindexer
        service-lwip
        service-time
        sys
        ${LWIP_LIBRARIES}
        "$<$<STREQUAL:${PROJECT_TARGET},TARGET_Linux>:iosyscalls>"
        "$<$<STREQUAL:${PROJECT_TARGET},TARGET_RT1051>:CrashCatcher::CrashCatcher>"
    )

if (${PROJECT_TARGET} STREQUAL "TARGET_Linux")
    add_dependencies(Pure service_renderer)
endif()

set_source_files_properties(PurePhoneMain.cpp PROPERTIES COMPILE_DEFINITIONS "${ENABLED_APPS_DEFINES}")

strip_executable(PurePhone)

message_serial_status()

add_dependencies(PurePhone version)

include(AddBootBin)
add_boot_bin(PurePhone)
add_image(
    PRODUCT PurePhone
    SYSROOT sysroot
    ASSETS  assets
)

include(AddHexTarget)
add_hex_target(PurePhone)

include(AddVersionJson)
add_version_json(PurePhone)

add_standalone_image(PurePhone)
add_update_package(PurePhone)

include(GitHubWorkerMessages)
github_message_standalone()
github_message_update()

add_subdirectory(apps)
add_subdirectory(services)
add_subdirectory(sys)

if (${ENABLE_TESTS})
    add_subdirectory(test)
endif ()
