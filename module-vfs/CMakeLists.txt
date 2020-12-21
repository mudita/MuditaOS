cmake_minimum_required(VERSION 3.12)
include(thirdparty)


project(module-vfs VERSION 1.0
        DESCRIPTION "VFS module library")

add_subdirectory(thirdparty/lfsfs)

set(FREERTOS_FAT_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/board/freeRTOS_FAT/ff_crc.c
        ${CMAKE_CURRENT_SOURCE_DIR}/board/freeRTOS_FAT/ff_dir.c
        ${CMAKE_CURRENT_SOURCE_DIR}/board/freeRTOS_FAT/ff_error.c
        ${CMAKE_CURRENT_SOURCE_DIR}/board/freeRTOS_FAT/ff_fat.c
        ${CMAKE_CURRENT_SOURCE_DIR}/board/freeRTOS_FAT/ff_file.c
        ${CMAKE_CURRENT_SOURCE_DIR}/board/freeRTOS_FAT/ff_format.c
        ${CMAKE_CURRENT_SOURCE_DIR}/board/freeRTOS_FAT/ff_ioman.c
        ${CMAKE_CURRENT_SOURCE_DIR}/board/freeRTOS_FAT/ff_locking.c
        ${CMAKE_CURRENT_SOURCE_DIR}/board/freeRTOS_FAT/ff_memory.c
        ${CMAKE_CURRENT_SOURCE_DIR}/board/freeRTOS_FAT/ff_stdio.c
        ${CMAKE_CURRENT_SOURCE_DIR}/board/freeRTOS_FAT/ff_string.c
        ${CMAKE_CURRENT_SOURCE_DIR}/board/freeRTOS_FAT/ff_sys.c
        ${CMAKE_CURRENT_SOURCE_DIR}/board/freeRTOS_FAT/ff_time.c
        )

third_party_source_optimization(${FREERTOS_FAT_SOURCES})


set(FF_FAT_SOURCES_THIRDPARTY
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/fatfs/source/ff.c
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/fatfs/source/ffunicode.c
)
third_party_source_optimization(${FREERTOS_FAT_SOURCES_THIRDPARTY})

set(FF_FAT_SOURCES
    ${FF_FAT_SOURCES_THIRDPARTY}
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/src/thirdparty/fatfs/ffsystem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/src/thirdparty/fatfs/ff_glue.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/src/thirdparty/littlefs/lfs_glue.cpp
)


# Disable some warnings due to ugly embedded hacks
# in the FREERTOS FAT implementation
if(${PROJECT_TARGET}  STREQUAL "TARGET_Linux")
set_property(SOURCE ${FREERTOS_FAT_SOURCES} APPEND_STRING PROPERTY COMPILE_FLAGS
" -Wno-overflow -Wno-type-limits -Wno-format -Wno-int-to-pointer-cast -Wno-pointer-to-int-cast -Wno-stringop-truncation")
endif()

set(PROJECT_INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}/board/freeRTOS_FAT/include
        ${CMAKE_CURRENT_SOURCE_DIR}/board/free_rtos_custom/include
        ${CMAKE_CURRENT_SOURCE_DIR}/freertos-fat-custom/include
)

set(FREERTOS_FAT_EXTSOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/freertos-fat-custom/src/ff_stdio_flush.c
	${CMAKE_CURRENT_SOURCE_DIR}/freertos-fat-custom/src/ff_file_flush.c
    ${CMAKE_CURRENT_SOURCE_DIR}/freertos-fat-custom/src/ff_stdio_listdir_recursive.c
)

set(SOURCES ""
        ${FREERTOS_FAT_SOURCES}
        ${FREERTOS_FAT_EXTSOURCES}
        ${FF_FAT_SOURCES}
        src/purefs/filesystem_paths.cpp
        src/purefs/blkdev/disk_manager.cpp
        src/purefs/blkdev/disk.cpp
        src/purefs/blkdev/partition_parser.cpp
        src/purefs/blkdev/disk_handle.cpp
        src/purefs/fs/filesystem.cpp
        src/purefs/fs/filesystem_operations.cpp
        src/purefs/fs/filesystem_syscalls.cpp
        src/purefs/fs/filesystem_cwd.cpp
        src/purefs/vfs_subsystem.cpp
        drivers/src/purefs/fs/filesystem_vfat.cpp
        drivers/src/purefs/fs/filesystem_littlefs.cpp
        src/deprecated/vfs.cpp
        src/deprecated/vfs-utils.cpp
        src/deprecated/vfsNotifier.cpp
)

if(NOT ${PROJECT_TARGET}  STREQUAL "TARGET_Linux")
    include(targets/Target_Cross.cmake)
endif()

if(${PROJECT_TARGET} STREQUAL "TARGET_RT1051")
    include(targets/Target_RT1051.cmake)
elseif(${PROJECT_TARGET} STREQUAL "TARGET_Linux")
    include(targets/Target_Linux.cmake)
else()
    message(FATAL_ERROR "Invalid target!")
endif()

add_library(${PROJECT_NAME} STATIC ${SOURCES} ${BOARD_SOURCES})


# Board specific compilation definitions,options,include directories and features
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_CONFIG_DEFINITIONS})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_TARGET})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_DEFINITIONS})
target_include_directories(${PROJECT_NAME} PUBLIC ${BOARD_DIR_INCLUDES})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_INCLUDES})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_BINARY_DIR})
target_compile_features(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_FEATURES})
target_compile_options(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_OPTIONS})
target_link_options(${PROJECT_NAME} PUBLIC ${TARGET_LINK_OPTIONS})

target_compile_definitions(${PROJECT_NAME}
        PUBLIC
)

target_include_directories(${PROJECT_NAME}
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include/user/deprecated
        ${CMAKE_CURRENT_SOURCE_DIR}/include/user/
        ${CMAKE_CURRENT_SOURCE_DIR}/include/internal/
        ${CMAKE_CURRENT_SOURCE_DIR}/drivers/include
)

target_include_directories(${PROJECT_NAME}
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/drivers/include/thirdparty/fatfs
        ${CMAKE_CURRENT_SOURCE_DIR}/drivers/include/thirdparty
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/fatfs/source
)

target_link_libraries(${PROJECT_NAME} PUBLIC ${TARGET_LIBRARIES} module-bsp module-utils)
target_link_libraries(${PROJECT_NAME} PRIVATE littlefs )
# TODO: Temporary only remove when removing old vfs
target_link_libraries(${PROJECT_NAME} PUBLIC utils-bootconfig )

if (${ENABLE_TESTS})
    add_subdirectory(tests)
endif ()
