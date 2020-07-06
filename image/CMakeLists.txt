set(ASSETS_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(ASSETS_DEST_DIR "${CMAKE_BINARY_DIR}")

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

    if(dir)
        set(destdir "${ASSETS_DEST_DIR}/${dir}")
        install(FILES ${ASSET} DESTINATION ${dir})
    else()
        set(destdir "${ASSETS_DEST_DIR}")
        install(FILES ${ASSET} DESTINATION "./" )
    endif()

    set(outfile "${destdir}/${filename}")
    string(REPLACE "/" "-" target_name ${ASSET})
    if(${target_name} STREQUAL ${ASSET})
        set(target_name "file-${target_name}")
    endif()

    add_custom_command(
        COMMENT "Copying resource: ${ASSET}"
        OUTPUT ${outfile}
        DEPENDS ${asset_source}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${asset_source}
            ${outfile}
        )
    add_custom_target(${target_name} ALL DEPENDS ${outfile})
    if (${ENABLE_TESTS})
        add_dependencies(check ${target_name})
    endif ()
    add_dependencies(assets ${target_name})
endforeach()
