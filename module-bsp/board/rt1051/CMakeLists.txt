# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md


target_sources(module-bsp
	PRIVATE
		bluetooth/BlueKitchen.cpp
		bluetooth/BluetoothCommon.cpp
		bsp/audio/CodecMAX98090.cpp
		bsp/audio/qfilter.c
		bsp/cellular/rt1051_cellular.cpp
		bsp/eeprom/eeprom.cpp
		bsp/eink_frontlight/eink_frontlight.cpp
		bsp/eink/bsp_eink.cpp
		bsp/eink/ED028TC1.cpp
		bsp/eink/eink_binarization_luts.c
		bsp/eMMC/fsl_mmc.c
		bsp/eMMC/fsl_sdmmc_common.c
		bsp/eMMC/fsl_sdmmc_event.c
		bsp/eMMC/fsl_sdmmc_host.c
		bsp/headset/headset.cpp
		bsp/keypad_backlight/keypad_backlight.cpp
		bsp/light_sensor/light_sensor.cpp
		bsp/lpm/ClockState.cpp
		bsp/lpm/CpuFreqLPM.cpp
		bsp/lpm/Oscillator.cpp
		bsp/lpm/RT1051LPMCommon.cpp
		bsp/magnetometer/ALS31300.cpp
		bsp/pit/pit.cpp
		bsp/rtc/rtc.cpp
		bsp/torch/torch.cpp
		bsp/vibrator/vibrator.cpp
		bsp/watchdog/watchdog.cpp
		common/board.cpp
		common/chip.cpp
		common/startup_mimxrt1052.cpp
		common/system_MIMXRT1051.c
		drivers/RT1051DriverDMA.cpp
		drivers/RT1051DriverDMAMux.cpp
		drivers/RT1051DriverGPIO.cpp
		drivers/RT1051DriverI2C.cpp
		drivers/RT1051DriverLPSPI.cpp
		drivers/RT1051DriverLPUART.cpp
		drivers/RT1051DriverOscillator.cpp
		drivers/RT1051DriverPLL.cpp
		drivers/RT1051DriverPLL2.cpp
		drivers/RT1051DriverPWM.cpp
		drivers/RT1051DriverSEMC.cpp
		drivers/RT1051DriverUSDHC.cpp
   		bsp/magnetometer/magnetometer.cpp
)

target_include_directories(module-bsp
	PUBLIC
	$<BUILD_INTERFACE:
		${CMAKE_CURRENT_SOURCE_DIR}
		${CMAKE_CURRENT_SOURCE_DIR}/bsp
		${CMAKE_CURRENT_SOURCE_DIR}/common

		${CMAKE_SOURCE_DIR}/module-bluetooth/Bluetooth
		${CMAKE_SOURCE_DIR}/module-sys/
	>
)

set_source_files_properties(
	eMMC/fsl_mmc.c PROPERTIES COMPILE_FLAGS -Wno-unused-function
	common/i2c.c PROPERTIES COMPILE_FLAGS -Wno-unused-function
)

target_compile_definitions(module-bsp PUBLIC USB_STACK_FREERTOS)

add_subdirectory(common/fsl_drivers)
add_subdirectory(${BOARD})

target_link_libraries(module-bsp PUBLIC cmsis fsl)
