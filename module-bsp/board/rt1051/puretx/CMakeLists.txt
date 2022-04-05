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
        hal/battery_charger/BatteryCharger.cpp
        hal/battery_charger/CurrentMeasurementScope.cpp
        hal/key_input/KeyInput.cpp
        bsp/battery_charger/battery_charger.cpp
        bsp/eink/eink_pin_config.cpp
        bsp/keyboard/keyboard.cpp
        bsp/lpm/PowerProfile.cpp
        bsp/lpm/RT1051LPM.cpp
        bsp/rtc/rtc_configuration.cpp

        audio.cpp
        pin_mux.c
        clock_config.cpp
        irq_gpio.cpp
        debug_console.cpp
        board.cpp

    PUBLIC
        eink-config.h
        board/pin_mux.h
        board/clock_config.h
        board/irq_gpio.hpp
        board/BoardDefinitions.hpp
)

if((${PROJECT_TARGET} STREQUAL "TARGET_RT1051") AND (${CURRENT_MEASUREMENT} STREQUAL "ON"))
    set (ENABLE_CURRENT_MEASUREMENT_SCOPE 1 CACHE INTERNAL "")
else()
    set (ENABLE_CURRENT_MEASUREMENT_SCOPE 0 CACHE INTERNAL "")
endif()

target_compile_definitions(module-bsp
    PRIVATE
        ENABLE_CURRENT_MEASUREMENT_SCOPE=${ENABLE_CURRENT_MEASUREMENT_SCOPE}
)
