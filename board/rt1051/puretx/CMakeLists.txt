if (${BOARD_REVISION} EQUAL 7)
    set (MEMORY_LINKER_FILE_PATH "${CMAKE_CURRENT_LIST_DIR}/memory_pure_T7.ld" CACHE INTERNAL "")
    target_compile_definitions(
        board
            PUBLIC
        PROJECT_CONFIG_USER_DYNMEM_SIZE=28*1024*1024
        HW_SDRAM_64_MB=1
    )
elseif (${BOARD_REVISION} EQUAL 6)
    set (MEMORY_LINKER_FILE_PATH "${CMAKE_CURRENT_LIST_DIR}/memory_pure_T6.ld" CACHE INTERNAL "")
    target_compile_definitions(
        board
            PUBLIC
        PROJECT_CONFIG_USER_DYNMEM_SIZE=9*1024*1024
        HW_SDRAM_64_MB=0
    )
else ()
    message (FATAL_ERROR "Unsupported board revision selected!")
endif ()

target_include_directories(
        board
    PUBLIC
        $<BUILD_INTERFACE:
            ${CMAKE_CURRENT_SOURCE_DIR}
        >
)

target_sources(
	board
		PRIVATE
	eink-config.h
)
