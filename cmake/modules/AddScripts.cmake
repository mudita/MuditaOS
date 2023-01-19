function(add_scripts_target)
    cmake_parse_arguments(
            _ARG
            ""
            "TARGET;DEST_DIR;PRODUCT"
            "DEPENDS"
            ${ARGN}
    )

    add_custom_target(
            ${_ARG_TARGET}
            DEPENDS ${_ARG_DEPENDS}

            COMMAND  ${PROJECT_SOURCE_DIR}/scripts/lua/install.sh ${_ARG_PRODUCT} ${_ARG_DEST_DIR} udm
            COMMENT
            "Installing scripts for ${_ARG_PRODUCT} to ${_ARG_DEST_DIR} directory"
    )
endfunction()
