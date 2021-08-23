/*
 * The Clear BSD License
 * Copyright 2017 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 *  that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _BOARD_H_
#define _BOARD_H_

#include "clock_config.h"
#include "fsl_clock.h"
#include "fsl_common.h"
#include "fsl_gpio.h"
#include "pin_names.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define BOARD_SD_POWER_RESET_GPIO     (GPIO1)
#define BOARD_SD_POWER_RESET_GPIO_PIN (5U)

#define BOARD_USDHC_CARD_INSERT_CD_LEVEL (0U)

#define BOARD_USDHC_MMCCARD_POWER_CONTROL(state)

#define BOARD_USDHC_MMCCARD_POWER_CONTROL_INIT()                                                                       \
    {                                                                                                                  \
        gpio_pin_config_t sw_config = {                                                                                \
            kGPIO_DigitalOutput,                                                                                       \
            0,                                                                                                         \
            kGPIO_NoIntmode,                                                                                           \
        };                                                                                                             \
        GPIO_PinInit(BOARD_SD_POWER_RESET_GPIO, BOARD_SD_POWER_RESET_GPIO_PIN, &sw_config);                            \
        GPIO_PinWrite(BOARD_SD_POWER_RESET_GPIO, BOARD_SD_POWER_RESET_GPIO_PIN, true);                                 \
    }

#define BOARD_USDHC_SDCARD_POWER_CONTROL_INIT()                                                                        \
    {                                                                                                                  \
        gpio_pin_config_t sw_config = {                                                                                \
            kGPIO_DigitalOutput,                                                                                       \
            0,                                                                                                         \
            kGPIO_NoIntmode,                                                                                           \
        };                                                                                                             \
        GPIO_PinInit(BOARD_SD_POWER_RESET_GPIO, BOARD_SD_POWER_RESET_GPIO_PIN, &sw_config);                            \
    }

#define BOARD_USDHC_SDCARD_POWER_CONTROL(state)                                                                        \
    (GPIO_PinWrite(BOARD_SD_POWER_RESET_GPIO, BOARD_SD_POWER_RESET_GPIO_PIN, state))

#define BOARD_MMC_PIN_CONFIG(speed, strength)                                                                          \
    {                                                                                                                  \
        IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_05_USDHC2_CMD,                                                           \
                            IOMUXC_SW_PAD_CTL_PAD_SPEED(speed) | IOMUXC_SW_PAD_CTL_PAD_SRE_MASK |                      \
                                IOMUXC_SW_PAD_CTL_PAD_PKE_MASK | IOMUXC_SW_PAD_CTL_PAD_PUE_MASK |                      \
                                IOMUXC_SW_PAD_CTL_PAD_HYS_MASK | IOMUXC_SW_PAD_CTL_PAD_PUS(1) |                        \
                                IOMUXC_SW_PAD_CTL_PAD_DSE(strength));                                                  \
        IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_04_USDHC2_CLK,                                                           \
                            IOMUXC_SW_PAD_CTL_PAD_SPEED(speed) | IOMUXC_SW_PAD_CTL_PAD_SRE_MASK |                      \
                                IOMUXC_SW_PAD_CTL_PAD_HYS_MASK | IOMUXC_SW_PAD_CTL_PAD_PUS(0) |                        \
                                IOMUXC_SW_PAD_CTL_PAD_DSE(strength));                                                  \
        IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_03_USDHC2_DATA0,                                                         \
                            IOMUXC_SW_PAD_CTL_PAD_SPEED(speed) | IOMUXC_SW_PAD_CTL_PAD_SRE_MASK |                      \
                                IOMUXC_SW_PAD_CTL_PAD_PKE_MASK | IOMUXC_SW_PAD_CTL_PAD_PUE_MASK |                      \
                                IOMUXC_SW_PAD_CTL_PAD_HYS_MASK | IOMUXC_SW_PAD_CTL_PAD_PUS(1) |                        \
                                IOMUXC_SW_PAD_CTL_PAD_DSE(strength));                                                  \
        IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_02_USDHC2_DATA1,                                                         \
                            IOMUXC_SW_PAD_CTL_PAD_SPEED(speed) | IOMUXC_SW_PAD_CTL_PAD_SRE_MASK |                      \
                                IOMUXC_SW_PAD_CTL_PAD_PKE_MASK | IOMUXC_SW_PAD_CTL_PAD_PUE_MASK |                      \
                                IOMUXC_SW_PAD_CTL_PAD_HYS_MASK | IOMUXC_SW_PAD_CTL_PAD_PUS(1) |                        \
                                IOMUXC_SW_PAD_CTL_PAD_DSE(strength));                                                  \
        IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_01_USDHC2_DATA2,                                                         \
                            IOMUXC_SW_PAD_CTL_PAD_SPEED(speed) | IOMUXC_SW_PAD_CTL_PAD_SRE_MASK |                      \
                                IOMUXC_SW_PAD_CTL_PAD_PKE_MASK | IOMUXC_SW_PAD_CTL_PAD_PUE_MASK |                      \
                                IOMUXC_SW_PAD_CTL_PAD_HYS_MASK | IOMUXC_SW_PAD_CTL_PAD_PUS(1) |                        \
                                IOMUXC_SW_PAD_CTL_PAD_DSE(strength));                                                  \
        IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_00_USDHC2_DATA3,                                                         \
                            IOMUXC_SW_PAD_CTL_PAD_SPEED(speed) | IOMUXC_SW_PAD_CTL_PAD_SRE_MASK |                      \
                                IOMUXC_SW_PAD_CTL_PAD_PKE_MASK | IOMUXC_SW_PAD_CTL_PAD_PUE_MASK |                      \
                                IOMUXC_SW_PAD_CTL_PAD_HYS_MASK | IOMUXC_SW_PAD_CTL_PAD_PUS(1) |                        \
                                IOMUXC_SW_PAD_CTL_PAD_DSE(strength));                                                  \
        IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_08_USDHC2_DATA4,                                                         \
                            IOMUXC_SW_PAD_CTL_PAD_SPEED(speed) | IOMUXC_SW_PAD_CTL_PAD_SRE_MASK |                      \
                                IOMUXC_SW_PAD_CTL_PAD_PKE_MASK | IOMUXC_SW_PAD_CTL_PAD_PUE_MASK |                      \
                                IOMUXC_SW_PAD_CTL_PAD_HYS_MASK | IOMUXC_SW_PAD_CTL_PAD_PUS(1) |                        \
                                IOMUXC_SW_PAD_CTL_PAD_DSE(strength));                                                  \
        IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_09_USDHC2_DATA5,                                                         \
                            IOMUXC_SW_PAD_CTL_PAD_SPEED(speed) | IOMUXC_SW_PAD_CTL_PAD_SRE_MASK |                      \
                                IOMUXC_SW_PAD_CTL_PAD_PKE_MASK | IOMUXC_SW_PAD_CTL_PAD_PUE_MASK |                      \
                                IOMUXC_SW_PAD_CTL_PAD_HYS_MASK | IOMUXC_SW_PAD_CTL_PAD_PUS(1) |                        \
                                IOMUXC_SW_PAD_CTL_PAD_DSE(strength));                                                  \
        IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_10_USDHC2_DATA6,                                                         \
                            IOMUXC_SW_PAD_CTL_PAD_SPEED(speed) | IOMUXC_SW_PAD_CTL_PAD_SRE_MASK |                      \
                                IOMUXC_SW_PAD_CTL_PAD_PKE_MASK | IOMUXC_SW_PAD_CTL_PAD_PUE_MASK |                      \
                                IOMUXC_SW_PAD_CTL_PAD_HYS_MASK | IOMUXC_SW_PAD_CTL_PAD_PUS(1) |                        \
                                IOMUXC_SW_PAD_CTL_PAD_DSE(strength));                                                  \
        IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_11_USDHC2_DATA7,                                                         \
                            IOMUXC_SW_PAD_CTL_PAD_SPEED(speed) | IOMUXC_SW_PAD_CTL_PAD_SRE_MASK |                      \
                                IOMUXC_SW_PAD_CTL_PAD_PKE_MASK | IOMUXC_SW_PAD_CTL_PAD_PUE_MASK |                      \
                                IOMUXC_SW_PAD_CTL_PAD_HYS_MASK | IOMUXC_SW_PAD_CTL_PAD_PUS(1) |                        \
                                IOMUXC_SW_PAD_CTL_PAD_DSE(strength));                                                  \
    }

/* USB PHY condfiguration */
#define BOARD_USB_PHY_D_CAL     (0x0CU)
#define BOARD_USB_PHY_TXCAL45DP (0x06U)
#define BOARD_USB_PHY_TXCAL45DM (0x06U)

/**
 * BOARD EINK DEFINITIONS
 */

#define BOARD_EINK_LPSPI_BASE       LPSPI1
#define BOARD_EINK_CS_GPIO_PIN      (13U)
#define BOARD_EINK_CS_GPIO          GPIO3
#define BOARD_EINK_BUSY_GPIO_PIN    (17U)
#define BOARD_EINK_BUSY_GPIO        GPIO3
#define BOARD_EINK_RESET_GPIO_PIN   (16U)
#define BOARD_EINK_RESET_GPIO       GPIO3
#define BOARD_EINK_BUSY_IRQ         GPIO3_Combined_16_31_IRQn
#define BOARD_EINK_BUSY_IRQ_HANDLER GPIO3_Combined_16_31_IRQHandler

#include <eink-config.h>

#define CELLULAR_UART_BASE   LPUART1        ///< This macro indicate port to which GSM module is connected
#define CELLULAR_UART_CLKSRC kCLOCK_PllUsb1 ///< This macro indicate LPUART clock source

#define BSP_CELLULAR_UART_TX_PORT GPIO1
#define BSP_CELLULAR_UART_TX_PIN  12
#define BSP_CELLULAR_UART_TX_PAD  GPIO_AD_B0_12

#define BSP_CELLULAR_UART_RX_PORT GPIO1
#define BSP_CELLULAR_UART_RX_PIN  13
#define BSP_CELLULAR_UART_RX_PAD  GPIO_AD_B0_13

#define BSP_CELLULAR_UART_CTS_PORT GPIO1
#define BSP_CELLULAR_UART_CTS_PIN  14
#define BSP_CELLULAR_UART_CTS_PAD  GPIO_AD_B0_14

#define BSP_CELLULAR_UART_RTS_PORT GPIO1
#define BSP_CELLULAR_UART_RTS_PIN  15
#define BSP_CELLULAR_UART_RTS_PAD  GPIO_AD_B0_15

#define BSP_CELLULAR_UART_DTR_PORT GPIO3
#define BSP_CELLULAR_UART_DTR_PIN  27
#define BSP_CELLULAR_UART_DTR_PAD  GPIO_EMC_41

#define BSP_CELLULAR_POWER_PORT GPIO2
#define BSP_CELLULAR_POWER_PIN  18
#define BSP_CELLULAR_POWER_PAD  GPIO_B1_02

#define BSP_CELLULAR_RESET_PORT GPIO2
#define BSP_CELLULAR_RESET_PIN  17
#define BSP_CELLULAR_RESET_PAD  GPIO_B1_01

#define BSP_CELLULAR_RI_PORT GPIO2
#define BSP_CELLULAR_RI_PIN  21
#define BSP_CELLULAR_RI_PAD  GPIO_B1_05

#define BSP_CELLULAR_AP_RDY_PORT GPIO2
#define BSP_CELLULAR_AP_RDY_PIN  16
#define BSP_CELLULAR_AP_RDY_PAD  GPIO_B1_00

#define BSP_CELLULAR_WAKEUP_PORT GPIO2
#define BSP_CELLULAR_WAKEUP_PIN  22
#define BSP_CELLULAR_WAKEUP_PAD  GPIO_B1_06

#define BSP_CELLULAR_SIM_TRAY_INSERTED_PORT  GPIO2
#define BSP_CELLULAR_SIM_TRAY_INSERTED_PIN   11
#define BSP_CELLULAR_SIM_CARD_1_INSERTED_PAD GPIO_B0_11

#define BSP_CELLULAR_SIM_CARD_PRESENCE_PORT GPIO2
#define BSP_CELLULAR_SIM_CARD_PRESENCE_PIN  9
#define BSP_CELLULAR_SIM_CARD_PRESENCE_PAD  GPIO_B0_09

#define BSP_CELLULAR_SIM_CARD_SELECTION_PORT GPIO2
#define BSP_CELLULAR_SIM_CARD_SELECTION_PIN  20
#define BSP_CELLULAR_SIM_CARD_SELECTION_PAD  GPIO_B1_04

#define BSP_CELLULAR_STATUS_PORT GPIO1
#define BSP_CELLULAR_STATUS_PIN  2
#define BSP_CELLULAR_STATUS_PAD  GPIO_AD_B0_02

/* SAI */
#define BOARD_CELLULAR_AUDIO_SAIx           SAI1
#define BOARD_CELLULAR_AUDIO_SAIx_MUX       kCLOCK_Sai1Mux
#define BOARD_CELLULAR_AUDIO_SAIx_PREDIV    kCLOCK_Sai1PreDiv
#define BOARD_CELLULAR_AUDIO_SAIx_DIV       kCLOCK_Sai1Div
#define BOARD_CELLULAR_AUDIO_SAIx_MCLK_MASK IOMUXC_GPR_GPR1_SAI1_MCLK_DIR_MASK

/* IRQ */
#define BOARD_CELLULAR_AUDIO_SAIx_TX_IRQ SAI1_IRQn
#define BOARD_CELLULAR_AUDIO_SAIx_RX_IRQ SAI1_IRQn

#define BSP_CELLULAR_SIMSEL_PORT GPIO2
#define BSP_CELLULAR_SIMSEL_PIN  20 // GSM_SIM_SEL
#define BSP_CELLULAR_SIMSEL_PAD  GPIO_B1_04

/**
 * Definitions for BSP_BLUETOOTH pins configuration
 */

#define BSP_BLUETOOTH_UART_BASE   LPUART2        ///< This macro indicate port to which BT module is connected
#define BSP_BLUETOOTH_UART_CLKSRC kCLOCK_PllUsb1 ///< This macro indicate LPUART clock source

#define BSP_BLUETOOTH_UART_RTS_PORT GPIO1
#define BSP_BLUETOOTH_UART_RTS_PIN  17
#define BSP_BLUETOOTH_UART_RTS_PAD  GPIO_AD_B1_01

#define BSP_BLUETOOTH_UART_CTS_PORT GPIO1
#define BSP_BLUETOOTH_UART_CTS_PIN  16
#define BSP_BLUETOOTH_UART_CTS_PAD  GPIO_AD_B1_00

#define BSP_BLUETOOTH_SHUTDOWN_PORT GPIO2
#define BSP_BLUETOOTH_SHUTDOWN_PIN  1
#define BSP_BLUETOOTH_SHUTDOWN_PAD  GPIO_B0_01 // active low

/**
 * BOARD KEYBOARD DEFINITIONS
 */

#define BOARD_KEYBOARD_RESET_GPIO_PIN (29U)
#define BOARD_KEYBOARD_RESET_GPIO     GPIO2
#define BOARD_KEYBOARD_IRQ_GPIO_PIN   (28U)
#define BOARD_KEYBOARD_IRQ_GPIO       GPIO2

#define BOARD_KEYBOARD_RF_BUTTON_PORT GPIO2
#define BOARD_KEYBOARD_RF_BUTTON_PIN  6
#define BOARD_KEYBOARD_RF_BUTTON_PAD  GPIO_B0_06
/**
 * BOARD AUDIOCODEC DEFINITIONS
 */

/* SAI */
#define BOARD_AUDIOCODEC_SAIx           SAI2
#define BOARD_AUDIOCODEC_SAIx_MUX       kCLOCK_Sai2Mux
#define BOARD_AUDIOCODEC_SAIx_PREDIV    kCLOCK_Sai2PreDiv
#define BOARD_AUDIOCODEC_SAIx_DIV       kCLOCK_Sai2Div
#define BOARD_AUDIOCODEC_SAIx_MCLK_MASK IOMUXC_GPR_GPR1_SAI2_MCLK_DIR_MASK

/* IRQ */
#define BOARD_AUDIOCODEC_SAIx_TX_IRQ SAI2_IRQn
#define BOARD_AUDIOCODEC_SAIx_RX_IRQ SAI2_IRQn

/**
 * BOARD USB TYPE-C CONTROLLER DEFINITIONS
 */
#define BOARD_USBC_GPIO     GPIO2
#define BOARD_USBC_NEN_PIN  (22U)
#define BOARD_USBC_NINT_PIN (25U) // GPIO_B1_09, GPIO2.IO[25]
#define BOARD_USBC_ID_PIN   (26U)
#define BOARD_USBC_PORT_PIN (23U)

/**
 * BOARD LED DRIVER DEFINITIONS
 */
#define BOARD_LEDDRIVER_GPIO     GPIO1
#define BOARD_LEDDRIVER_NRST_PIN (3U)

/**
 * BOARD JACK DETECTION DEFINITIONS
 */
#define BOARD_JACKDET_IRQ_GPIO     GPIO2
#define BOARD_JACKDET_IRQ_GPIO_PIN (30U)

/**
 * MICROPHONE BIAS DEFINITIONS
 */
#define BOARD_MIC_LDO_EN_GPIO GPIO2
#define BOARD_MIC_LDO_EN_PIN  19

#define DMA_MAX_SINGLE_TRANSACTION_PAYLOAD 32767

/**
 * BOARD BATTERY CHARGER, FUEL GAUGE DEFINITIONS
 * @note VALID ONLY FOR T1
 */
#define BOARD_BATTERY_CHARGER_INOKB_GPIO GPIO2
#define BOARD_BATTERY_CHARGER_INOKB_PIN  12
#define BOARD_BATTERY_CHARGER_INOKB_PAD  GPIO_B0_12

#define BOARD_BATTERY_CHARGER_WCINOKB_GPIO GPIO2
#define BOARD_BATTERY_CHARGER_WCINOKB_PIN  13
#define BOARD_BATTERY_CHARGER_WCINOKB_PAD  GPIO_B0_13

#define BOARD_BATTERY_CHARGER_INTB_GPIO GPIO2
#define BOARD_BATTERY_CHARGER_INTB_PIN  14
#define BOARD_BATTERY_CHARGER_INTB_PAD  GPIO_B0_14

/**
 * BOARD AMBIENT LIGHT SENSOR
 */
#define BOARD_ALS_ADC_BASE          ADC1 // gpio_ad_b1_08 - gpio1.24 - alt5
#define BOARD_ALS_ADC_USER_CHANNEL  13U  // ADC_IN13
#define BOARD_ALS_ADC_CHANNEL_GROUP 0U
#define BOARD_ALS_GB1_GPIO          GPIO1
#define BOARD_ALS_GB1_PIN           26
#define BOARD_ALS_GB1_PAD           GPIO_AD_B1_10
#define BOARD_ALS_GB2_GPIO          GPIO1
#define BOARD_ALS_GB2_PIN           27
#define BOARD_ALS_GB2_PAD           GPIO_AD_B1_11

/**
 * BOARD VIBRATOR MOTOR DEFINITIONS
 */

#define BOARD_VIBRATOR_EN_PORT GPIO1
#define BOARD_VIBRATOR_EN_PIN  0U

#if defined(__cplusplus)
extern "C"
{
#endif /* __cplusplus */

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _BOARD_H_ */
