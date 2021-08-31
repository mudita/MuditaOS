include(Assets)
include(Utils)

set(SYSROOT ${CMAKE_BINARY_DIR}/sysroot)
set(ASSETS_DEST_DIR ${SYSROOT}/sys)

add_assets_target(
    TARGET assets
    SOURCE_DIR ${ASSETS_SOURCE_DIR}
    DEST_DIR ${ASSETS_DEST_DIR}
    DEVEL ${WITH_DEVELOPMENT_FEATURES}
)

multicomp_install(
    FILES
        ${ASSETS_DEST_DIR}/.boot.json
        ${ASSETS_DEST_DIR}/.boot.json.crc32
    DESTINATION ./
    COMPONENTS Standalone Update
)

multicomp_install(
    DIRECTORY
        ${ASSETS_DEST_DIR}/current/
        ${ASSETS_DEST_DIR}/user
    DESTINATION ./
    COMPONENTS Standalone Update
)
