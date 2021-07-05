add_library(lwip-port STATIC)
add_library(lwip::lwip ALIAS lwip-port)

set (LWIP_DIR ${CMAKE_SOURCE_DIR}/third-party/lwip/src)
set (LWIP_CONTRIB_DIR ${CMAKE_SOURCE_DIR}/third-party/lwip/src/contrib)

set (LWIP_INCLUDE_DIRS
    "${LWIP_DIR}/src/include"
    "${LWIP_DIR}/contrib"
    "${LWIP_DIR}/contrib/ports/freertos/include"
    "${CMAKE_SOURCE_DIR}/module-lwip/includes/"
    "${CMAKE_SOURCE_DIR}/module-lwip/lib/"
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
set (LWIP_DEFINITIONS LWIP_DEBUG=1)
endif()

include(src/src/Filelists.cmake)
include(src/contrib/Filelists.cmake)

target_sources(lwip-port
    PRIVATE
        src/contrib/ports/freertos/sys_arch.c
)

target_compile_definitions(lwip-port PUBLIC   ${PROJECT_CONFIG_DEFINITIONS}
                                                    ${PROJECT_TARGET}
                                                    ${TARGET_COMPILE_DEFINITIONS}
                                                    ${BOARD_DIR_DEFINITIONS}
                                                    )
target_compile_features(lwip-port PUBLIC ${TARGET_COMPILE_FEATURES})
target_compile_options(lwip-port PUBLIC ${TARGET_COMPILE_OPTIONS})
target_link_options(lwip-port PUBLIC ${TARGET_LINK_OPTIONS})

target_include_directories(
    lwip-port
    PUBLIC
        ${BOARD_DIR_INCLUDES}
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${LWIP_DIR}/src/include
        ${LWIP_DIR}/contrib
        ${LWIP_DIR}/contrib/ports/freertos/include
        ${CMAKE_SOURCE_DIR}/module-lwip/includes/
        ${CMAKE_SOURCE_DIR}/module-lwip/lib/

)

target_link_libraries(
    lwip-port
    PRIVATE
        module-sys
        lwipallapps
        lwipcore
)
