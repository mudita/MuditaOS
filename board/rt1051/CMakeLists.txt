add_library(board STATIC)

add_subdirectory(cmsis)
add_subdirectory(${BOARD})
add_subdirectory(crashdump-serial-number)

if (${MEMORY_LINKER_FILE_PATH} STREQUAL "")
    message(FATAL_ERROR "Linker RAM layout not provided")
endif()

set (CMAKE_EXE_LINKER_FLAGS "-nostdlib -Xlinker --gc-sections -Xlinker --sort-section=alignment -mcpu=cortex-m7 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -Xlinker -print-memory-usage -T ${LDSCRIPTSDIR}/libs.ld -T ${MEMORY_LINKER_FILE_PATH} -T ${LDSCRIPTSDIR}/sections.ld -nostartfiles" CACHE INTERNAL "")

if(("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" AND ${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 12))
    set(GCC_CXX_DIR_ITERATOR "newlib/gcc11/fs_dir.cc")
else()
    set(GCC_CXX_DIR_ITERATOR "newlib/gcc12/fs_dir.cc")
endif()


target_sources(board
    PRIVATE
        memcpy-armv7m.S
        crashdump/consoledump.cpp
        crashdump/crashcatcher_impl.cpp
        crashdump/crashdumpwriter_vfs.cpp
        memwrap.c
        newlib/io_syscalls.cpp
        newlib/cxx_guards.cpp
        xip/evkbimxrt1050_flexspi_nor_config.c
        xip/evkbimxrt1050_sdram_ini_dcd.c
        xip/fsl_flexspi_nor_boot.c
        ${GCC_CXX_DIR_ITERATOR}
)


target_include_directories(board
    PUBLIC
        $<BUILD_INTERFACE:
            ${CMAKE_CURRENT_SOURCE_DIR}
        >
)

target_link_libraries(board
    PRIVATE
        utils-rotator
        crashdump-serial-number
    PUBLIC
        fsl
        module-vfs
        log-api
        CrashCatcher::CrashCatcher
)
