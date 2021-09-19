add_subdirectory(${BOARD})

if (${MEMORY_LINKER_FILE_PATH} STREQUAL "")
    message(FATAL_ERROR "Linker RAM layout not provided")
endif()

set (CMAKE_EXE_LINKER_FLAGS "-nostdlib -Xlinker --gc-sections -Xlinker --sort-section=alignment -mcpu=cortex-m7 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -Xlinker -print-memory-usage -T ${LDSCRIPTSDIR}/libs.ld -T ${MEMORY_LINKER_FILE_PATH} -T ${LDSCRIPTSDIR}/sections.ld -nostartfiles" CACHE INTERNAL "")

target_sources(board
    PRIVATE
        common/board.cpp
        common/chip.cpp
        common/fsl_drivers/fsl_adc.c
        common/fsl_drivers/fsl_cache.c
        common/fsl_drivers/fsl_clock.c
        common/fsl_drivers/fsl_clock.c
        common/fsl_drivers/fsl_common.c
        common/fsl_drivers/fsl_dcdc.c
        common/fsl_drivers/fsl_dmamux.c
        common/fsl_drivers/fsl_edma.c
        common/fsl_drivers/fsl_flexram.c
        common/fsl_drivers/fsl_gpc.c
        common/fsl_drivers/fsl_gpio.c
        common/fsl_drivers/fsl_gpt.c
        common/fsl_drivers/fsl_lpi2c.c
        common/fsl_drivers/fsl_lpspi_edma.c
        common/fsl_drivers/fsl_lpspi.c
        common/fsl_drivers/fsl_lpuart_edma.c
        common/fsl_drivers/fsl_lpuart.c
        common/fsl_drivers/fsl_pit.c
        common/fsl_drivers/fsl_pmu.c
        common/fsl_drivers/fsl_pwm.c
        common/fsl_drivers/fsl_rtwdog.c
        common/fsl_drivers/fsl_sai_edma.c
        common/fsl_drivers/fsl_sai.c
        common/fsl_drivers/fsl_semc.c
        common/fsl_drivers/fsl_snvs_hp.c
        common/fsl_drivers/fsl_snvs_lp.c
        common/fsl_drivers/fsl_src.c
        common/fsl_drivers/fsl_usdhc.c
        common/fsl_drivers/fsl_wdog.c
        common/startup_mimxrt1052.cpp
        common/system_MIMXRT1051.c

        xip/evkbimxrt1050_flexspi_nor_config.c
        xip/evkbimxrt1050_sdram_ini_dcd.c
        xip/fsl_flexspi_nor_boot.c

    PUBLIC
        board/board.h
        board/debug_console.hpp
        board/macros.h
        board/reboot_codes.hpp
)

target_include_directories(board
    PRIVATE
    log-api

    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
	    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/common>
	    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/common/fsl_drivers>
	    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/common/cmsis>
)

