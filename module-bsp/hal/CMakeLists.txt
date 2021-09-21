add_library(hal INTERFACE)

target_include_directories(hal
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
)

target_sources(hal
    INTERFACE
        include/hal/temperature_source/TemperatureSource.hpp
        include/hal/GenericFactory.hpp
        include/hal/battery_charger/AbstractBatteryCharger.hpp
	include/hal/cellular/SIM.hpp
        include/hal/key_input/RawKey.hpp
        include/hal/key_input/KeyEventDefinitions.hpp
        include/hal/key_input/AbstractKeyInput.hpp
)
