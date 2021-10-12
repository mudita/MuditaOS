add_library(board STATIC)

add_subdirectory(cmsis)
add_subdirectory(${BOARD})

if (${MEMORY_LINKER_FILE_PATH} STREQUAL "")
    message(FATAL_ERROR "Linker RAM layout not provided")
endif()

set (CMAKE_EXE_LINKER_FLAGS "-nostdlib -Xlinker --gc-sections -Xlinker --sort-section=alignment -mcpu=cortex-m7 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -Xlinker -print-memory-usage -T ${LDSCRIPTSDIR}/libs.ld -T ${MEMORY_LINKER_FILE_PATH} -T ${LDSCRIPTSDIR}/sections.ld -nostartfiles" CACHE INTERNAL "")

target_sources(board
    PRIVATE
        crashdump/consoledump.cpp
        crashdump/crashcatcher_impl.cpp
        crashdump/crashdumpwriter_vfs.cpp
        memwrap.c
        newlib/fs_dir.cc
        newlib/io_syscalls.cpp
        newlib/cxx_guards.cpp
        xip/evkbimxrt1050_flexspi_nor_config.c
        xip/evkbimxrt1050_sdram_ini_dcd.c
        xip/fsl_flexspi_nor_boot.c
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
    PUBLIC
        fsl
        module-vfs
        log-api
        CrashCatcher::CrashCatcher
)
