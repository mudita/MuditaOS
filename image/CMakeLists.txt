set(ASSETS_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(ASSETS_DEST_DIR "${CMAKE_BINARY_DIR}/sys")

file (GLOB_RECURSE ASSETS_LIST
    LIST_DIRECTORIES false
    RELATIVE ${ASSETS_SOURCE_DIR}
    ${ASSETS_SOURCE_DIR}/*
    )

list(REMOVE_ITEM ASSETS_LIST "CMakeLists.txt")

foreach(ASSET ${ASSETS_LIST})
    set(asset_source "${ASSETS_SOURCE_DIR}/${ASSET}")
    get_filename_component(filename "${ASSET}" NAME)
    get_filename_component(dir "${ASSET}" DIRECTORY)

    set(destdir "${ASSETS_DEST_DIR}/${dir}")
    set(outfile "${destdir}/${filename}")

    add_custom_command(
        COMMENT "Copying resource: ${ASSET}"
        OUTPUT ${outfile}
        DEPENDS ${asset_source}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${asset_source}
            ${outfile}
        )
    add_custom_target(${filename} ALL DEPENDS ${ASSET} ${outfile})
endforeach()
