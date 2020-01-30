project(module-lwip VERSION 1.0 DESCRIPTION "LwIP port layer")

include(${CMAKE_SOURCE_DIR}/config/ModuleConfig.cmake)

# remove -Werror for lwip
string(REPLACE "-Werror" "" TARGET_COMPILE_OPTIONS "${TARGET_COMPILE_OPTIONS}")

# TODO check me
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BUILD_DIRECTORY})
set(LWIP_COMPILER_FLAGS ${TARGET_COMPILE_OPTIONS})

# module-lwip/lib/lwip/src/include/lwip/err.h

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
    ${LWIP_INCLUDE_DIRS}
)

target_link_libraries(
    ${PROJECT_NAME}
    module-bsp
    module-utils
    module-vfs
    module-sys
    ${BOARD_DIR_LIBRARIES}
    )
