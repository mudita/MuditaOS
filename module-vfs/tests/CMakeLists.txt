cmake_minimum_required(VERSION 3.12)

project(unittest_module-vfs VERSION 1.0
        DESCRIPTION "VFS module library")


add_executable( ${PROJECT_NAME} EXCLUDE_FROM_ALL
        "${CMAKE_CURRENT_LIST_DIR}/unittest.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/tests-main.cpp"
        )


target_link_libraries( ${PROJECT_NAME} PRIVATE module-vfs )

add_test(NAME ${PROJECT_NAME}
         COMMAND ${PROJECT_NAME}
         WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
         )
add_dependencies(check ${PROJECT_NAME})

set(ASSETS_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/test_dir")
set(ASSETS_TARGET_DIR "${CMAKE_BINARY_DIR}/module-vfs/test_dir")

file (GLOB_RECURSE ASSETS_LIST
    LIST_DIRECTORIES false
    RELATIVE ${ASSETS_SOURCE_DIR}
    ${ASSETS_SOURCE_DIR}/*
)
message ("test assets: ${ASSETS_LIST}")

foreach(ASSET ${ASSETS_LIST})
    set(asset_source "${ASSETS_SOURCE_DIR}/${ASSET}")
    get_filename_component(filename "${ASSET}" NAME)
    get_filename_component(dir "${ASSET}" DIRECTORY)
    
    set(destdir "${ASSETS_TARGET_DIR}/${dir}")
    set(outfile "${destdir}/${filename}")

    add_custom_command(
        COMMENT "Copying tests resources ${ASSET}"
        OUTPUT  ${outfile}
        DEPENDS ${asset_source}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${asset_source}
            ${outfile}
        )

    string(REPLACE "/" "_" target_name ${ASSET})
    add_custom_target(${target_name} DEPENDS ${ASSET} ${outfile})
    add_dependencies(${PROJECT_NAME} ${target_name})
endforeach()

