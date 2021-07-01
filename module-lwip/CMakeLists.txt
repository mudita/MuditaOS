add_library(module-lwip STATIC)

# remove -Werror for lwip
string(REPLACE "-Wno-error=format" "" LWIP_COMPILER_FLAGS "${TARGET_COMPILE_OPTIONS}")

target_sources(module-lwip
    PRIVATE
        lib/dhcp-server/dhserver.c
)

target_compile_definitions(module-lwip PUBLIC   ${PROJECT_CONFIG_DEFINITIONS}
                                                    ${PROJECT_TARGET}
                                                    ${TARGET_COMPILE_DEFINITIONS}
                                                    ${BOARD_DIR_DEFINITIONS}
                                                    )
target_compile_features(module-lwip PUBLIC ${TARGET_COMPILE_FEATURES})
target_compile_options(module-lwip PUBLIC ${TARGET_COMPILE_OPTIONS})
target_link_options(module-lwip PUBLIC ${TARGET_LINK_OPTIONS})

target_include_directories(
    module-lwip
    PUBLIC
    ${BOARD_DIR_INCLUDES}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(
    module-lwip
    module-bsp
    module-utils
    module-vfs
    module-sys
    lwip::lwip
    )
