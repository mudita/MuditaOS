set(BOARD_SOURCES ${BOARD_SOURCES}

        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/fsl_drivers/fsl_cache.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/fsl_drivers/fsl_clock.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/fsl_drivers/fsl_common.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/fsl_drivers/fsl_dcdc.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/fsl_drivers/fsl_dmamux.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/fsl_drivers/fsl_edma.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/fsl_drivers/fsl_gpio.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/fsl_drivers/fsl_lpspi.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/fsl_drivers/fsl_lpspi_edma.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/fsl_drivers/fsl_lpuart.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/fsl_drivers/fsl_lpuart_edma.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/fsl_drivers/fsl_pit.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/fsl_drivers/fsl_pmu.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/fsl_drivers/fsl_semc.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/fsl_drivers/fsl_usdhc.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/fsl_drivers/fsl_lpi2c.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/fsl_drivers/fsl_sai.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/fsl_drivers/fsl_sai_edma.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/fsl_drivers/fsl_flexram.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/fsl_drivers/fsl_snvs_hp.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/fsl_drivers/fsl_gpt.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/fsl_drivers/fsl_gpc.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/fsl_drivers/fsl_src.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/fsl_drivers/fsl_adc.c"

        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/i2c.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/irq/irq_gpio.cpp"

        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/i2c.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/irq/irq_gpio.cpp"

        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/board.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/clock_config.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/pin_mux.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/dma_config.c"

        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/startup_mimxrt1052.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/system_MIMXRT1051.c"


        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/eMMC/fsl_mmc.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/eMMC/fsl_sdmmc_common.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/eMMC/fsl_sdmmc_event.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/eMMC/fsl_sdmmc_host.c"
        
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/eink/bsp_eink.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/eink/ED028TC1.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/eink/eink_binarization_luts.c"


        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/keyboard/keyboard.cpp"

        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/cellular/rt1051_cellular.cpp"

        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/keyboard/keyboard.cpp"

		"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/rtc/rtc.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/battery-charger/battery_charger.cpp"
		
		"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/pwr/lpm.c"

        CACHE INTERNAL ""
        )

set(BOARD_DIR_INCLUDES ${BOARD_DIR_INCLUDES}

            ${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051
            ${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common
            ${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/fsl_drivers
            ${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/cmsis
            ${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/eMMC
            ${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/eink
            ${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/pwr


        CACHE INTERNAL "")


set(BOARD_SOURCE_FILES_PROPERTIES ${BOARD_SOURCE_FILES_PROPERTIES}

        ${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/eMMC/fsl_mmc.c PROPERTIES COMPILE_FLAGS -Wno-unused-function
        ${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/i2c.c PROPERTIES COMPILE_FLAGS -Wno-unused-function

        CACHE INTERNAL "")

