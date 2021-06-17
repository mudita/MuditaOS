add_library(lwip-port STATIC)
add_library(lwip::lwip ALIAS lwip-port)

# remove -Werror for lwip
string(REPLACE "-Wno-error=format" "" LWIP_COMPILER_FLAGS "${TARGET_COMPILE_OPTIONS}")

include(${CMAKE_CURRENT_SOURCE_DIR}/lwip-includes.cmake)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
set (LWIP_DEFINITIONS LWIP_DEBUG=1)
endif()

include(${CMAKE_CURRENT_SOURCE_DIR}/src/src/Filelists.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/src/contrib/Filelists.cmake)

target_sources(lwip-port
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/contrib/ports/freertos/sys_arch.c
        ${BOARD_DIR_SOURCES}
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
        ${TARGET_LIBRARIES_INCLUDES}
        ${LWIP_INCLUDE_DIRS}
)

target_link_libraries(
    lwip-port
        module-bsp
        module-utils
        module-vfs
        module-sys
        ${BOARD_DIR_LIBRARIES}
        ${LWIP_LIBRARIES}
    )