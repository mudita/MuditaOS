add_executable(PurePhone)
add_image(PurePhone)

add_custom_target(Pure
    DEPENDS PurePhone PurePhone.img
    )


target_link_directories(PurePhone PUBLIC ${PROJECT_LIB_DIRECTORY})

target_compile_definitions(PurePhone PUBLIC ${PROJECT_CONFIG_DEFINITIONS}
    ${TARGET_COMPILE_DEFINITIONS}
    ${PROJECT_TARGET}
    )

define_serial(PurePhone)

target_compile_features(PurePhone PUBLIC
    ${TARGET_COMPILE_FEATURES})

target_compile_options(PurePhone PUBLIC $<$<COMPILE_LANGUAGE:C>:-Wno-discarded-qualifiers>)
target_compile_options(PurePhone PUBLIC "-Wno-error=deprecated-declarations")

target_sources(PurePhone PRIVATE ${TARGET_SOURCES})

target_sources(PurePhone
    PRIVATE
    PurePhoneMain.cpp
    )
target_include_directories(PurePhone PUBLIC ${TARGET_DIR_INCLUDES})
target_include_directories(PurePhone PUBLIC ${PROJECT_INCLUDES})
target_include_directories(PurePhone PUBLIC ${CMAKE_BINARY_DIR})

set_target_properties(PurePhone PROPERTIES SUFFIX ".elf")

set_target_properties(PurePhone PROPERTIES LINK_FLAGS "-Xlinker -Map=${CMAKE_BINARY_DIR}/PurePhone.map ")

target_link_libraries(PurePhone
    PUBLIC
    module-apps
    service-desktop
    service-bluetooth
    service-lwip
    ${LWIP_LIBRARIES}
    )

if (${PROJECT_TARGET} STREQUAL "TARGET_Linux")
    target_link_libraries(PurePhone
        PUBLIC iosyscalls)
    add_dependencies(Pure service_renderer)
endif()

set_source_files_properties(PurePhoneMain.cpp PROPERTIES COMPILE_DEFINITIONS "${ENABLED_APPS_DEFINES}")

target_link_options(PurePhone PUBLIC ${TARGET_LINK_OPTIONS})



if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
    add_custom_command(TARGET PurePhone POST_BUILD
        COMMAND ${CMAKE_STRIP} --strip-debug --strip-unneeded $<TARGET_FILE:PurePhone>
        )
endif()

if (${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
    add_custom_command(TARGET PurePhone POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} --only-keep-debug
        $<TARGET_FILE:PurePhone>
        $<TARGET_FILE:PurePhone>.debug
        COMMAND ${CMAKE_STRIP} --strip-debug --strip-unneeded $<TARGET_FILE:PurePhone>
        COMMAND ${CMAKE_OBJCOPY} --add-gnu-debuglink=$<TARGET_FILE:PurePhone>.debug
        $<TARGET_FILE:PurePhone>
        )
endif()

message_serial_status()

target_link_libraries(PurePhone PUBLIC messagetype)

add_dependencies(PurePhone version)

include(AddBootBin)
add_boot_bin(PurePhone)

include(AddHexTarget)
add_hex_target(PurePhone)

add_standalone_image(PurePhone)
add_update_package(PurePhone)

