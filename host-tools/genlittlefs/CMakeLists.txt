cmake_minimum_required(VERSION 3.14)

project(genlittlefs LANGUAGES C)

set(GENLITTLEFS_SRCS
    mklfs.c
)

add_executable(${PROJECT_NAME} ${GENLITTLEFS_SRCS})

target_link_libraries(${PROJECT_NAME} PRIVATE littlefs)
