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

    PUBLIC
        eink-config.h
)
