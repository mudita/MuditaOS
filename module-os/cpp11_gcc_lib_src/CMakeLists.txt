cmake_minimum_required(VERSION 3.0)
project (rtosstdthread CXX)

#set(TOOLCHAIN_PATH "/home/rockford/gcc-arm-none-eabi-8-2019-q3-update")
#set(TOOLCHAIN_PREFIX "arm-none-eabi")
#
#set(CMAKE_C_COMPILER ${TOOLCHAIN_PATH}/bin/${TOOLCHAIN_PREFIX}-gcc CACHE INTERNAL "")
#set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PATH}/bin/${TOOLCHAIN_PREFIX}-g++ CACHE INTERNAL "")
#
set(pure_dir "/home/rockford/mudita/PurePhone")
include_directories("${pure_dir}/module-os/FreeRTOS/include")
include_directories("${pure_dir}/module-os/board/rt1051")
include_directories("${pure_dir}/module-utils")

set(SOURCES 
    condition_variable.cpp
    freertos_time.cpp
    future.cc
    gthr_key.cpp
    mutex.cc
    thread.cpp
)

set(CMAKE_CXX_STANDARD 11)
add_compile_definitions(_GCC_MULTITHREAD_FREERTOS_ENABLE)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

