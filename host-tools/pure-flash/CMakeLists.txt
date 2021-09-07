cmake_minimum_required(VERSION 3.14)

project(pureflash LANGUAGES C)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake" ${CMAKE_MODULE_PATH})

set(PUREFLASH_SRCS
    pure-flash.c
)

add_executable(${PROJECT_NAME} ${PUREFLASH_SRCS})
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -pedantic -Werror -Wextra )

target_compile_definitions(${PROJECT_NAME} PRIVATE _GNU_SOURCE _DEFAULT_SOURCE)

install(TARGETS pureflash
    RUNTIME DESTINATION .
)
