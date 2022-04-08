add_library(module-bsp-os INTERFACE)
add_library(module-bsp::os ALIAS module-bsp-os)
target_include_directories(module-bsp-os INTERFACE .)

# Set required FreeRTOS CMake flags
set(FREERTOS_PORT GCC_POSIX CACHE STRING "")
set(FREERTOS_CONFIG_FILE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} CACHE STRING "")
