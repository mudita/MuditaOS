set(ASSETS_SOURCE_DIR ${CMAKE_SOURCE_DIR}/image)

function(add_assets_target)
    cmake_parse_arguments(
        _ASSETS
        ""
        "TARGET;SOURCE_DIR;SYSTEM_DEST_DIR;USER_DEST_DIR;DEVEL;"
        "DEPENDS"
        ${ARGN}
    )

    add_custom_target(
        ${_ASSETS_TARGET}
        DEPENDS ${_ASSETS_DEPENDS}

        # Copy user assets
        COMMAND mkdir -p ${_ASSETS_USER_DEST_DIR}
        COMMAND rsync -qau
            ${_ASSETS_SOURCE_DIR}/user/
            ${_ASSETS_USER_DEST_DIR}
        COMMAND find ${_ASSETS_USER_DEST_DIR} -name "*-devel*" | sed "\"s,\\(.*\\)-devel\\(.*\\),& \\1\\2,\"" | xargs --no-run-if-empty -L1 mv

        # Copy system assets
        COMMAND mkdir -p ${_ASSETS_SYSTEM_DEST_DIR}
        COMMAND rsync -qau
            ${_ASSETS_SOURCE_DIR}/system_a/data
            ${_ASSETS_SOURCE_DIR}/system_a/db
            ${_ASSETS_SYSTEM_DEST_DIR}

        # Create 'golden copy' of DBs
        COMMAND mkdir -p ${_ASSETS_SYSTEM_DEST_DIR}/db/factory
        COMMAND rsync -qlptgoDu
            ${_ASSETS_SYSTEM_DEST_DIR}/db/*
            ${_ASSETS_SYSTEM_DEST_DIR}/db/factory

        COMMENT  "Copying assets... add_assets_target (${_ASSETS_TARGET}) <- ${_ASSETS_DEPENDS}"
    )
endfunction()
