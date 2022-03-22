include(Assets)
include(Utils)

set(SYSROOT ${CMAKE_BINARY_DIR}/sysroot)
set(ASSETS_DEST_DIR ${SYSROOT}/sys)

set(ASSETS_DEPENDENCIES "json-common-target")

if (${ASSETS_TYPE} STREQUAL "Proprietary")
    list(APPEND ASSETS_DEPENDENCIES "json-proprietary-target")
endif()

if (${ASSETS_TYPE} STREQUAL "Community")
    list(APPEND ASSETS_DEPENDENCIES "json-community-target")
endif()

if (${PROJECT_TARGET} STREQUAL "TARGET_RT1051")
    list(APPEND ASSETS_DEPENDENCIES "json-rt1051-target")
endif()

add_assets_target(
    TARGET assets
    SOURCE_DIR ${ASSETS_SOURCE_DIR}
    DEST_DIR ${ASSETS_DEST_DIR}
    DEVEL ${WITH_DEVELOPMENT_FEATURES}
    DEPENDS
        ${ASSETS_DEPENDENCIES}
)

multicomp_install(
    FILES
        ${ASSETS_DEST_DIR}/.boot.json
        ${ASSETS_DEST_DIR}/.boot.json.crc32
    DESTINATION ./
    COMPONENTS Standalone Update
)
