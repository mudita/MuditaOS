add_subdirectory(${BOARD})

target_sources(
        platform

    PRIVATE
        _exit.c
        memwrap.c

        crashdump/consoledump.cpp
        crashdump/crashcatcher_impl.cpp
        crashdump/crashdumpwriter_vfs.cpp

        newlib/fs_dir.cc
        newlib/io_syscalls.cpp

        src/BlockDeviceFactory.hpp
        src/disk_eeprom.cpp
        src/disk_eeprom.hpp
        src/disk_emmc.cpp
        src/disk_emmc.hpp
        src/RT1051Platform.cpp

    PUBLIC
        include/platform/rt1051/RT1051Platform.hpp
)

target_include_directories(
        platform

    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    PUBLIC
        $<BUILD_INTERFACE:
           ${CMAKE_CURRENT_SOURCE_DIR}/include
           ${CMAKE_CURRENT_SOURCE_DIR}/newlib/include
        >
)

target_link_libraries(
        platform

    PUBLIC
        CrashCatcher::CrashCatcher

    PRIVATE
        module-vfs
        log-api
)

