cmake_minimum_required(VERSION 3.14)

project(genlittlefs LANGUAGES C)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake" ${CMAKE_MODULE_PATH})

find_package(BLKID REQUIRED)

set(GENLITTLEFS_SRCS
    mklfs.c
    parse_partitions.c
)

add_executable(${PROJECT_NAME} ${GENLITTLEFS_SRCS})

target_link_libraries(${PROJECT_NAME} PRIVATE littlefs ${BLKID_LIBRARIES})
target_include_directories(${PROJECT_NAME} PRIVATE ${BLKID_INCLUDE_DIR})

