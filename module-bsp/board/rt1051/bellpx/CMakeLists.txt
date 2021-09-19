target_include_directories(
        module-bsp
    PUBLIC
        $<BUILD_INTERFACE:
            ${CMAKE_CURRENT_SOURCE_DIR}
        >
)

target_sources(
        module-bsp

    PRIVATE
        hal/temperature_source/TemperatureSource.cpp
        hal/battery_charger/BatteryCharger.cpp
        hal/key_input/KeyInput.cpp
        bsp/eink/eink_pin_config.cpp
        bsp/switches/switches.cpp
        bsp/rotary_encoder/rotary_encoder.cpp
        bsp/bell_temp_sensor/bell_temp_sensor.cpp
        pin_mux.c
        clock_config.cpp
        irq_gpio.cpp
        debug_console.cpp
)
