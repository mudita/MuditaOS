foreach(check_dir SOURCE DESTINATION GLOB_EXPR)
    if(NOT DEFINED ${check_dir})
        message(FATAL_ERROR "${check_dir} is not defined on command line")
    endif()
endforeach()

message("source: ${SOURCE}")
message("destination: ${DESTINATION}")
message("glob_expr: ${GLOB_EXPR}")

file(GLOB_RECURSE ASSETS_LIST
    LIST_DIRECTORIES false
    RELATIVE ${SOURCE}
    ${SOURCE}/${GLOB_EXPR}
    )

list(REMOVE_ITEM ASSETS_LIST "CMakeLists.txt")
list(REMOVE_ITEM ASSETS_LIST "copy_assets.cmake")

message("assets: ${ASSETS_LIST}")

foreach(File IN LISTS ASSETS_LIST)
    set(DEST ${DESTINATION}/${File})
    message("Coying file: \"${File}\" to ${DEST}")
    configure_file("${SOURCE}/${File}" "${DEST}" COPYONLY)
endforeach()
