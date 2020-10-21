set(USB_SRC

        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/usb/component/lists/generic_list.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/usb/component/osa/fsl_os_abstraction_free_rtos.c"

        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/usb/device/ehci/usb_device_ehci.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/usb/device/usb_device_ch9.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/usb/device/usb_device_class.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/usb/device/usb_device_dci.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/usb/device/usb_string_descriptor.c"

        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/usb/cdc/usb_device_cdc_acm.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/usb/cdc/virtual_com.c"


        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/usb/mtp/libmtp/mtp_container.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/usb/mtp/libmtp/mtp_dataset.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/usb/mtp/libmtp/mtp_responder.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/usb/mtp/libmtp/mtp_storage.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/usb/mtp/libmtp/mtp_util.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/usb/mtp/mtp.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/usb/mtp/mtp_db.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/usb/mtp/mtp_fs.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/usb/mtp/usb_device_mtp.c"

        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/usb/phy/usb_phy.c"


        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/usb/usb_device_descriptor.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/usb/composite.c"

        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/usb/virtual_com_demo.c"

        CACHE INTERNAL ""
        )

set(USB_DIR_INCLUDES

        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/usb"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/usb/cdc"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/usb/component/lists"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/usb/component/osa"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/usb/device"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/usb/device/ehci"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/usb/mtp"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/usb/mtp/libmtp"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/usb/phy"

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
	"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/fsl_drivers/fsl_snvs_lp.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/fsl_drivers/fsl_gpt.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/fsl_drivers/fsl_gpc.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/fsl_drivers/fsl_src.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/fsl_drivers/fsl_adc.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/fsl_drivers/fsl_lpi2c.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/fsl_drivers/fsl_clock.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/fsl_drivers/fsl_dmamux.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/fsl_drivers/fsl_edma.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/fsl_drivers/fsl_wdog.c"
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
	"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/headset/headset.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/cellular/rt1051_cellular.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/pit/pit.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/harness/rt1051_harness.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/harness/rt1051_harness_gpio.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/vibrator/vibrator.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/watchdog/watchdog.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/lpm/RT1051LPM.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bluetooth/BluetoothCommon.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bluetooth/BlueKitchen.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/rtc/rtc.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/battery-charger/battery_charger.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/audio/RT1051Audiocodec.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/audio/RT1051CellularAudio.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/audio/CodecMAX98090.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/audio/qfilter.c"
	"${USB_SRC}"
	"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/usb_cdc/usb_cdc.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/drivers/RT1051DriverPLL.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/drivers/RT1051DriverI2C.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/drivers/RT1051DriverDMAMux.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/drivers/RT1051DriverDMA.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/drivers/RT1051DriverGPIO.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/magnetometer/magnetometer.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/torch/torch.cpp"

	CACHE INTERNAL ""
)

set(BOARD_DIR_INCLUDES ${BOARD_DIR_INCLUDES}

	${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp
	${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common
	${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/fsl_drivers
	${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/cmsis
	${CMAKE_CURRENT_SOURCE_DIR}/bsp
	${CMAKE_CURRENT_SOURCE_DIR}/drivers/i2c
	${CMAKE_CURRENT_SOURCE_DIR}/drivers/gpio
	${CMAKE_CURRENT_SOURCE_DIR}/bsp/headset
	${CMAKE_SOURCE_DIR}/module-bluetooth/Bluetooth
	${CMAKE_SOURCE_DIR}/module-sys/
	${USB_DIR_INCLUDES}

	CACHE INTERNAL ""
)


set(BOARD_SOURCE_FILES_PROPERTIES ${BOARD_SOURCE_FILES_PROPERTIES}

	${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/eMMC/fsl_mmc.c PROPERTIES COMPILE_FLAGS -Wno-unused-function
	${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/common/i2c.c PROPERTIES COMPILE_FLAGS -Wno-unused-function

	CACHE INTERNAL ""
)

