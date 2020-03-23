set(USB_CDC_SRC

		"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/usb_cdc/driver/device/class/usb_device_class.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/usb_cdc/driver/device/class/hid/usb_device_hid.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/usb_cdc/driver/device/source/usb_device_ch9.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/usb_cdc/driver/device/source/usb_device_dci.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/usb_cdc/driver/device/source/ehci/usb_device_ehci.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/usb_cdc/driver/phy/usb_phy.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/usb_cdc/driver/cdc/virtual_com.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/usb_cdc/driver/cdc/usb_device_descriptor.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/usb_cdc/driver/cdc/usb_device_cdc_acm.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/usb_cdc/driver/osa/usb_osa_freertos.c"

		CACHE INTERNAL ""
		)

set(USB_CDC_DIR_INCLUDES

		"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/usb_cdc/driver/device/class"
		"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/usb_cdc/driver/device/class/hid"
		"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/usb_cdc/driver/device/include"
		"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/usb_cdc/driver/device/source"
		"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/usb_cdc/driver/device/source/ehci"
		"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/usb_cdc/driver/include"
		"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/usb_cdc/driver/phy"
		"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/usb_cdc/driver/osa"

		CACHE INTERNAL ""
		)

set(BOARD_SOURCES ${BOARD_SOURCES}

        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/fsl_drivers/fsl_cache.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/fsl_drivers/fsl_clock.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/fsl_drivers/fsl_common.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/fsl_drivers/fsl_dcdc.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/fsl_drivers/fsl_gpio.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/fsl_drivers/fsl_lpspi.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/fsl_drivers/fsl_lpspi_edma.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/fsl_drivers/fsl_lpuart.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/fsl_drivers/fsl_lpuart_edma.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/fsl_drivers/fsl_pit.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/fsl_drivers/fsl_pmu.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/fsl_drivers/fsl_semc.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/fsl_drivers/fsl_usdhc.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/fsl_drivers/fsl_sai.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/fsl_drivers/fsl_sai_edma.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/fsl_drivers/fsl_flexram.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/fsl_drivers/fsl_snvs_hp.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/fsl_drivers/fsl_gpt.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/fsl_drivers/fsl_gpc.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/fsl_drivers/fsl_src.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/fsl_drivers/fsl_adc.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/fsl_drivers/fsl_lpi2c.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/fsl_drivers/fsl_clock.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/fsl_drivers/fsl_dmamux.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/fsl_drivers/fsl_edma.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/fsl_drivers/fsl_sai.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/fsl_drivers/fsl_sai_edma.c"

        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/irq/irq_gpio.cpp"

        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/board.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/clock_config.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/pin_mux.c"

        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/startup_mimxrt1052.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/system_MIMXRT1051.c"


        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/eMMC/fsl_mmc.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/eMMC/fsl_sdmmc_common.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/eMMC/fsl_sdmmc_event.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/eMMC/fsl_sdmmc_host.c"
        
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/eink/bsp_eink.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/eink/ED028TC1.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/eink/eink_binarization_luts.c"


        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/keyboard/keyboard.cpp"

        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/cellular/rt1051_cellular.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/pit/pit.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/harness/rt1051_harness.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/harness/rt1051_harness_gpio.cpp"

		"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/vibrator/vibrator.cpp"

		"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/lpm/RT1051LPM.cpp"

        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bluetooth/BluetoothCommon.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bluetooth/BlueKitchen.cpp"

		"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/rtc/rtc.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/battery-charger/battery_charger.cpp"
		
		#"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/pwr/lpm.c"

		"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/audio/RT1051Audiocodec.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/audio/RT1051CellularAudio.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/audio/CodecMAX98090.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/audio/qfilter.c"

		"${USB_CDC_SRC}"
		"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/usb_cdc/usb_cdc.cpp"


		"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/drivers/RT1051DriverPLL.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/drivers/RT1051DriverI2C.cpp"
		#"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/drivers/RT1051DriverSAI.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/drivers/RT1051DriverDMAMux.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/drivers/RT1051DriverDMA.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/drivers/RT1051DriverGPIO.cpp"


        CACHE INTERNAL ""
        )

set(BOARD_DIR_INCLUDES ${BOARD_DIR_INCLUDES}

			${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common
            ${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/fsl_drivers
            ${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/cmsis
            ${CMAKE_SOURCE_DIR}/module-bluetooth/Bluetooth
            ${CMAKE_SOURCE_DIR}/module-sys/
			${USB_CDC_DIR_INCLUDES}

        CACHE INTERNAL "")


set(BOARD_SOURCE_FILES_PROPERTIES ${BOARD_SOURCE_FILES_PROPERTIES}

        ${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/eMMC/fsl_mmc.c PROPERTIES COMPILE_FLAGS -Wno-unused-function
        ${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/i2c.c PROPERTIES COMPILE_FLAGS -Wno-unused-function

        CACHE INTERNAL "")

