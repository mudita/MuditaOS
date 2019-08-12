set(BOARD_SOURCES ${BOARD_SOURCES}

		"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/drivers/RT1051DriverPLL.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/drivers/RT1051DriverI2C.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/drivers/RT1051DriverSAI.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/drivers/RT1051DriverDMAMux.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/drivers/RT1051DriverDMA.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/fsl_drivers/fsl_lpi2c.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/fsl_drivers/fsl_clock.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/fsl_drivers/fsl_dmamux.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/fsl_drivers/fsl_edma.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/fsl_drivers/fsl_sai.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/fsl_drivers/fsl_sai_edma.c"

        CACHE INTERNAL ""
        )

set(BOARD_DIR_INCLUDES ${BOARD_DIR_INCLUDES}

        CACHE INTERNAL "")


set(BOARD_SOURCE_FILES_PROPERTIES ${BOARD_SOURCE_FILES_PROPERTIES}

        CACHE INTERNAL "")

