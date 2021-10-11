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
        hal/key_input/KeyInput.cpp
        bsp/battery_charger/battery_charger.cpp
        bsp/eink/eink_pin_config.cpp
        bsp/keyboard/keyboard.cpp
        bsp/lpm/PowerProfile.cpp
        pin_mux.c
        clock_config.cpp
        irq_gpio.cpp
        debug_console.cpp
        
    PUBLIC
        eink-config.h
        board/pin_mux.h
        board/clock_config.h
        board/irq_gpio.hpp
        board/BoardDefinitions.hpp
)
