
set(TEST_SYSROOT ${CMAKE_BINARY_DIR}/test-sysroot)
add_custom_target("test_disk_image")
add_custom_command(
        PRE_BUILD
        TARGET test_disk_image
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/gendisktestimg.sh ${CMAKE_BINARY_DIR}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

add_catch2_executable(
    NAME vfs-disk
    SRCS
        ${CMAKE_CURRENT_LIST_DIR}/unittest_disk_manager.cpp
    LIBS
        platform
    DEPS
        test_disk_image
    USE_FS
)

add_catch2_executable(
    NAME vfs-core-fs
    SRCS
        ${CMAKE_CURRENT_LIST_DIR}/unittest_filesystem_core.cpp
    LIBS
        platform
        module-vfs
    USE_FS
)

set(LITTLEFS_IMAGE "lfstest.img")
add_custom_target(
    ${LITTLEFS_IMAGE}
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/genlfstestimg.sh 1G ${LITTLEFS_IMAGE} ${TEST_SYSROOT}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    DEPENDS genlittlefs test-assets
)

add_catch2_executable(
    NAME vfs-littlefs
    SRCS
        ${CMAKE_CURRENT_LIST_DIR}/unittest_filesystem_littlefs.cpp
    LIBS
        platform
        module-vfs
    DEPS
        ${LITTLEFS_IMAGE}
    USE_FS
)

add_catch2_executable(
    NAME vfs-dualmount
    SRCS
        ${CMAKE_CURRENT_LIST_DIR}/unittest_filesystem_dualmount.cpp
    LIBS
        platform
        module-vfs
    DEPS
        ${LITTLEFS_IMAGE}
    USE_FS
)

# iosyscalls tests
add_catch2_executable(
    NAME
        iosyscalls
    SRCS
        unittest_iosys.cpp
    LIBS
        module-sys
        module-vfs
        platform
    USE_FS
)

# prepare test assets
set(ASSETS_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/test_dir")
set(ASSETS_TARGET_DIR "${CMAKE_BINARY_DIR}/module-platform/test_dir")

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
    add_dependencies(platform ${target_name})
endforeach()
