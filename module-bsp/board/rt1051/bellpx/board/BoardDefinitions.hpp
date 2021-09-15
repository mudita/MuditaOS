// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "drivers/dmamux/DriverDMAMux.hpp"
#include "drivers/dma/DriverDMA.hpp"
#include "drivers/i2c/DriverI2C.hpp"
#include "drivers/pll/DriverPLL.hpp"
#include "drivers/gpio/DriverGPIO.hpp"
#include "drivers/lpspi/DriverLPSPI.hpp"
#include "drivers/usdhc/DriverUSDHC.hpp"
#include "drivers/lpuart/DriverLPUART.hpp"

enum class BoardDefinitions
{

    POWER_SWITCH_HOLD_GPIO   = static_cast<int>(drivers::GPIOInstances ::GPIO_2),
    POWER_SWITCH_HOLD_BUTTON = 7,

    USB_FUNCTION_MUX_SELECT = 25, // GPIO_AD_B1_09, USB_MUX_SEL0
    USB_POWER_ACK           = 3,  // GPIO_B0_03  Note: pull-up in order to read
    I2C_STD_BAUDRATE        = 100000,

    AUDIOCODEC_I2C_BAUDRATE   = I2C_STD_BAUDRATE,
    AUDIOCODEC_I2C            = static_cast<int>(drivers::I2CInstances ::I2C2),
    AUDIOCODEC_DMAMUX         = static_cast<int>(drivers::DMAMuxInstances ::DMAMUX0),
    AUDIOCODEC_DMA            = static_cast<int>(drivers::DMAInstances ::DMA_0),
    AUDIOCODEC_TX_DMA_CHANNEL = 6,
    AUDIOCODEC_RX_DMA_CHANNEL = 7,
    AUDIOCODEC_IRQ            = 31, // GPIO_B1_15  requires pull-up 10kΩ
    AUDIOCODEC_IRQ_GPIO       = static_cast<int>(drivers::GPIOInstances ::GPIO_2),

    MIC_BIAS_DRIVER_GPIO = static_cast<int>(drivers::GPIOInstances ::GPIO_2),
    MIC_BIAS_DRIVER_EN   = 19, // GPIO_B1_03 MIC_LDO_EN

    CELLULAR_AUDIO_DMAMUX         = AUDIOCODEC_DMAMUX,
    CELLULAR_AUDIO_DMA            = AUDIOCODEC_DMA,
    CELLULAR_AUDIO_TX_DMA_CHANNEL = 2,
    CELLULAR_AUDIO_RX_DMA_CHANNEL = 3,

    KEYBOARD_I2C_BAUDRATE = AUDIOCODEC_I2C_BAUDRATE,
    KEYBOARD_I2C          = AUDIOCODEC_I2C,
    KEYBOARD_GPIO         = static_cast<int>(drivers::GPIOInstances ::GPIO_2),
    KEYBOARD_RF_BUTTON    = 6,
    KEYBOARD_IRQ_PIN      = 28,
    KEYBOARD_RESET_PIN    = 29,

    HEADSET_I2C_BAUDRATE = AUDIOCODEC_I2C_BAUDRATE,
    HEADSET_I2C          = AUDIOCODEC_I2C,
    HEADSET_GPIO         = static_cast<int>(drivers::GPIOInstances ::GPIO_2),
    HEADSET_IRQ_PIN      = 30,

    BATTERY_CHARGER_I2C_BAUDRATE = AUDIOCODEC_I2C_BAUDRATE,
    BATTERY_CHARGER_I2C          = AUDIOCODEC_I2C,
    BATTERY_CHARGER_GPIO         = static_cast<int>(drivers::GPIOInstances ::GPIO_2),
    BATTERY_CHARGER_INOKB_PIN    = 12,
    BATTERY_CHARGER_WCINOKB      = 13, // UNUSABLE as WCIN is not connected to the charger !
    BATTERY_CHARGER_INTB_PIN     = 14, // interrupt on charger/fuel-gauge event

    CELLULAR_DMA               = static_cast<int>(drivers::DMAInstances ::DMA_0),
    CELLULAR_DMAMUX            = static_cast<int>(drivers::DMAMuxInstances ::DMAMUX0),
    CELLULAR_TX_DMA_CHANNEL    = 4,
    CELLULAR_RX_DMA_CHANNEL    = 5,
    CELLULAR_GPIO_1            = static_cast<int>(drivers::GPIOInstances ::GPIO_1),
    CELLULAR_GPIO_2            = static_cast<int>(drivers::GPIOInstances ::GPIO_2),
    CELLULAR_GPIO_3            = static_cast<int>(drivers::GPIOInstances ::GPIO_3),
    CELLULAR_GPIO_1_CTS_PIN    = 14,
    CELLULAR_GPIO_1_RTS_PIN    = 15,
    CELLULAR_GPIO_3_DTR_PIN    = 27,
    CELLULAR_GPIO_2_POWER_PIN  = 18,
    CELLULAR_GPIO_2_RESET_PIN  = 17,
    CELLULAR_GPIO_2_RI_PIN     = 21,
    CELLULAR_GPIO_2_APRDY_PIN  = 16,
    CELLULAR_GPIO_2_WAKEUP_PIN = 22, // GPIO_B1_06, long time no see. Active low, external pull-down 10kΩ. docs: 6. Do
                                     // not pull up WAKEUP_IN, […], pins unless the module starts up sucessfully.
    CELLULAR_GPIO_2_WIRELESS_DISABLE_PIN  = 23, // GPIO_B1_07, pull-up in modem, active low, equiv. AT+CFUN=4, see docs
    CELLULAR_GPIO_2_SIM_TRAY_INSERTED_PIN = 11,
    CELLULAR_GPIO_2_NC                    = 10, // GPIO_B0_10
    CELLULAR_GPIO_2_SIMCARD_PRESENCE_PIN  = 9,
    CELLULAR_GPIO_2_SIMSEL_PIN            = 20, // GPIO_B1_04
    CELLULAR_GPIO_2_ANTSEL_PIN            = 2,
    CELLULAR_GPIO_1_STATUS_PIN            = 2,  // GPIO_AD_B0_02
    CELLULAR_GPIO_2_USB_BOOT_PIN          = 24, // GPIO_B1_08, output
    CELLULAR_LPUART_INSTANCE              = static_cast<int>(drivers::LPUARTInstances::LPUART_1),

    EINK_DMA            = static_cast<int>(drivers::DMAInstances ::DMA_0),
    EINK_DMAMUX         = static_cast<int>(drivers::DMAMuxInstances ::DMAMUX0),
    EINK_TX_DMA_CHANNEL = 0,
    EINK_RX_DMA_CHANNEL = 1,
    EINK_GPIO           = static_cast<int>(drivers::GPIOInstances ::GPIO_3),
    EINK_CS_PIN         = 13,
    EINK_RESET_PIN      = 16,
    EINK_BUSY_PIN       = 17,
    EINK_PLL            = static_cast<int>(drivers::PLLInstances::PLL2_PFD2),
    EINK_LPSPI_INSTANCE = static_cast<int>(drivers::LPSPIInstances::LPSPI_1),
    EINK_BELL_PWR_GPIO  = static_cast<int>(drivers::GPIOInstances ::GPIO_1),
    EINK_BELL_PWR_PIN   = 19,

    BLUETOOTH_DMA            = static_cast<int>(drivers::DMAInstances ::DMA_0),
    BLUETOOTH_DMAMUX         = static_cast<int>(drivers::DMAMuxInstances ::DMAMUX0),
    BLUETOOTH_TX_DMA_CHANNEL = 8,
    BLUETOOTH_RX_DMA_CHANNEL = 9,

    EMMC_PLL            = static_cast<int>(drivers::PLLInstances::PLL2_PFD2),
    EMMC_USDHC_INSTANCE = static_cast<int>(drivers::USDHCInstances::USDHC_2),

    AUDIO_PLL = static_cast<int>(drivers::PLLInstances::PLL4_Audio),

    VIBRATOR_GPIO = static_cast<int>(drivers::GPIOInstances::GPIO_1),
    VIBRATOR_EN   = 0, // GPIO_AD_B0_00

    MAGNETOMETER_I2C          = AUDIOCODEC_I2C,
    MAGNETOMETER_I2C_BAUDRATE = AUDIOCODEC_I2C_BAUDRATE,
    MAGNETOMETER_GPIO         = static_cast<int>(drivers::GPIOInstances::GPIO_1),
    MAGNETOMETER_IRQ          = 20, // GPIO_AD_B1_04

    TORCH_DRIVER_I2C_BAUDRATE = AUDIOCODEC_I2C_BAUDRATE, // can do 400 kHz
    TORCH_DRIVER_I2C          = AUDIOCODEC_I2C,
    TORCH_DRIVER_GPIO         = static_cast<int>(drivers::GPIOInstances::GPIO_1),
    TORCH_DRIVER_EN = 21, // GPIO_AD_B1_05 Active HIGH. Internal pulldown resistor of 300kΩ between HWEN and GND.

    KEYPAD_BACKLIGHT_DRIVER_I2C_BAUDRATE = AUDIOCODEC_I2C_BAUDRATE,
    KEYPAD_BACKLIGHT_DRIVER_I2C          = AUDIOCODEC_I2C,
    KEYPAD_BACKLIGHT_DRIVER_GPIO         = static_cast<int>(drivers::GPIOInstances::GPIO_1),
    KEYPAD_BACKLIGHT_DRIVER_NRST =
        3, // GPIO_AD_B0_03 Active LOW. External pulldown resistor of 10kΩ between NRST and GND.

    EINK_FRONTLIGHT_PWM_INSTANCE = 4, // GPIO_B1_15 = FLEXPWM4_PWM3_A
    EINK_FRONTLIGHT_PWM_MODULE   = 3,
    EINK_FRONTLIGHT_PWM_CHANNEL  = 0, // A

    LIGHT_SENSOR_I2C          = AUDIOCODEC_I2C,
    LIGHT_SENSOR_I2C_BAUDRATE = AUDIOCODEC_I2C_BAUDRATE,
    LIGHT_SENSOR_IRQ_GPIO     = static_cast<int>(drivers::GPIOInstances::GPIO_2),
    LIGHT_SENSOR_IRQ          = 15, // GPIO_B0_15

    DCDC_INVERTER_MODE_GPIO = static_cast<int>(drivers::GPIOInstances ::GPIO_2),
    DCDC_INVERTER_MODE_PIN  = 26,

    EEPROM_I2C          = AUDIOCODEC_I2C,
    EEPROM_I2C_BAUDRATE = I2C_STD_BAUDRATE,

    BELL_TEMP_SENSOR_I2C          = static_cast<int>(drivers::I2CInstances ::I2C4),
    BELL_TEMP_SENSOR_I2C_BAUDRATE = I2C_STD_BAUDRATE,

    BELL_SWITCHES_GPIO   = static_cast<int>(drivers::GPIOInstances ::GPIO_2),
    BELL_SWITCHES_CENTER = 16, // GPIO_B1_00
    BELL_SWITCHES_LEFT   = 24, // GPIO_B1_08
    BELL_SWITCHES_RIGHT  = 25, // GPIO_B1_09
    BELL_SWITCHES_LATCH  = 26, // GPIO_B1_10
    BELL_SWITCHES_DOME   = 27, // GPIO_B1_11

    BELL_WAKEUP_GPIO = static_cast<int>(drivers::GPIOInstances ::GPIO_5),
    BELL_WAKEUP      = 0, // SNVS_WAKEUP_GPIO5_IO00

    BELL_BATTERY_CHARGER_GPIO       = static_cast<int>(drivers::GPIOInstances ::GPIO_2),
    BELL_BATTERY_CHARGER_CHGEN_PIN  = 23,
    BELL_BATTERY_CHARGER_ACOK_PIN   = 22,
    BELL_BATTERY_CHARGER_CHGOK_GPIO = static_cast<int>(drivers::GPIOInstances ::GPIO_1),
    BELL_BATTERY_CHARGER_CHGOK_PIN  = 0,

    BELL_FUELGAUGE_I2C_BAUDRATE = I2C_STD_BAUDRATE,
    BELL_FUELGAUGE_I2C          = static_cast<int>(drivers::I2CInstances ::I2C4),
    BELL_FUELGAUGE_ALRT_GPIO    = static_cast<int>(drivers::GPIOInstances ::GPIO_1),
    BELL_FUELGAUGE_ALRT_PIN     = 3,

    BELL_AUDIOCODEC_GPIO = static_cast<int>(drivers::GPIOInstances ::GPIO_1),
    BELL_AUDIOCODEC_RSTN_PA_PIN = 14,
    BELL_AUDIOCODEC_INTN_PA_PIN = 24,
};
