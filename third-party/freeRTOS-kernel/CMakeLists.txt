add_subdirectory(freeRTOS-kernel)

if (${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/freeRTOS-kernel/portable/MemMang/heap_4.c PROPERTIES COMPILE_OPTIONS "-O0")
endif ()

target_link_libraries(freertos_kernel module-os)
