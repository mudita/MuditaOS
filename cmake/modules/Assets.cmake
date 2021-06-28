set(ASSETS_SOURCE_DIR ${CMAKE_SOURCE_DIR}/image)

macro(add_assets_target)

    set(_ASSETS_TARGET ${ARGV0})
    set(_ASSETS_SOURCE_DIR ${ARGV1})
    set(_ASSETS_DEST_DIR ${ARGV2})

    add_custom_target(
        ${_ASSETS_TARGET}
        COMMAND mkdir -p ${_ASSETS_DEST_DIR}
        COMMAND rsync -qravu --delete
            ${_ASSETS_SOURCE_DIR}/.boot.json*
            ${_ASSETS_SOURCE_DIR}/personalization.json
            ${_ASSETS_DEST_DIR}
        COMMAND rsync -qravu --delete
            ${_ASSETS_SOURCE_DIR}/assets
            ${_ASSETS_SOURCE_DIR}/Luts.bin
            ${_ASSETS_SOURCE_DIR}/country-codes.db
            ${_ASSETS_DEST_DIR}/current
        COMMAND rsync -qravu --delete
            ${_ASSETS_SOURCE_DIR}/user
            ${_ASSETS_DEST_DIR}
        COMMENT
            "Copying assets.. (${_ASSETS_TARGET})"
    )
endmacro()
