cmake_minimum_required(VERSION 3.14)

project(lfsfuse LANGUAGES C)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake" ${CMAKE_MODULE_PATH})

find_package(FUSE REQUIRED)

file(GLOB_RECURSE LFSFUSE_SRCS *.c)
add_executable(${PROJECT_NAME} ${LFSFUSE_SRCS})


target_compile_options(${PROJECT_NAME} PRIVATE -Wall -pedantic -Wno-missing-field-initializers -Wno-array-bounds -Wno-maybe-uninitialized)
target_compile_definitions( ${PROJECT_NAME}
        PRIVATE
        _FILE_OFFSET_BITS=64
        _XOPEN_SOURCE=700
        LFS_MIGRATE
)

target_include_directories( ${PROJECT_NAME}
    PRIVATE
    lfsfuse/littlefs
    ${FUSE_INCLUDE_DIR}
)

target_link_libraries( ${PROJECT_NAME} ${FUSE_LIBRARIES} )
