add_executable(BellHybrid)
add_image(BellHybrid)

add_custom_target(Bell
    DEPENDS BellHybrid BellHybrid.img
    )


target_link_directories(BellHybrid PUBLIC ${PROJECT_LIB_DIRECTORY})

target_compile_definitions(BellHybrid PUBLIC ${PROJECT_CONFIG_DEFINITIONS}
    ${TARGET_COMPILE_DEFINITIONS}
    ${PROJECT_TARGET}
    )

target_compile_features(BellHybrid PUBLIC
    ${TARGET_COMPILE_FEATURES})

target_compile_options(BellHybrid PUBLIC $<$<COMPILE_LANGUAGE:C>:-Wno-discarded-qualifiers>)
target_compile_options(BellHybrid PUBLIC "-Wno-error=deprecated-declarations")

target_sources(BellHybrid PRIVATE ${TARGET_SOURCES})

target_sources(BellHybrid
    PRIVATE
    BellHybridMain.cpp
    )
target_include_directories(BellHybrid PUBLIC ${TARGET_DIR_INCLUDES})
target_include_directories(BellHybrid PUBLIC ${PROJECT_INCLUDES})
target_include_directories(BellHybrid PUBLIC ${CMAKE_BINARY_DIR})

set_target_properties(BellHybrid PROPERTIES SUFFIX ".elf")

set_target_properties(BellHybrid PROPERTIES LINK_FLAGS "-Xlinker -Map=${CMAKE_BINARY_DIR}/BellHybrid.map ")

target_link_libraries(BellHybrid
    PUBLIC
    module-apps
    service-desktop
    service-bluetooth
    service-lwip
    ${LWIP_LIBRARIES}
    )

if (${PROJECT_TARGET} STREQUAL "TARGET_Linux")
    target_link_libraries(BellHybrid
        PUBLIC iosyscalls)
    add_dependencies(Pure service_renderer)
endif()

set_source_files_properties(BellHybridMain.cpp PROPERTIES COMPILE_DEFINITIONS "${ENABLED_APPS_DEFINES}")

target_link_options(BellHybrid PUBLIC ${TARGET_LINK_OPTIONS})



if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
    add_custom_command(TARGET BellHybrid POST_BUILD
        COMMAND ${CMAKE_STRIP} --strip-debug --strip-unneeded $<TARGET_FILE:BellHybrid>
        )
endif()

if (${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
    add_custom_command(TARGET BellHybrid POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} --only-keep-debug
        $<TARGET_FILE:BellHybrid>
        $<TARGET_FILE:BellHybrid>.debug
        COMMAND ${CMAKE_STRIP} --strip-debug --strip-unneeded $<TARGET_FILE:BellHybrid>
        COMMAND ${CMAKE_OBJCOPY} --add-gnu-debuglink=$<TARGET_FILE:BellHybrid>.debug
        $<TARGET_FILE:BellHybrid>
        )
endif()

message_serial_status()

target_link_libraries(BellHybrid PUBLIC messagetype)

add_dependencies(BellHybrid version)

include(AddBootBin)
add_boot_bin(BellHybrid)

include(AddHexTarget)
add_hex_target(BellHybrid)

add_standalone_image(BellHybrid)
add_update_package(BellHybrid)

