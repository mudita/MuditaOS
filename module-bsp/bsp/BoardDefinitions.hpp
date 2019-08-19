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
    BATTERY_CHARGER_GPIO = static_cast<int >(drivers::GPIOInstances ::GPIO_2),
    BATTERY_CHARGER_INOKB_PIN = 12,
    BATTERY_CHARGER_WCINOKB = 13,
    BATTERY_CHARGER_INTB_PIN = 14,

    CELLULAR_DMA = static_cast<int >(drivers::DMAInstances ::DMA_0),
    CELLULAR_DMAMUX = static_cast<int >(drivers::DMAMuxInstances ::DMAMUX0),
    CELLULAR_TX_DMA_CHANNEL = 5,
    CELLULAR_GPIO_1 = static_cast<int >(drivers::GPIOInstances ::GPIO_1),
    CELLULAR_GPIO_2 = static_cast<int >(drivers::GPIOInstances ::GPIO_2),
    CELLULAR_GPIO_3 = static_cast<int >(drivers::GPIOInstances ::GPIO_3),
    CELLULAR_GPIO_1_CTS_PIN = 14,
    CELLULAR_GPIO_1_RTS_PIN = 15,
    CELLULAR_GPIO_3_DTR_PIN = 27,
    CELLULAR_GPIO_2_POWER_PIN = 18,
    CELLULAR_GPIO_2_RESET_PIN = 17,
    CELLULAR_GPIO_2_RI_PIN = 21,
    CELLULAR_GPIO_2_APRDY_PIN = 16,
    CELLULAR_GPIO_2_WAKEUP_PIN = 19,
    CELLULAR_GPIO_2_SIMCARD_1_INSERTED_PIN = 11,
    CELLULAR_GPIO_2_SIMCARD_2_INSERTED_PIN = 10,
    CELLULAR_GPIO_2_SIMCARD_PRESENCE_PIN = 9,

#define BSP_CELLULAR_UART_CTS_PORT                           GPIO1
#define BSP_CELLULAR_UART_CTS_PIN                            14
#define BSP_CELLULAR_UART_CTS_PAD                            GPIO_AD_B0_14

#define BSP_CELLULAR_UART_RTS_PORT                           GPIO1
#define BSP_CELLULAR_UART_RTS_PIN                            15
#define BSP_CELLULAR_UART_RTS_PAD                            GPIO_AD_B0_15

#define BSP_CELLULAR_UART_DTR_PORT                           GPIO3
#define BSP_CELLULAR_UART_DTR_PIN                            27
#define BSP_CELLULAR_UART_DTR_PAD                            GPIO_EMC_41

#define BSP_CELLULAR_POWER_PORT                              GPIO2
#define BSP_CELLULAR_POWER_PIN                               18
#define BSP_CELLULAR_POWER_PAD                               GPIO_B1_02

#define BSP_CELLULAR_RESET_PORT                              GPIO2
#define BSP_CELLULAR_RESET_PIN                               17
#define BSP_CELLULAR_RESET_PAD                               GPIO_B1_01

#define BSP_CELLULAR_RI_PORT                                 GPIO2
#define BSP_CELLULAR_RI_PIN                                  21
#define BSP_CELLULAR_RI_PAD                                  GPIO_B1_05

#define BSP_CELLULAR_AP_RDY_PORT                             GPIO2
#define BSP_CELLULAR_AP_RDY_PIN                              16
#define BSP_CELLULAR_AP_RDY_PAD                              GPIO_B1_00

#define BSP_CELLULAR_WAKEUP_PORT                             GPIO2
#define BSP_CELLULAR_WAKEUP_PIN                              19
#define BSP_CELLULAR_WAKEUP_PAD                              GPIO_B1_03

#define BSP_CELLULAR_SIM_CARD_1_INSERTED_PORT                GPIO2
#define BSP_CELLULAR_SIM_CARD_1_INSERTED_PIN                 11
#define BSP_CELLULAR_SIM_CARD_1_INSERTED_PAD                 GPIO_B0_11

#define BSP_CELLULAR_SIM_CARD_2_INSERTED_PORT                GPIO2
#define BSP_CELLULAR_SIM_CARD_2_INSERTED_PIN                 10
#define BSP_CELLULAR_SIM_CARD_2_INSERTED_PAD                 GPIO_B0_10

#define BSP_CELLULAR_SIM_CARD_PRESENCE_PORT                  GPIO2
#define BSP_CELLULAR_SIM_CARD_PRESENCE_PIN                   9
#define BSP_CELLULAR_SIM_CARD_PRESENCE_PAD                   GPIO_B0_09

#define BSP_CELLULAR_SIM_CARD_SELECTION_PORT                 GPIO2
#define BSP_CELLULAR_SIM_CARD_SELECTION_PIN                  20
#define BSP_CELLULAR_SIM_CARD_SELECTION_PAD                  GPIO_B1_04

    EINK_DMA = static_cast<int >(drivers::DMAInstances ::DMA_0),
    EINK_DMAMUX = static_cast<int >(drivers::DMAMuxInstances ::DMAMUX0),
    EINK_TX_DMA_CHANNEL = 0,
    EINK_RX_DMA_CHANNEL = 1,
    EINK_GPIO = static_cast<int >(drivers::GPIOInstances ::GPIO_3),
    EINK_CS_PIN = 13,
    EINK_RESET_PIN=16,
    EINK_BUSY_PIN=17,



    AUDIO_PLL = static_cast<int >(drivers::PLLInstances::Audio),


};

#endif //PUREPHONE_BOARDDEFINITIONS_HPP
