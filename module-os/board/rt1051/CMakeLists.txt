target_sources(
        module-os
    PRIVATE
        fsl_tickless_gpt.c
        fsl_runtimestat_gpt.c

        _exit.cpp
        __atexit.c
        __call_atexit.c
)

target_include_directories(
        module-os
    PUBLIC
        .
        include
)

target_link_libraries(module-os
    PRIVATE
        module-vfs
        logdump
    PUBLIC
        fsl
        cmsis
)

target_compile_options(module-os PRIVATE "-Wno-prio-ctor-dtor")

# Set required FreeRTOS CMake flags
set(FREERTOS_PORT GCC_ARM_CM7 CACHE STRING "")
set(FREERTOS_CONFIG_FILE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include CACHE STRING "")
