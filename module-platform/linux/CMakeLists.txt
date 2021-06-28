target_sources(
        platform

    PRIVATE
        src/BlockDeviceFactory.cpp
        src/BlockDeviceFactory.hpp
        src/DiskImage.cpp
        src/LinuxPlatform.cpp

    PUBLIC
        include/platform/linux/DiskImage.hpp
        include/platform/linux/LinuxPlatform.hpp
)

target_include_directories(
        platform
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(
        platform
    PRIVATE
        iosyscalls
        module-vfs
)

if (${ENABLE_TESTS})
    add_subdirectory(tests)
endif ()
