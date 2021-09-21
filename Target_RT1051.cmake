SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_SYSTEM_VERSION 1)

set(TOOLCHAIN_PREFIX "arm-none-eabi")

if(WIN32)
    set(CMAKE_C_COMPILER ${TOOLCHAIN_PATH}/bin/${TOOLCHAIN_PREFIX}-gcc.exe CACHE INTERNAL "")
    set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PATH}/bin/${TOOLCHAIN_PREFIX}-g++.exe CACHE INTERNAL "")
    set(CMAKE_STRIP ${TOOLCHAIN_PATH}/bin/${TOOLCHAIN_PREFIX}-strip.exe CACHE INTERNAL "")
    set(CMAKE_OBJCOPY ${TOOLCHAIN_PATH}/bin/${TOOLCHAIN_PREFIX}-objcopy.exe CACHE INTERNAL "")
else()
    if (${TOOLCHAIN_PATH})
        string(APPEND ${TOOLCHAIN_PATH} "/")
    endif()
    set(CMAKE_C_COMPILER ${TOOLCHAIN_PATH}${TOOLCHAIN_PREFIX}-gcc CACHE INTERNAL "")
    set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PATH}${TOOLCHAIN_PREFIX}-g++ CACHE INTERNAL "")
    set(CMAKE_STRIP ${TOOLCHAIN_PATH}${TOOLCHAIN_PREFIX}-strip CACHE INTERNAL "")
    set(CMAKE_OBJCOPY ${TOOLCHAIN_PATH}${TOOLCHAIN_PREFIX}-objcopy CACHE INTERNAL "")
endif()

set(LDSCRIPTSDIR "${CMAKE_CURRENT_LIST_DIR}/board/rt1051/ldscripts" CACHE INTERNAL "")

set(PROJECT_TARGET "TARGET_RT1051" CACHE INTERNAL "")

include_directories(${CMAKE_SOURCE_DIR}/board/rt1051/newlib/include)

add_compile_definitions(
        CPU_MIMXRT1051DVL6B_cm7
        CPU_MIMXRT1051DVL6B
        __MCUXPRESSO
        __USE_CMSIS
        __NEWLIB__
        SKIP_SYSCLK_INIT
        _HAVE_SQLITE_CONFIG_H
        CPP_FREERTOS_NO_EXCEPTIONS
        _GLIBCXX_HAVE_DIRENT_H
)

add_compile_options(
        -mcpu=cortex-m7
        -mthumb
        -mfloat-abi=hard
        -mfpu=fpv5-sp-d16
        -fsingle-precision-constant
        -ffunction-sections
        -fdata-sections
        -MMD
        -MP
        -fno-builtin
        -mno-unaligned-access
        -Wno-psabi

        $<$<COMPILE_LANGUAGE:CXX>:-Wno-register> # "register" keyword was marked as deprecated in C++17
        $<$<COMPILE_LANGUAGE:CXX>:-Wno-literal-suffix>
)

# where is the target environment
SET(CMAKE_FIND_ROOT_PATH  ${TOOLCHAIN_PATH}/${TOOLCHAIN_PREFIX})
# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
