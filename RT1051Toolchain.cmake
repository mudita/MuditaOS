SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_SYSTEM_VERSION 1)

set(TOOLCHAIN_PREFIX "arm-none-eabi")

set(TOOLCHAIN_PATH  "/usr" CACHE INTERNAL "")

# specify the cross compiler
set (CMAKE_C_COMPILER ${TOOLCHAIN_PATH}/bin/${TOOLCHAIN_PREFIX}-gcc CACHE INTERNAL "")
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PATH}/bin/${TOOLCHAIN_PREFIX}-g++ CACHE INTERNAL "")


set(BOARD_COMPILE_DEFINITIONS

        -DCPU_MIMXRT1051DVL6B_cm7
        -DCPU_MIMXRT1051DVL6B


        CACHE INTERNAL ""
        )

set(BOARD_COMPILE_OPTIONS

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

        CACHE INTERNAL ""
        )

set(BOARD_COMPILE_FEATURES

        CACHE INTERNAL "")


set(BOARD_SOURCES

        CACHE INTERNAL ""
        )

set(BOARD_DIR_INCLUDES board/rt1051 CACHE INTERNAL "")


# where is the target environment
SET(CMAKE_FIND_ROOT_PATH  ${TOOLCHAIN_PATH}/${TOOLCHAIN_PREFIX})
# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)


set(CMAKE_EXE_LINKER_FLAGS "--specs=nosys.specs" CACHE INTERNAL "")
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH)
