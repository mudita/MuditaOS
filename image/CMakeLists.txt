include(Utils)

set(ASSETS_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(ASSETS_DEST_DIR "${CMAKE_BINARY_DIR}/sys")

add_custom_target(
    assets
    COMMAND rsync -ravu --delete
        ${ASSETS_SOURCE_DIR}/.boot.json*
        ${ASSETS_SOURCE_DIR}/personalization.json
        ${ASSETS_DEST_DIR}
    COMMAND rsync -ravu --delete
        ${ASSETS_SOURCE_DIR}/assets
        ${ASSETS_SOURCE_DIR}/Luts.bin
        ${ASSETS_SOURCE_DIR}/country-codes.db
        ${ASSETS_DEST_DIR}/current
    COMMAND rsync -ravu --delete
        ${ASSETS_SOURCE_DIR}/user
        ${ASSETS_DEST_DIR}
    COMMENT
        "Copying assets.."
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
