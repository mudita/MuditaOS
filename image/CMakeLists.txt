set(ASSETS_SOURCE "assets")
set(DB_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}")
set(SYS_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}")

set(SYS_DEST "${CMAKE_BINARY_DIR}/sys")
set(ASSETS_DEST "${SYS_DEST}/assets")
set(DB_DEST "${SYS_DEST}/db")

add_custom_target(copy_assets ALL
    COMMAND ${CMAKE_COMMAND}
        -DSOURCE=${SYS_SOURCE}
        -DDESTINATION=${SYS_DEST}
        -DGLOB_EXPR="*"
        -P ${CMAKE_CURRENT_SOURCE_DIR}/copy_assets.cmake
    COMMENT "Copyingt assets"
    )
