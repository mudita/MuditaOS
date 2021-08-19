add_subdirectory(${BOARD})

target_sources(
        platform

    PRIVATE
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
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(
        platform

    PRIVATE
        module-vfs
)
