configure_file(eink/config.h.in eink-config.h)

target_sources(module-bsp
        PRIVATE
                board.cpp

                battery_charger/battery_charger.cpp
                bluetooth/Bluetooth.cpp
                bluetooth/test/bsp_bt.cpp
                cellular/linux_cellular.cpp
                eeprom/eeprom.cpp
                eink_frontlight/eink_frontlight.cpp
                eink/ED028TC1.c
                headset/headset.cpp
                keypad_backlight/keypad_backlight.cpp
                light_sensor/light_sensor.cpp
                lpm/LinuxLPM.cpp
                lpm/PowerProfile.cpp
                magnetometer/magnetometer.cpp
                rtc/rtc.cpp
                torch/torch.cpp
                usb_cdc/usb_cdc.cpp
                vibrator/vibrator.cpp
                watchdog/software_watchdog.cpp
                watchdog/watchdog.cpp
                hal/temperature_source/TemperatureSource.cpp
                hal/battery_charger/BatteryCharger.cpp
                hal/key_input/KeyInput.cpp
                bell_temp_sensor/bell_temp_sensor.cpp

                ${CMAKE_CURRENT_BINARY_DIR}/eink-config.h
)

target_include_directories(
                module-bsp
        PUBLIC
                $<BUILD_INTERFACE:
                        ${CMAKE_CURRENT_SOURCE_DIR}
                        ${CMAKE_CURRENT_SOURCE_DIR}/audio
                        ${CMAKE_CURRENT_SOURCE_DIR}/common
                        ${CMAKE_CURRENT_SOURCE_DIR}/eink
                        ${CMAKE_CURRENT_SOURCE_DIR}/pwr

                        ${CMAKE_CURRENT_BINARY_DIR}

                        ${CMAKE_SOURCE_DIR}/module-bluetooth/Bluetooth
                        ${CMAKE_SOURCE_DIR}/module-sys/
                >
)

target_link_libraries(module-bsp PRIVATE sys-common)

set_source_files_properties(board.cpp PROPERTIES COMPILE_FLAGS -Wno-unused-function)

target_compile_options(module-bsp PUBLIC "-Wno-unused-result")

add_library(system-stats-sink-board)
target_sources(system-stats-sink-board
    PRIVATE
      sink/RTTSink.cpp
)
target_link_libraries(system-stats-sink-board
    PRIVATE
        segger::rtt
        system-stats-sink-interface
)
