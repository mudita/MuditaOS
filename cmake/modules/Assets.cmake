set(ASSETS_SOURCE_DIR ${CMAKE_SOURCE_DIR}/image)

function(add_assets_target)
    cmake_parse_arguments(
        _ASSETS
        ""
        "TARGET;SOURCE_DIR;DEST_DIR;DEVEL;"
        "DEPENDS"
        ${ARGN}
    )

    if(NOT ${_ASSETS_DEVEL})
        set(EXCLUDED --exclude \"*-devel*\")
    endif()

    add_custom_target(
        ${_ASSETS_TARGET}
        DEPENDS ${_ASSETS_DEPENDS}
        COMMAND mkdir -p ${_ASSETS_DEST_DIR}
        COMMAND rsync -qravu --delete
            ${_ASSETS_SOURCE_DIR}/.boot.json*
            ${_ASSETS_SOURCE_DIR}/personalization.json
            ${_ASSETS_DEST_DIR}
        COMMAND rsync -qravu
            ${_ASSETS_SOURCE_DIR}/assets
            ${_ASSETS_SOURCE_DIR}/country-codes.db
            ${_ASSETS_DEST_DIR}/current
        COMMAND rsync -qravu ${EXCLUDED}
            ${_ASSETS_SOURCE_DIR}/user
            ${_ASSETS_DEST_DIR}
        COMMAND find ${_ASSETS_DEST_DIR} -name "*-devel*" | sed "\"s,\\(.*\\)-devel\\(.*\\),& \\1\\2,\"" | xargs --no-run-if-empty -L1 mv
        COMMENT
            "Copying assets.. add_assets_target (${_ASSETS_TARGET}) <- ${_ASSETS_DEPENDS}"
    )
endfunction()
