SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_SYSTEM_VERSION 1)

set(TOOLCHAIN_PREFIX "arm-none-eabi")

# specify the cross compiler

if(WIN32)
    set(CMAKE_C_COMPILER ${TOOLCHAIN_PATH}/bin/${TOOLCHAIN_PREFIX}-gcc.exe CACHE INTERNAL "")
    set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PATH}/bin/${TOOLCHAIN_PREFIX}-g++.exe CACHE INTERNAL "")
    set(CMAKE_STRIP ${TOOLCHAIN_PATH}/bin/${TOOLCHAIN_PREFIX}-strip.exe CACHE INTERNAL "")
    set(CMAKE_OBJCOPY ${TOOLCHAIN_PATH}/bin/${TOOLCHAIN_PREFIX}-objcopy.exe CACHE INTERNAL "")
else()
    #make shure that if TOOLCHAIN_PATH exists finishes with /
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
	-Werror
        -Wno-psabi

        $<$<COMPILE_LANGUAGE:CXX>:-Wno-register> # "register" keyword was marked as deprecated in C++17
        $<$<COMPILE_LANGUAGE:CXX>:-Wno-literal-suffix>
        $<$<COMPILE_LANGUAGE:CXX>:-Wno-register> # "register" 


        CACHE INTERNAL ""

        )

set(TARGET_COMPILE_FEATURES

        CACHE INTERNAL "" )


set(TARGET_SOURCES

        ${CMAKE_CURRENT_LIST_DIR}/board/rt1051/_exit.c
        ${CMAKE_CURRENT_LIST_DIR}/board/rt1051/newlib/io_syscalls.cpp
        ${CMAKE_CURRENT_LIST_DIR}/board/rt1051/newlib/fs_dir.cc
        ${CMAKE_CURRENT_LIST_DIR}/board/rt1051/memwrap.c
        ${CMAKE_CURRENT_LIST_DIR}/board/rt1051/xip/evkbimxrt1050_flexspi_nor_config.c
        ${CMAKE_CURRENT_LIST_DIR}/board/rt1051/xip/evkbimxrt1050_sdram_ini_dcd.c
        ${CMAKE_CURRENT_LIST_DIR}/board/rt1051/xip/fsl_flexspi_nor_boot.c
        ${CMAKE_CURRENT_LIST_DIR}/board/rt1051/crashdump/crashcatcher_impl.cpp
        ${CMAKE_CURRENT_LIST_DIR}/board/rt1051/crashdump/crashdumpwriter_vfs.cpp
        ${CMAKE_CURRENT_LIST_DIR}/board/rt1051/crashdump/consoledump.cpp
        CACHE INTERNAL ""
        )

set(TARGET_DIR_INCLUDES

        ${CMAKE_CURRENT_LIST_DIR}/board/rt1051
        CACHE INTERNAL "" )

set(BOARD_DIR_INCLUDES
        ${CMAKE_CURRENT_LIST_DIR}/board/rt1051/newlib/include
        ${CMAKE_SOURCE_DIR}/module-vfs/include/internal
)

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
