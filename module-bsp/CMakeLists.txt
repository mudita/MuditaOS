# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

project(module-bsp VERSION 1.0
        DESCRIPTION "BSP module library")

add_library(${PROJECT_NAME} STATIC)
target_sources(
        module-bsp
    PRIVATE
        bsp/bluetooth/Bluetooth.cpp
        bsp/cellular/bsp_cellular.cpp
        bsp/common.cpp
        bsp/lpm/bsp_lpm.cpp
        devices/Device.cpp
        drivers/dma/DriverDMA.cpp
        drivers/dmamux/DriverDMAMux.cpp
        drivers/gpio/DriverGPIO.cpp
        drivers/i2c/DriverI2C.cpp
        drivers/lpspi/DriverLPSPI.cpp
        drivers/lpuart/DriverLPUART.cpp
        drivers/pll/DriverPLL.cpp
        drivers/pwm/DriverPWM.cpp
        drivers/semc/DriverSEMC.cpp
        drivers/usdhc/DriverUSDHC.cpp
)
add_board_subdirectory(board)

# Board specific compilation definitions,options,include directories and features
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_CONFIG_DEFINITIONS})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_TARGET})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_INCLUDES})
target_compile_options(${PROJECT_NAME}
    PRIVATE
    -Wno-sign-compare
    -Wno-missing-field-initializers
    -Wno-unused-function
    -Wno-switch
    -Wno-implicit-fallthrough
    -Wno-unused-variable

    # C only flags
    $<$<COMPILE_LANGUAGE:C>:-Wno-old-style-declaration>
)

target_compile_definitions(${PROJECT_NAME}

        PUBLIC
        -DFSL_SDK_ENABLE_DRIVER_CACHE_CONTROL=1
        -D__STARTUP_INITIALIZE_NONCACHEDATA
        -D__USE_CMSIS
        -DDEBUG_VCOM=1
        -DDEBUG_USB=1
        )

target_include_directories(${PROJECT_NAME}
        PUBLIC
        $<BUILD_INTERFACE:
            ${CMAKE_CURRENT_SOURCE_DIR}
            ${CMAKE_CURRENT_SOURCE_DIR}/bsp/headset
            ${CMAKE_CURRENT_SOURCE_DIR}/drivers/gpio
            ${CMAKE_CURRENT_SOURCE_DIR}/drivers/i2c
            ${CMAKE_CURRENT_SOURCE_DIR}/bsp
        >
)

add_subdirectory(hal)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        hal
        eventstore
        module-os
        magic_enum
        utility
    PRIVATE
        purefs-paths
        time-constants
)

if (${ENABLE_TESTS})
    add_subdirectory(tests)
endif ()
