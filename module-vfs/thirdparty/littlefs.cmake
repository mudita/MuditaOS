include(thirdparty)

set(LIBLITTLEFS_TARGET littlefs)


set(LIBLITTLEFS_SRCDIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/littlefs)

set(LIBLITTLEFS_SOURCES
        ${LIBLITTLEFS_SRCDIR}/lfs.c
        ${LIBLITTLEFS_SRCDIR}/lfs_util.c
        ${LIBLITTLEFS_SRCDIR}/bd/lfs_filebd.c
        ${LIBLITTLEFS_SRCDIR}/bd/lfs_rambd.c
        ${LIBLITTLEFS_SRCDIR}/bd/lfs_testbd.c
)

set( LIBLITTLEFS_PUBLIC_INCLUDES  ${LIBLITTLEFS_SRCDIR})
set( LIBLITTLEFS_PRIVATE_INCLUDES ${LIBLITTLEFS_SRCDIR}/bd)


add_library( ${LIBLITTLEFS_TARGET} STATIC ${LIBLITTLEFS_SOURCES} )
third_party_target_setup( ${LIBLITTLEFS_TARGET} )

target_include_directories( ${LIBLITTLEFS_TARGET}
        PUBLIC
        ${LIBLITTLEFS_PUBLIC_INCLUDES}
        PRIVATE
        ${LIBLITTLEFS_PRIVATE_INCLUDES}
)

third_party_source_optimization( ${LIBLITTLEFS_SOURCES} )
