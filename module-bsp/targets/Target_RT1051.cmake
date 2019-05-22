set(BOARD_SOURCES ${BOARD_SOURCES}

        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/fsl_drivers/fsl_cache.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/fsl_drivers/fsl_clock.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/fsl_drivers/fsl_common.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/fsl_drivers/fsl_dcdc.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/fsl_drivers/fsl_dmamux.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/fsl_drivers/fsl_edma.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/fsl_drivers/fsl_gpio.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/fsl_drivers/fsl_lpspi.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/fsl_drivers/fsl_lpspi_edma.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/fsl_drivers/fsl_lpuart.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/fsl_drivers/fsl_lpuart_edma.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/fsl_drivers/fsl_pit.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/fsl_drivers/fsl_pmu.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/fsl_drivers/fsl_semc.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/fsl_drivers/fsl_usdhc.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/fsl_drivers/fsl_lpi2c.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/fsl_drivers/fsl_sai.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/fsl_drivers/fsl_sai_edma.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/fsl_drivers/fsl_flexram.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/fsl_drivers/fsl_snvs_hp.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/fsl_drivers/fsl_gpt.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/fsl_drivers/fsl_gpc.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/fsl_drivers/fsl_src.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/fsl_drivers/fsl_adc.c"


        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/eMMC/fsl_mmc.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/eMMC/fsl_sdmmc_common.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/eMMC/fsl_sdmmc_event.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/eMMC/fsl_sdmmc_host.c"

        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/i2c.c"

        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/keyboard/keyboard.c"

        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/board.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/clock_config.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/pin_mux.c"

        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/startup_mimxrt1052.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/system_MIMXRT1051.c"


        CACHE INTERNAL ""
        )

set(BOARD_DIR_INCLUDES ${BOARD_DIR_INCLUDES}

            ${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051
            ${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/fsl_drivers
            ${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/eMMC


        CACHE INTERNAL "")


