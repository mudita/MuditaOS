target_sources(board
    PRIVATE
        _exit.c
        crashdump/consoledump.cpp
        crashdump/crashcatcher_impl.cpp
        crashdump/crashdumpwriter_vfs.cpp
        memwrap.c
        newlib/fs_dir.cc
        newlib/io_syscalls.cpp
        xip/evkbimxrt1050_flexspi_nor_config.c
        xip/evkbimxrt1050_sdram_ini_dcd.c
        xip/fsl_flexspi_nor_boot.c
)

target_include_directories(board
    PUBLIC
        $<BUILD_INTERFACE:
            ${CMAKE_CURRENT_SOURCE_DIR}
            ${CMAKE_CURRENT_SOURCE_DIR}/newlib/include
        >
)

target_link_libraries(board
    PUBLIC
        CrashCatcher::CrashCatcher
)
