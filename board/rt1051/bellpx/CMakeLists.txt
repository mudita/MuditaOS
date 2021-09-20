target_compile_definitions(board
   PUBLIC
       PROJECT_CONFIG_USER_DYNMEM_SIZE=9*1024*1024
       HW_SDRAM_64_MB=0
)

target_include_directories(board
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_sources(board
    PUBLIC
        board/BoardDefinitions.hpp
        board/clock_config.h
        board/pin_mux.h
        eink-config.h

    PRIVATE
        clock_config.cpp
        irq_init.cpp
        pin_mux.c
)

set (MEMORY_LINKER_FILE_PATH "${CMAKE_CURRENT_LIST_DIR}/memory_bell_P1.ld" CACHE INTERNAL "")
