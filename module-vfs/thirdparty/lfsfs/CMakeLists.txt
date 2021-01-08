cmake_minimum_required(VERSION 3.14)

project(littlefs LANGUAGES C)

include(thirdparty OPTIONAL)

set(LIBLITTLEFS_SRCDIR littlefs)
set(LIBLITTLEFS_INCDIR include)
set(LIBLITTLEFS_TARGET ${PROJECT_NAME})

set(LIBLITTLEFS_SOURCES
        ${LIBLITTLEFS_SRCDIR}/lfs.c
        ${LIBLITTLEFS_SRCDIR}/lfs_util.c
        ${LIBLITTLEFS_SRCDIR}/bd/lfs_filebd.c
        ${LIBLITTLEFS_SRCDIR}/bd/lfs_rambd.c
        ${LIBLITTLEFS_SRCDIR}/bd/lfs_testbd.c
)

set( LIBLITTLEFS_PUBLIC_INCLUDES  ${LIBLITTLEFS_INCDIR} )
set( LIBLITTLEFS_PRIVATE_INCLUDES ${LIBLITTLEFS_SRCDIR} )


add_library( ${LIBLITTLEFS_TARGET} STATIC ${LIBLITTLEFS_SOURCES} )

#target_compile_definitions(${LIBLITTLEFS_TARGET} PRIVATE LFS_YES_TRACE )
target_compile_definitions(${LIBLITTLEFS_TARGET} PUBLIC LFS_THREADSAFE )

if( COMMAND third_party_target_setup )
    third_party_target_setup( ${LIBLITTLEFS_TARGET} )
endif()

target_include_directories( ${LIBLITTLEFS_TARGET}
        PUBLIC
        ${LIBLITTLEFS_PUBLIC_INCLUDES}
        PRIVATE
        ${LIBLITTLEFS_PRIVATE_INCLUDES}
)
if( COMMAND third_party_source_optimization )
    third_party_source_optimization( ${LIBLITTLEFS_SOURCES} )
endif()
