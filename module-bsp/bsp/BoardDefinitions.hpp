/*
 *  @file BoardDefinitions.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 09.08.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#ifndef PUREPHONE_BOARDDEFINITIONS_HPP
#define PUREPHONE_BOARDDEFINITIONS_HPP

#include "drivers/dmamux/DriverDMAMux.hpp"
#include "drivers/dma/DriverDMA.hpp"
#include "drivers/i2c/DriverI2C.hpp"
#include "drivers/pll/DriverPLL.hpp"
#include "drivers/gpio/DriverGPIO.hpp"

enum class BoardDefinitions{
    AUDIOCODEC_I2C_BAUDRATE = 100000,
    AUDIOCODEC_I2C = static_cast<int >(drivers::I2CInstances ::I2C2),
    AUDIOCODEC_DMAMUX = static_cast<int >(drivers::DMAMuxInstances ::DMAMUX0),
    AUDIOCODEC_DMA = static_cast<int >(drivers::DMAInstances ::DMA_0),
    AUDIOCODEC_TX_DMA_CHANNEL = 6,
    AUDIOCODEC_RX_DMA_CHANNEL = 7,

    CELLULAR_AUDIO_DMAMUX = AUDIOCODEC_DMAMUX,
    CELLULAR_AUDIO_DMA = AUDIOCODEC_DMA,
    CELLULAR_AUDIO_TX_DMA_CHANNEL = 3,
    CELLULAR_AUDIO_RX_DMA_CHANNEL = 4,

    KEYBOARD_I2C_BAUDRATE = AUDIOCODEC_I2C_BAUDRATE,
    KEYBOARD_I2C = AUDIOCODEC_I2C,
    KEYBOARD_GPIO = static_cast<int >(drivers::GPIOInstances ::GPIO_2),
    KEYBOARD_RF_BUTTON = 6,
    KEYBOARD_IRQ_PIN = 28,
    KEYBOARD_RESET_PIN = 29,

    BATTERY_CHARGER_I2C_BAUDRATE = AUDIOCODEC_I2C_BAUDRATE,
    BATTERY_CHARGER_I2C = AUDIOCODEC_I2C,

    CELLULAR_DMA = static_cast<int >(drivers::DMAInstances ::DMA_0),
    CELLULAR_DMAMUX = static_cast<int >(drivers::DMAMuxInstances ::DMAMUX0),
    CELLULAR_TX_DMA_CHANNEL = 5,

    EINK_DMA = static_cast<int >(drivers::DMAInstances ::DMA_0),
    EINK_DMAMUX = static_cast<int >(drivers::DMAMuxInstances ::DMAMUX0),
    EINK_TX_DMA_CHANNEL = 0,
    EINK_RX_DMA_CHANNEL = 1,



    AUDIO_PLL = static_cast<int >(drivers::PLLInstances::Audio),


};

#endif //PUREPHONE_BOARDDEFINITIONS_HPP
