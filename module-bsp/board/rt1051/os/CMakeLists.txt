add_library(module-bsp-os

        __atexit.c
        __call_atexit.c
        _exit.cpp
        fsl_tickless_gpt.c
        fsl_runtimestat_gpt.c
        )

add_library(module-bsp::os ALIAS module-bsp-os)

target_include_directories(module-bsp-os
        PUBLIC
        include
        )

target_compile_options(module-bsp-os PRIVATE "-Wno-prio-ctor-dtor")

target_link_libraries(module-bsp-os
        PRIVATE
        module-vfs
        logdump
        PUBLIC
        fsl
        cmsis
        )

# Set required FreeRTOS CMake flags
set(FREERTOS_PORT GCC_ARM_CM7 CACHE STRING "")
set(FREERTOS_CONFIG_FILE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include CACHE STRING "")





