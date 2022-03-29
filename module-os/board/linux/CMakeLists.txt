target_include_directories(
        module-os
    PUBLIC
    $<BUILD_INTERFACE:
        ${CMAKE_CURRENT_SOURCE_DIR}
    >
)

# Set required FreeRTOS CMake flags
set(FREERTOS_PORT GCC_POSIX CACHE STRING "")
set(FREERTOS_CONFIG_FILE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} CACHE STRING "")
