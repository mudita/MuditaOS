cmake_minimum_required(VERSION 3.14)

project(genlittlefs LANGUAGES C)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake" ${CMAKE_MODULE_PATH})

find_package(FDISK REQUIRED)

set(GENLITTLEFS_SRCS
    mklfs.c
)

add_executable(${PROJECT_NAME} ${GENLITTLEFS_SRCS})

target_link_libraries(${PROJECT_NAME} PRIVATE littlefs ${FDISK_LIBRARIES})
target_include_directories(${PROJECT_NAME} ${FDISK_INCLUDE_DIR})
