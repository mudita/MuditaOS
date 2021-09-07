project(module-lwip VERSION 1.0 DESCRIPTION "LwIP port layer")

include(${CMAKE_CURRENT_SOURCE_DIR}/lwip-includes.cmake)

set (SOURCES
    "${LWIP_DIR}/contrib/ports/freertos/sys_arch.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/dhcp-server/dhserver.c"
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
set (LWIP_DEFINITIONS LWIP_DEBUG=1)
endif()

include(${LWIP_DIR}/src/Filelists.cmake)
include(${LWIP_DIR}/contrib/Filelists.cmake)

# for any target dependent on lwip
# target_include_directories(<app> PRIVATE ${LWIP_INCLUDE_DIRS})

# add to target
set(LWIP_LIBRARIES lwipcontribapps lwipallapps lwipcore)

### TODO maybe make it a bit shared
add_library(${PROJECT_NAME} STATIC ${SOURCES} ${BOARD_DIR_SOURCES})
target_compile_definitions(${PROJECT_NAME} PUBLIC   ${PROJECT_CONFIG_DEFINITIONS}
                                                    ${PROJECT_TARGET}
                                                    ${BOARD_DIR_DEFINITIONS}
                                                    )

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${LWIP_INCLUDE_DIRS}
)

target_link_libraries(
    ${PROJECT_NAME}
    module-bsp
    module-utils
    module-vfs
    module-sys
    ${BOARD_DIR_LIBRARIES}
    ${LWIP_LIBRARIES}
    )
