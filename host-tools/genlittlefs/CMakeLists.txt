cmake_minimum_required(VERSION 3.14)

project(genlittlefs LANGUAGES C)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake" ${CMAKE_MODULE_PATH})

find_package(BLKID REQUIRED)

# only add LittleFS subdirectory during a standalone config of genlittlefs
if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../third-party/littlefs
        ${CMAKE_CURRENT_BINARY_DIR}/littlefs EXCLUDE_FROM_ALL
    )
endif()

set(GENLITTLEFS_SRCS
    mklfs.c
    parse_partitions.c
    parse_args.c
    lfs_ioaccess.c
)

add_executable(${PROJECT_NAME} ${GENLITTLEFS_SRCS})
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -pedantic -Werror -Wextra )
target_compile_definitions(${PROJECT_NAME} PRIVATE _GNU_SOURCE )

target_link_libraries(${PROJECT_NAME} PRIVATE littlefs ${BLKID_LIBRARIES})
target_include_directories(${PROJECT_NAME} PRIVATE ${BLKID_INCLUDE_DIR})

