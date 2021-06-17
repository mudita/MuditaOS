project(module-lwip VERSION 1.0 DESCRIPTION "LwIP port layer")


# remove -Werror for lwip
string(REPLACE "-Wno-error=format" "" LWIP_COMPILER_FLAGS "${TARGET_COMPILE_OPTIONS}")

include(${CMAKE_SOURCE_DIR}/third-party/lwip/lwip-includes.cmake)

set (SOURCES
    "${LWIP_DIR}/contrib/ports/freertos/sys_arch.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/dhcp-server/dhserver.c"
)

### TODO maybe make it a bit shared
add_library(${PROJECT_NAME} STATIC ${SOURCES} ${BOARD_DIR_SOURCES})
target_compile_definitions(${PROJECT_NAME} PUBLIC   ${PROJECT_CONFIG_DEFINITIONS}
                                                    ${PROJECT_TARGET}
                                                    ${TARGET_COMPILE_DEFINITIONS}
                                                    ${BOARD_DIR_DEFINITIONS}
                                                    )
target_compile_features(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_FEATURES})
target_compile_options(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_OPTIONS})
target_link_options(${PROJECT_NAME} PUBLIC ${TARGET_LINK_OPTIONS})


target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
    ${BOARD_DIR_INCLUDES}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${TARGET_LIBRARIES_INCLUDES}
)

target_link_libraries(
    ${PROJECT_NAME}
    module-bsp
    module-utils
    module-vfs
    module-sys
    ${BOARD_DIR_LIBRARIES}
    lwip::lwip
)
