SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_SYSTEM_VERSION 1)



set(TOOLCHAIN_PREFIX "arm-none-eabi")

# specify the cross compiler

if(WIN32)
        set (CMAKE_C_COMPILER ${TOOLCHAIN_PATH}/bin/${TOOLCHAIN_PREFIX}-gcc.exe CACHE INTERNAL "")
        set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PATH}/bin/${TOOLCHAIN_PREFIX}-g++.exe CACHE INTERNAL "")
else()
        set (CMAKE_C_COMPILER ${TOOLCHAIN_PATH}/bin/${TOOLCHAIN_PREFIX}-gcc CACHE INTERNAL "")
        set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PATH}/bin/${TOOLCHAIN_PREFIX}-g++ CACHE INTERNAL "")
endif()

set(LDSCRIPTSDIR "${CMAKE_CURRENT_LIST_DIR}/board/rt1051/ldscripts" CACHE INTERNAL "")

set(PROJECT_TARGET "TARGET_RT1051" CACHE INTERNAL "")


set(TARGET_COMPILE_DEFINITIONS

        -DCPU_MIMXRT1051DVL6B_cm7
        -DCPU_MIMXRT1051DVL6B
        -D__MCUXPRESSO
        -D__USE_CMSIS
        -D__NEWLIB__
        -DSKIP_SYSCLK_INIT
        -D_HAVE_SQLITE_CONFIG_H
        CPP_FREERTOS_NO_EXCEPTIONS


        CACHE INTERNAL ""
        )

set(TARGET_COMPILE_OPTIONS

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

        $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
        $<$<COMPILE_LANGUAGE:CXX>:-fno-non-call-exceptions>
        $<$<COMPILE_LANGUAGE:CXX>:-Wno-literal-suffix>
        $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>
        $<$<COMPILE_LANGUAGE:CXX>:-Wno-register> # "register" 


        CACHE INTERNAL ""

        )

set(TARGET_COMPILE_FEATURES

        cxx_noexcept

        CACHE INTERNAL "" )


set(TARGET_SOURCES

        ${CMAKE_CURRENT_LIST_DIR}/board/rt1051/_exit.c
        ${CMAKE_CURRENT_LIST_DIR}/board/rt1051/memwrap.c
        ${CMAKE_CURRENT_LIST_DIR}/board/rt1051/xip/evkbimxrt1050_flexspi_nor_config.c
        ${CMAKE_CURRENT_LIST_DIR}/board/rt1051/xip/evkbimxrt1050_sdram_ini_dcd.c
        ${CMAKE_CURRENT_LIST_DIR}/board/rt1051/xip/fsl_flexspi_nor_boot.c

        CACHE INTERNAL ""
        )

set(TARGET_DIR_INCLUDES

        ${CMAKE_CURRENT_LIST_DIR}/board/rt1051

        CACHE INTERNAL "" )

set(TARGET_LIBRARIES

        CACHE INTERNAL ""
        )

set(TARGET_LINK_OPTIONS

        CACHE INTERNAL ""
        )


# where is the target environment
SET(CMAKE_FIND_ROOT_PATH  ${TOOLCHAIN_PATH}/${TOOLCHAIN_PREFIX})
# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)



set(CMAKE_EXE_LINKER_FLAGS "-nostdlib -Xlinker --gc-sections -Xlinker --sort-section=alignment -mcpu=cortex-m7 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -Xlinker -print-memory-usage -T ${LDSCRIPTSDIR}/libs.ld -T ${LDSCRIPTSDIR}/memory.ld -T ${LDSCRIPTSDIR}/sections.ld -nostartfiles" CACHE INTERNAL "")
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH)
