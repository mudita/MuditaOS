set(ASSETS_SOURCE_DIR ${CMAKE_SOURCE_DIR}/image)

function(add_assets_target)
    cmake_parse_arguments(
        _ASSETS
        ""
        "TARGET;SOURCE_DIR;DEST_DIR;DEVEL;DEFINITION"
        ""
        ${ARGN}
    )

    set(ASSET_COPY_TOOL ${CMAKE_SOURCE_DIR}/tools/copy-assets.py)

    if(${_ASSETS_DEVEL})
        set(COPY_DEVEL "--devel")
    endif()

    add_custom_target(
        ${_ASSETS_TARGET}
        DEPENDS ${ASSET_COPY_TOOL}
        DEPENDS ${_ASSETS_DEFINITION}
        COMMAND ${ASSET_COPY_TOOL}
            ${COPY_DEVEL}
            ${_ASSETS_SOURCE_DIR}
            ${_ASSETS_DEST_DIR}
            ${_ASSETS_DEFINITION}
        COMMENT "Copying assets..."
    )

    set_target_properties(
        ${_ASSETS_TARGET}
        PROPERTIES ADDITIONAL_CLEAN_FILES ${_ASSETS_DEST_DIR}
    )
endfunction()
