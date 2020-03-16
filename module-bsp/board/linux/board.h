#ifndef _BOARD_H_
#define _BOARD_H_

//#include "clock_config.h"
//#include "fsl_common.h"
//#include "fsl_gpio.h"
//#include "fsl_clock.h"
//#include "pin_names.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*! @brief The board name */
#define BOARD_NAME "IMXRT1050-EVKB"

/* The UART to use for debug messages. */
//#define BOARD_DEBUG_UART_TYPE DEBUG_CONSOLE_DEVICE_TYPE_LPUART
//#define BOARD_DEBUG_UART_BASEADDR LPUART3
//#define BOARD_DEBUG_UART_INSTANCE 1U
//
//#define BOARD_DEBUG_UART_CLK_FREQ BOARD_DebugConsoleSrcFreq()
//
//#define BOARD_UART_IRQ LPUART3_IRQn
//#define BOARD_UART_IRQ_HANDLER LPUART3_IRQHandler
//
//#ifndef BOARD_DEBUG_UART_BAUDRATE
//#define BOARD_DEBUG_UART_BAUDRATE (115200U)
//#endif /* BOARD_DEBUG_UART_BAUDRATE */
//
///*! @brief The USER_LED used for board */
//#define LOGIC_LED_ON (0U)
//#define LOGIC_LED_OFF (1U)
//#ifndef BOARD_USER_LED_GPIO
//#define BOARD_USER_LED_GPIO GPIO1
//#endif
//#ifndef BOARD_USER_LED_GPIO_PIN
//#define BOARD_USER_LED_GPIO_PIN (9U)
//#endif
//
/*!< Enable target USER_LED */
/*#define USER_LED_INIT(output)                                            \
    GPIO_PinWrite(BOARD_USER_LED_GPIO, BOARD_USER_LED_GPIO_PIN, output); \
    BOARD_USER_LED_GPIO->GDIR |= (1U << BOARD_USER_LED_GPIO_PIN)*/
/*!< Turn off target USER_LED */
/*#define USER_LED_ON() \
    GPIO_PortClear(BOARD_USER_LED_GPIO, 1U << BOARD_USER_LED_GPIO_PIN)*/

/*!<Turn on target USER_LED*/
//#define USER_LED_OFF() GPIO_PortSet(BOARD_USER_LED_GPIO, 1U << BOARD_USER_LED_GPIO_PIN)
//!< Toggle target USER_LED */
/*#define USER_LED_TOGGLE()                                       \
    GPIO_PinWrite(BOARD_USER_LED_GPIO, BOARD_USER_LED_GPIO_PIN, \
                  0x1 ^ GPIO_PinRead(BOARD_USER_LED_GPIO, BOARD_USER_LED_GPIO_PIN))*/
//
///*! @brief Define the port interrupt number for the board switches */
//#ifndef BOARD_USER_BUTTON_GPIO
//#define BOARD_USER_BUTTON_GPIO GPIO5
//#endif
//#ifndef BOARD_USER_BUTTON_GPIO_PIN
//#define BOARD_USER_BUTTON_GPIO_PIN (0U)
//#endif
//#define BOARD_USER_BUTTON_IRQ GPIO5_Combined_0_15_IRQn
//#define BOARD_USER_BUTTON_IRQ_HANDLER GPIO5_Combined_0_15_IRQHandler
//#define BOARD_USER_BUTTON_NAME "SW8"
//
///*! @brief The hyper flash size */
//#define BOARD_FLASH_SIZE    (0x4000000U)
//
///*! @brief The ENET PHY address. */
//#define BOARD_ENET0_PHY_ADDRESS (0x02U) /* Phy address of enet port 0. */
//
///* USB PHY condfiguration */
//#define BOARD_USB_PHY_D_CAL (0x0CU)
//#define BOARD_USB_PHY_TXCAL45DP (0x06U)
//#define BOARD_USB_PHY_TXCAL45DM (0x06U)
//
//#define BOARD_ARDUINO_INT_IRQ (GPIO1_INT3_IRQn)
//#define BOARD_ARDUINO_I2C_IRQ (LPI2C1_IRQn)
//#define BOARD_ARDUINO_I2C_INDEX (1)
//#define BOARD_USDHC1_BASEADDR USDHC1
//#define BOARD_USDHC2_BASEADDR USDHC2
//#define BOARD_USDHC_CD_GPIO_BASE GPIO2
//#define BOARD_USDHC_CD_GPIO_PIN 28
//#define BOARD_USDHC_CD_PORT_IRQ GPIO2_Combined_16_31_IRQn
//#define BOARD_USDHC_CD_PORT_IRQ_HANDLER GPIO2_Combined_16_31_IRQHandler
//
//#define BOARD_USDHC_CD_STATUS() (GPIO_PinRead(BOARD_USDHC_CD_GPIO_BASE, BOARD_USDHC_CD_GPIO_PIN))
//
//#define BOARD_USDHC_CD_INTERRUPT_STATUS() (GPIO_PortGetInterruptFlags(BOARD_USDHC_CD_GPIO_BASE))
//#define BOARD_USDHC_CD_CLEAR_INTERRUPT(flag) (GPIO_PortClearInterruptFlags(BOARD_USDHC_CD_GPIO_BASE, flag))
//
/*#define BOARD_USDHC_CD_GPIO_INIT()                                                          \
    {                                                                                       \
        gpio_pin_config_t sw_config = {                                                     \
            kGPIO_DigitalInput, 0, kGPIO_IntRisingOrFallingEdge,                                    \
        };                                                                                  \
        GPIO_PinInit(BOARD_USDHC_CD_GPIO_BASE, BOARD_USDHC_CD_GPIO_PIN, &sw_config);        \
        GPIO_PortEnableInterrupts(BOARD_USDHC_CD_GPIO_BASE, 1U << BOARD_USDHC_CD_GPIO_PIN); \
        GPIO_PortClearInterruptFlags(BOARD_USDHC_CD_GPIO_BASE, ~0);                         \
    }*/
//#define BOARD_HAS_SDCARD (1U)
//#define BOARD_SD_POWER_RESET_GPIO (GPIO1)
//#define BOARD_SD_POWER_RESET_GPIO_PIN (5U)
//
//#define BOARD_USDHC_CARD_INSERT_CD_LEVEL (0U)
//
//#define BOARD_USDHC_MMCCARD_POWER_CONTROL(state)
//
/*#define BOARD_USDHC_MMCCARD_POWER_CONTROL_INIT()                                            \
    {                                                                                       \
        gpio_pin_config_t sw_config = {                                                     \
            kGPIO_DigitalOutput, 0, kGPIO_NoIntmode,                                        \
        };                                                                                  \
        GPIO_PinInit(BOARD_SD_POWER_RESET_GPIO, BOARD_SD_POWER_RESET_GPIO_PIN, &sw_config); \
        GPIO_PinWrite(BOARD_SD_POWER_RESET_GPIO, BOARD_SD_POWER_RESET_GPIO_PIN, true);      \
    }
*/
/*#define BOARD_USDHC_SDCARD_POWER_CONTROL_INIT()                                             \
    {                                                                                       \
        gpio_pin_config_t sw_config = {                                                     \
            kGPIO_DigitalOutput, 0, kGPIO_NoIntmode,                                        \
        };                                                                                  \
        GPIO_PinInit(BOARD_SD_POWER_RESET_GPIO, BOARD_SD_POWER_RESET_GPIO_PIN, &sw_config); \
    }*/
//
/*#define BOARD_USDHC_SDCARD_POWER_CONTROL(state) \
    (GPIO_PinWrite(BOARD_SD_POWER_RESET_GPIO, BOARD_SD_POWER_RESET_GPIO_PIN, state))*/
//
//#define BOARD_USDHC1_CLK_FREQ (CLOCK_GetSysPfdFreq(kCLOCK_Pfd0) / (CLOCK_GetDiv(kCLOCK_Usdhc1Div) + 1U))
//#define BOARD_USDHC2_CLK_FREQ (CLOCK_GetSysPfdFreq(kCLOCK_Pfd0) / (CLOCK_GetDiv(kCLOCK_Usdhc2Div) + 1U))
//
//#define BOARD_SD_HOST_BASEADDR BOARD_USDHC1_BASEADDR
//#define BOARD_SD_HOST_CLK_FREQ BOARD_USDHC1_CLK_FREQ
//#define BOARD_SD_HOST_IRQ USDHC1_IRQn
//
//#define BOARD_MMC_HOST_BASEADDR BOARD_USDHC2_BASEADDR
//#define BOARD_MMC_HOST_CLK_FREQ BOARD_USDHC2_CLK_FREQ
//#define BOARD_MMC_HOST_IRQ USDHC2_IRQn
//#define BOARD_MMC_VCCQ_SUPPLY kMMC_VoltageWindow170to195
//#define BOARD_MMC_VCC_SUPPLY kMMC_VoltageWindows270to360
///* we are using the BB SD socket to DEMO the MMC example,but the
//* SD socket provide 4bit bus only, so we define this macro to avoid
//* 8bit data bus test
//*/
//#define BOARD_MMC_SUPPORT_8BIT_BUS (1U)
//
//#define BOARD_SD_HOST_SUPPORT_SDR104_FREQ (200000000U)
//#define BOARD_SD_HOST_SUPPORT_HS200_FREQ (180000000U)
///* define for SD/MMC config IO driver strength dynamic */
/*#define BOARD_SD_PIN_CONFIG(speed, strength)                                                      \
    {                                                                                             \
        IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B0_00_USDHC1_CMD,                                      \
                            IOMUXC_SW_PAD_CTL_PAD_SPEED(speed) | IOMUXC_SW_PAD_CTL_PAD_SRE_MASK | \
                                IOMUXC_SW_PAD_CTL_PAD_PKE_MASK | IOMUXC_SW_PAD_CTL_PAD_PUE_MASK | \
                                IOMUXC_SW_PAD_CTL_PAD_HYS_MASK | IOMUXC_SW_PAD_CTL_PAD_PUS(1) |   \
                                IOMUXC_SW_PAD_CTL_PAD_DSE(strength));                             \
        IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B0_01_USDHC1_CLK,                                      \
                            IOMUXC_SW_PAD_CTL_PAD_SPEED(speed) | IOMUXC_SW_PAD_CTL_PAD_SRE_MASK | \
                                IOMUXC_SW_PAD_CTL_PAD_HYS_MASK | IOMUXC_SW_PAD_CTL_PAD_PUS(0) |   \
                                IOMUXC_SW_PAD_CTL_PAD_DSE(strength));                             \
        IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B0_02_USDHC1_DATA0,                                    \
                            IOMUXC_SW_PAD_CTL_PAD_SPEED(speed) | IOMUXC_SW_PAD_CTL_PAD_SRE_MASK | \
                                IOMUXC_SW_PAD_CTL_PAD_PKE_MASK | IOMUXC_SW_PAD_CTL_PAD_PUE_MASK | \
                                IOMUXC_SW_PAD_CTL_PAD_HYS_MASK | IOMUXC_SW_PAD_CTL_PAD_PUS(1) |   \
                                IOMUXC_SW_PAD_CTL_PAD_DSE(strength));                             \
        IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B0_03_USDHC1_DATA1,                                    \
                            IOMUXC_SW_PAD_CTL_PAD_SPEED(speed) | IOMUXC_SW_PAD_CTL_PAD_SRE_MASK | \
                                IOMUXC_SW_PAD_CTL_PAD_PKE_MASK | IOMUXC_SW_PAD_CTL_PAD_PUE_MASK | \
                                IOMUXC_SW_PAD_CTL_PAD_HYS_MASK | IOMUXC_SW_PAD_CTL_PAD_PUS(1) |   \
                                IOMUXC_SW_PAD_CTL_PAD_DSE(strength));                             \
        IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B0_04_USDHC1_DATA2,                                    \
                            IOMUXC_SW_PAD_CTL_PAD_SPEED(speed) | IOMUXC_SW_PAD_CTL_PAD_SRE_MASK | \
                                IOMUXC_SW_PAD_CTL_PAD_PKE_MASK | IOMUXC_SW_PAD_CTL_PAD_PUE_MASK | \
                                IOMUXC_SW_PAD_CTL_PAD_HYS_MASK | IOMUXC_SW_PAD_CTL_PAD_PUS(1) |   \
                                IOMUXC_SW_PAD_CTL_PAD_DSE(strength));                             \
        IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B0_05_USDHC1_DATA3,                                    \
                            IOMUXC_SW_PAD_CTL_PAD_SPEED(speed) | IOMUXC_SW_PAD_CTL_PAD_SRE_MASK | \
                                IOMUXC_SW_PAD_CTL_PAD_PKE_MASK | IOMUXC_SW_PAD_CTL_PAD_PUE_MASK | \
                                IOMUXC_SW_PAD_CTL_PAD_HYS_MASK | IOMUXC_SW_PAD_CTL_PAD_PUS(1) |   \
                                IOMUXC_SW_PAD_CTL_PAD_DSE(strength));                             \
    }*/
//
/*#define BOARD_MMC_PIN_CONFIG(speed, strength)                                                     \
    {                                                                                             \
        IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_05_USDHC2_CMD,                                      \
                            IOMUXC_SW_PAD_CTL_PAD_SPEED(speed) | IOMUXC_SW_PAD_CTL_PAD_SRE_MASK | \
                                IOMUXC_SW_PAD_CTL_PAD_PKE_MASK | IOMUXC_SW_PAD_CTL_PAD_PUE_MASK | \
                                IOMUXC_SW_PAD_CTL_PAD_HYS_MASK | IOMUXC_SW_PAD_CTL_PAD_PUS(1) |   \
                                IOMUXC_SW_PAD_CTL_PAD_DSE(strength));                             \
        IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_04_USDHC2_CLK,                                      \
                            IOMUXC_SW_PAD_CTL_PAD_SPEED(speed) | IOMUXC_SW_PAD_CTL_PAD_SRE_MASK | \
                                IOMUXC_SW_PAD_CTL_PAD_HYS_MASK | IOMUXC_SW_PAD_CTL_PAD_PUS(0) |   \
                                IOMUXC_SW_PAD_CTL_PAD_DSE(strength));                             \
        IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_03_USDHC2_DATA0,                                    \
                            IOMUXC_SW_PAD_CTL_PAD_SPEED(speed) | IOMUXC_SW_PAD_CTL_PAD_SRE_MASK | \
                                IOMUXC_SW_PAD_CTL_PAD_PKE_MASK | IOMUXC_SW_PAD_CTL_PAD_PUE_MASK | \
                                IOMUXC_SW_PAD_CTL_PAD_HYS_MASK | IOMUXC_SW_PAD_CTL_PAD_PUS(1) |   \
                                IOMUXC_SW_PAD_CTL_PAD_DSE(strength));                             \
        IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_02_USDHC2_DATA1,                                    \
                            IOMUXC_SW_PAD_CTL_PAD_SPEED(speed) | IOMUXC_SW_PAD_CTL_PAD_SRE_MASK | \
                                IOMUXC_SW_PAD_CTL_PAD_PKE_MASK | IOMUXC_SW_PAD_CTL_PAD_PUE_MASK | \
                                IOMUXC_SW_PAD_CTL_PAD_HYS_MASK | IOMUXC_SW_PAD_CTL_PAD_PUS(1) |   \
                                IOMUXC_SW_PAD_CTL_PAD_DSE(strength));                             \
        IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_01_USDHC2_DATA2,                                    \
                            IOMUXC_SW_PAD_CTL_PAD_SPEED(speed) | IOMUXC_SW_PAD_CTL_PAD_SRE_MASK | \
                                IOMUXC_SW_PAD_CTL_PAD_PKE_MASK | IOMUXC_SW_PAD_CTL_PAD_PUE_MASK | \
                                IOMUXC_SW_PAD_CTL_PAD_HYS_MASK | IOMUXC_SW_PAD_CTL_PAD_PUS(1) |   \
                                IOMUXC_SW_PAD_CTL_PAD_DSE(strength));                             \
        IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_00_USDHC2_DATA3,                                    \
                            IOMUXC_SW_PAD_CTL_PAD_SPEED(speed) | IOMUXC_SW_PAD_CTL_PAD_SRE_MASK | \
                                IOMUXC_SW_PAD_CTL_PAD_PKE_MASK | IOMUXC_SW_PAD_CTL_PAD_PUE_MASK | \
                                IOMUXC_SW_PAD_CTL_PAD_HYS_MASK | IOMUXC_SW_PAD_CTL_PAD_PUS(1) |   \
                                IOMUXC_SW_PAD_CTL_PAD_DSE(strength));                             \
        IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_08_USDHC2_DATA4,                                    \
                            IOMUXC_SW_PAD_CTL_PAD_SPEED(speed) | IOMUXC_SW_PAD_CTL_PAD_SRE_MASK | \
                                IOMUXC_SW_PAD_CTL_PAD_PKE_MASK | IOMUXC_SW_PAD_CTL_PAD_PUE_MASK | \
                                IOMUXC_SW_PAD_CTL_PAD_HYS_MASK | IOMUXC_SW_PAD_CTL_PAD_PUS(1) |   \
                                IOMUXC_SW_PAD_CTL_PAD_DSE(strength));                             \
        IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_09_USDHC2_DATA5,                                    \
                            IOMUXC_SW_PAD_CTL_PAD_SPEED(speed) | IOMUXC_SW_PAD_CTL_PAD_SRE_MASK | \
                                IOMUXC_SW_PAD_CTL_PAD_PKE_MASK | IOMUXC_SW_PAD_CTL_PAD_PUE_MASK | \
                                IOMUXC_SW_PAD_CTL_PAD_HYS_MASK | IOMUXC_SW_PAD_CTL_PAD_PUS(1) |   \
                                IOMUXC_SW_PAD_CTL_PAD_DSE(strength));                             \
        IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_10_USDHC2_DATA6,                                    \
                            IOMUXC_SW_PAD_CTL_PAD_SPEED(speed) | IOMUXC_SW_PAD_CTL_PAD_SRE_MASK | \
                                IOMUXC_SW_PAD_CTL_PAD_PKE_MASK | IOMUXC_SW_PAD_CTL_PAD_PUE_MASK | \
                                IOMUXC_SW_PAD_CTL_PAD_HYS_MASK | IOMUXC_SW_PAD_CTL_PAD_PUS(1) |   \
                                IOMUXC_SW_PAD_CTL_PAD_DSE(strength));                             \
        IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_11_USDHC2_DATA7,                                    \
                            IOMUXC_SW_PAD_CTL_PAD_SPEED(speed) | IOMUXC_SW_PAD_CTL_PAD_SRE_MASK | \
                                IOMUXC_SW_PAD_CTL_PAD_PKE_MASK | IOMUXC_SW_PAD_CTL_PAD_PUE_MASK | \
                                IOMUXC_SW_PAD_CTL_PAD_HYS_MASK | IOMUXC_SW_PAD_CTL_PAD_PUS(1) |   \
                                IOMUXC_SW_PAD_CTL_PAD_DSE(strength));                             \
    }*/
//
///*! @brief The WIFI-QCA shield pin. */
//#define BOARD_INITGT202SHIELD_PWRON_GPIO GPIO1                    /*!< GPIO device name: GPIO */
//#define BOARD_INITGT202SHIELD_PWRON_PORT 1U                       /*!< PORT device index: 1 */
//#define BOARD_INITGT202SHIELD_PWRON_GPIO_PIN 3U                   /*!< PIO4 pin index: 3 */
//#define BOARD_INITGT202SHIELD_PWRON_PIN_NAME GPIO1_3              /*!< Pin name */
//#define BOARD_INITGT202SHIELD_PWRON_LABEL "PWRON"                 /*!< Label */
//#define BOARD_INITGT202SHIELD_PWRON_NAME "PWRON"                  /*!< Identifier name */
//#define BOARD_INITGT202SHIELD_PWRON_DIRECTION kGPIO_DigitalOutput /*!< Direction */
//
//#define BOARD_INITGT202SHIELD_IRQ_GPIO GPIO1                   /*!< GPIO device name: GPIO */
//#define BOARD_INITGT202SHIELD_IRQ_PORT 1U                      /*!< PORT device index: 1 */
//#define BOARD_INITGT202SHIELD_IRQ_GPIO_PIN 19U                 /*!< PIO1 pin index: 19 */
//#define BOARD_INITGT202SHIELD_IRQ_PIN_NAME GPIO1_19            /*!< Pin name */
//#define BOARD_INITGT202SHIELD_IRQ_LABEL "IRQ"                  /*!< Label */
//#define BOARD_INITGT202SHIELD_IRQ_NAME "IRQ"                   /*!< Identifier name */
//#define BOARD_INITGT202SHIELD_IRQ_DIRECTION kGPIO_DigitalInput /*!< Direction */
//
//
///* @Brief Board accelerator sensor configuration */
//#define BOARD_ACCEL_I2C_BASEADDR LPI2C1
///* Select USB1 PLL (480 MHz) as LPI2C's clock source */
//#define BOARD_ACCEL_I2C_CLOCK_SOURCE_SELECT (0U)
///* Clock divider for LPI2C clock source */
//#define BOARD_ACCEL_I2C_CLOCK_SOURCE_DIVIDER (5U)
//#define BOARD_ACCEL_I2C_CLOCK_FREQ (CLOCK_GetFreq(kCLOCK_Usb1PllClk) / 8 / (BOARD_ACCEL_I2C_CLOCK_SOURCE_DIVIDER +
//1U))
//
//#define BOARD_CODEC_I2C_BASEADDR LPI2C1
//#define BOARD_CODEC_I2C_CLOCK_SOURCE_SELECT (0U)
//#define BOARD_CODEC_I2C_CLOCK_SOURCE_DIVIDER (5U)
//#define BOARD_CODEC_I2C_CLOCK_FREQ ((CLOCK_GetFreq(kCLOCK_Usb1PllClk) / 8) / (BOARD_CODEC_I2C_CLOCK_SOURCE_DIVIDER +
//1U))
//
///* @Brief Board CAMERA configuration */
//#define BOARD_CAMERA_I2C_BASEADDR LPI2C1
//#define BOARD_CAMERA_I2C_CLOCK_SOURCE_DIVIDER (5U)
//#define BOARD_CAMERA_I2C_CLOCK_SOURCE_SELECT (0U) /* Select USB1 PLL (480 MHz) as LPI2C's clock source */
/*#define BOARD_CAMERA_I2C_CLOCK_FREQ \
    (CLOCK_GetFreq(kCLOCK_Usb1PllClk) / 8 / (BOARD_CAMERA_I2C_CLOCK_SOURCE_DIVIDER + 1U))*/

/**
 * BOARD EINK DEFINITIONS
 */

///* Select USB1 PLL PFD0 (720 MHz) as lpspi clock source */
//#define BOARD_EINK_LPSPI_CLOCK_SOURCE_SELECT (1U)
///* Clock divider for master lpspi clock source */
//#define BOARD_EINK_LPSPI_CLOCK_SOURCE_DIVIDER (7U)
/*#define BOARD_EINK_LPSPI_CLOCK_FREQ \
    (CLOCK_GetFreq(kCLOCK_Usb1PllPfd0Clk) / (BOARD_EINK_LPSPI_CLOCK_SOURCE_DIVIDER + 1U))*/
//
//
//#define BOARD_EINK_LPSPI_BASE		LPSPI1
//#define BOARD_EINK_CS_GPIO_PIN		(13U)
//#define BOARD_EINK_CS_GPIO			GPIO3
//#define BOARD_EINK_BUSY_GPIO_PIN	(17U)
//#define BOARD_EINK_BUSY_GPIO		GPIO3
//#define BOARD_EINK_RESET_GPIO_PIN 	(16U)
//#define BOARD_EINK_RESET_GPIO		GPIO3
//#define BOARD_EINK_BUSY_IRQ			GPIO3_Combined_16_31_IRQn
//#define BOARD_EINK_BUSY_IRQ_HANDLER	GPIO3_Combined_16_31_IRQHandler

#define BOARD_EINK_DISPLAY_RES_X 480
#define BOARD_EINK_DISPLAY_RES_Y 600

//#define CELLULAR_UART_BASE                                   LPUART1                 ///< This macro indicate port to
//which GSM module is connected #define CELLULAR_UART_CLKSRC                                 kCLOCK_PllUsb1 ///< This
//macro indicate LPUART clock source
//
//#define BSP_CELLULAR_UART_TX_PORT                            GPIO1
//#define BSP_CELLULAR_UART_TX_PIN                             12
//#define BSP_CELLULAR_UART_TX_PAD                             GPIO_AD_B0_12
//
//#define BSP_CELLULAR_UART_RX_PORT                            GPIO1
//#define BSP_CELLULAR_UART_RX_PIN                             13
//#define BSP_CELLULAR_UART_RX_PAD                             GPIO_AD_B0_13
//
//#define BSP_CELLULAR_UART_CTS_PORT                           GPIO1
//#define BSP_CELLULAR_UART_CTS_PIN                            14
//#define BSP_CELLULAR_UART_CTS_PAD                            GPIO_AD_B0_14
//
//#define BSP_CELLULAR_UART_RTS_PORT                           GPIO1
//#define BSP_CELLULAR_UART_RTS_PIN                            15
//#define BSP_CELLULAR_UART_RTS_PAD                            GPIO_AD_B0_15
//
//#define BSP_CELLULAR_UART_DTR_PORT                           GPIO3
//#define BSP_CELLULAR_UART_DTR_PIN                            27
//#define BSP_CELLULAR_UART_DTR_PAD                            GPIO_EMC_41
//
//#define BSP_CELLULAR_POWER_PORT                              GPIO2
//#define BSP_CELLULAR_POWER_PIN                               18
//#define BSP_CELLULAR_POWER_PAD                               GPIO_B1_02
//
//#define BSP_CELLULAR_RESET_PORT                              GPIO2
//#define BSP_CELLULAR_RESET_PIN                               17
//#define BSP_CELLULAR_RESET_PAD                               GPIO_B1_01
//
//#define BSP_CELLULAR_RI_PORT                                 GPIO2
//#define BSP_CELLULAR_RI_PIN                                  21
//#define BSP_CELLULAR_RI_PAD                                  GPIO_B1_05
//
//#define BSP_CELLULAR_AP_RDY_PORT                             GPIO2
//#define BSP_CELLULAR_AP_RDY_PIN                              16
//#define BSP_CELLULAR_AP_RDY_PAD                              GPIO_B1_00
//
//#define BSP_CELLULAR_WAKEUP_PORT                             GPIO2
//#define BSP_CELLULAR_WAKEUP_PIN                              19
//#define BSP_CELLULAR_WAKEUP_PAD                              GPIO_B1_03
//
//#define BSP_CELLULAR_SIM_CARD_1_INSERTED_PORT                GPIO2
//#define BSP_CELLULAR_SIM_CARD_1_INSERTED_PIN                 11
//#define BSP_CELLULAR_SIM_CARD_1_INSERTED_PAD                 GPIO_B0_11
//
//#define BSP_CELLULAR_SIM_CARD_2_INSERTED_PORT                GPIO2
//#define BSP_CELLULAR_SIM_CARD_2_INSERTED_PIN                 10
//#define BSP_CELLULAR_SIM_CARD_2_INSERTED_PAD                 GPIO_B0_10
//
//#define BSP_CELLULAR_SIM_CARD_PRESENCE_PORT                  GPIO2
//#define BSP_CELLULAR_SIM_CARD_PRESENCE_PIN                   9
//#define BSP_CELLULAR_SIM_CARD_PRESENCE_PAD                   GPIO_B0_09
//
//#define BSP_CELLULAR_SIM_CARD_SELECTION_PORT                 GPIO2
//#define BSP_CELLULAR_SIM_CARD_SELECTION_PIN                  20
//#define BSP_CELLULAR_SIM_CARD_SELECTION_PAD                  GPIO_B1_04
//
///* SAI */
//#define BOARD_CELLULAR_AUDIO_SAIx	                         SAI1
//#define BOARD_CELLULAR_AUDIO_SAIx_MUX	                    kCLOCK_Sai1Mux
//#define BOARD_CELLULAR_AUDIO_SAIx_PREDIV	                kCLOCK_Sai1PreDiv
//#define BOARD_CELLULAR_AUDIO_SAIx_DIV	                    kCLOCK_Sai1Div
//#define BOARD_CELLULAR_AUDIO_SAIx_MCLK_MASK	                IOMUXC_GPR_GPR1_SAI1_MCLK_DIR_MASK
//
///* IRQ */
//#define BOARD_CELLULAR_AUDIO_SAIx_TX_IRQ                    SAI1_IRQn
//#define BOARD_CELLULAR_AUDIO_SAIx_RX_IRQ                    SAI1_IRQn
//
//
///* Select Audio/Video PLL (786.48 MHz) as BOARD_AUDIOCODEC_SAIx clock source */
//#define BOARD_CELLULAR_AUDIO_SAIx_CLOCK_SOURCE_SELECT (2U)
///* Clock pre divider for BOARD_AUDIOCODEC_SAIx clock source */
//#define BOARD_CELLULAR_AUDIO_SAIx_CLOCK_SOURCE_PRE_DIVIDER (0U)
///* Divide to get 12.288MHz MCLK */
//#define BOARD_CELLULAR_AUDIO_SAIx_CLOCK_SOURCE_DIVIDER (63U)
/*#define BOARD_CELLULAR_AUDIO_SAIx_CLK_FREQ                                                        \
    (CLOCK_GetFreq(kCLOCK_AudioPllClk) / (BOARD_CELLULAR_AUDIO_SAIx_CLOCK_SOURCE_DIVIDER + 1U) / \
     (BOARD_CELLULAR_AUDIO_SAIx_CLOCK_SOURCE_PRE_DIVIDER + 1U))*/
//
//#define BSP_CELLULAR_SIMSEL_PORT                             GPIO2
//#define BSP_CELLULAR_SIMSEL_PIN                              20
//#define BSP_CELLULAR_SIMSEL_PAD                              GPIO_B1_04
//
//
///**
// * Definitions for BSP_BLUETOOTH pins configuration
// */
//
//#define BSP_BLUETOOTH_UART_BASE                              LPUART2                     ///< This macro indicate port
//to which GSM module is connected #define BSP_BLUETOOTH_UART_CLKSRC                            kCLOCK_PllUsb1 ///< This
//macro indicate LPUART clock source
//
//#define BSP_BLUETOOTH_UART_RTS_PORT                          GPIO1
//#define BSP_BLUETOOTH_UART_RTS_PIN                           17
//#define BSP_BLUETOOTH_UART_RTS_PAD                           GPIO_AD_B1_01
//
//#define BSP_BLUETOOTH_UART_CTS_PORT                          GPIO1
//#define BSP_BLUETOOTH_UART_CTS_PIN                           16
//#define BSP_BLUETOOTH_UART_CTS_PAD                           GPIO_AD_B1_00
//
//#define BSP_BLUETOOTH_OSC_EN_PIN                             0
//#define BSP_BLUETOOTH_OSC_EN_PORT                            GPIO2
//#define BSP_BLUETOOTH_OSC_EN_PAD                             GPIO_B0_00
//
//#define BSP_BLUETOOTH_SHUTDOWN_PIN                           1
//#define BSP_BLUETOOTH_SHUTDOWN_PORT                          GPIO2
//#define BSP_BLUETOOTH_SHUTDOWN_PAD                           GPIO_B0_01
//
///**
// * BOARD KEYBOARD DEFINITIONS
// */
//#define BOARD_KEYBOARD_I2C_BASEADDR LPI2C2
//#define BOARD_KEYBOARD_I2C_CLOCK_SOURCE_SELECT (0U)
//#define BOARD_KEYBOARD_I2C_CLOCK_SOURCE_DIVIDER (5U)
////#define BOARD_KEYBOARD_I2C_CLOCK_FREQ ((CLOCK_GetFreq(kCLOCK_Usb1PllClk) / 8) /
///(BOARD_KEYBOARD_I2C_CLOCK_SOURCE_DIVIDER + 1U))
//#define BOARD_KEYBOARD_I2C_CLOCK_FREQ ((CLOCK_GetFreq(kCLOCK_OscClk) / 8) / (BOARD_KEYBOARD_I2C_CLOCK_SOURCE_DIVIDER +
//1U))
//
//#define BOARD_KEYBOARD_RESET_GPIO_PIN 	(29U)
//#define BOARD_KEYBOARD_RESET_GPIO		GPIO2
//#define BOARD_KEYBOARD_IRQ_GPIO_PIN 	(28U)
//#define BOARD_KEYBOARD_IRQ_GPIO		GPIO2
//
//#define BOARD_KEYBOARD_RF_BUTTON_PORT               GPIO2
//#define BOARD_KEYBOARD_RF_BUTTON_PIN                6
//#define BOARD_KEYBOARD_RF_BUTTON_PAD                GPIO_B0_06
///**
// * BOARD AUDIOCODEC DEFINITIONS
// */
//
///* SAI */
//#define BOARD_AUDIOCODEC_SAIx	SAI2
//#define BOARD_AUDIOCODEC_SAIx_MUX	kCLOCK_Sai2Mux
//#define BOARD_AUDIOCODEC_SAIx_PREDIV	kCLOCK_Sai2PreDiv
//#define BOARD_AUDIOCODEC_SAIx_DIV	kCLOCK_Sai2Div
//#define BOARD_AUDIOCODEC_SAIx_MCLK_MASK	IOMUXC_GPR_GPR1_SAI2_MCLK_DIR_MASK
//
///* IRQ */
//#define BOARD_AUDIOCODEC_SAIx_TX_IRQ SAI2_IRQn
//#define BOARD_AUDIOCODEC_SAIx_RX_IRQ SAI2_IRQn
//
///* Select Audio/Video PLL (786.48 MHz) as BOARD_AUDIOCODEC_SAIx clock source */
//#define BOARD_AUDIOCODEC_SAIx_CLOCK_SOURCE_SELECT (2U)
///* Clock pre divider for BOARD_AUDIOCODEC_SAIx clock source */
//#define BOARD_AUDIOCODEC_SAIx_CLOCK_SOURCE_PRE_DIVIDER (0U)
///* Divide to get 12.288MHz MCLK */
//#define BOARD_AUDIOCODEC_SAIx_CLOCK_SOURCE_DIVIDER (63U)
/*#define BOARD_AUDIOCODEC_SAIx_CLK_FREQ                                                        \
    (CLOCK_GetFreq(kCLOCK_AudioPllClk) / (BOARD_AUDIOCODEC_SAIx_CLOCK_SOURCE_DIVIDER + 1U) / \
     (BOARD_AUDIOCODEC_SAIx_CLOCK_SOURCE_PRE_DIVIDER + 1U))*/
//
///* I2C */
//#define BOARD_AUDIOCODEC_I2C_BASEADDR LPI2C2
//#define BOARD_AUDIOCODEC_I2C_CLOCK_SOURCE_SELECT (0U)
//#define BOARD_AUDIOCODEC_I2C_CLOCK_SOURCE_DIVIDER (5U)
////#define BOARD_AUDIOCODEC_I2C_CLOCK_FREQ ((CLOCK_GetFreq(kCLOCK_Usb1PllClk) / 8) /
///(BOARD_AUDIOCODEC_I2C_CLOCK_SOURCE_DIVIDER + 1U))
//#define BOARD_AUDIOCODEC_I2C_CLOCK_FREQ ((CLOCK_GetFreq(kCLOCK_OscClk) / 8) /
//(BOARD_AUDIOCODEC_I2C_CLOCK_SOURCE_DIVIDER + 1U))
//
//
///**
// * BOARD EMMC DEFINITIONS
// */
//
//
//#define BOARD_EMMC_USDHCx USDHC2
//#define BOARD_EMMC_USDHCx_CLK_FREQ (CLOCK_GetSysPfdFreq(kCLOCK_Pfd0) / (CLOCK_GetDiv(kCLOCK_Usdhc2Div) + 1U))
//#define BOARD_EMMC_USDHCx_IRQ USDHC2_IRQn
//
//
//
///**
// * BOARD USB TYPE-C CONTROLLER DEFINITIONS
// */
//#define BOARD_USBC_GPIO	GPIO2
//#define BOARD_USBC_NEN_PIN	(22U)
//#define BOARD_USBC_NINT_PIN	(25U)	//GPIO_B1_09, GPIO2.IO[25]
//#define BOARD_USBC_ID_PIN (26U)
//#define BOARD_USBC_PORT_PIN	(23U)
//
//
///**
// * BOARD LED DRIVER DEFINITIONS
// */
//#define BOARD_LEDDRIVER_GPIO	                    GPIO1
//#define BOARD_LEDDRIVER_NRST_PIN	                (3U)
//
//
///**
// * BOARD JACK DETECTION DEFINITIONS
// */
//#define BOARD_JACKDET_IRQ_GPIO	                    GPIO2
//#define BOARD_JACKDET_IRQ_GPIO_PIN	                (30U)
//
//#define DMA_MAX_SINGLE_TRANSACTION_PAYLOAD          32767
//
///**
// * BOARD BATTERY CHARGER, FUEL GAUGE DEFINITIONS
// * @note VALID ONLY FOR T1
// */
//#define BOARD_BATTERY_CHARGER_INOKB_GPIO            GPIO2
//#define BOARD_BATTERY_CHARGER_INOKB_PIN             12
//#define BOARD_BATTERY_CHARGER_INOKB_PAD             GPIO_B0_12
//
//#define BOARD_BATTERY_CHARGER_WCINOKB_GPIO          GPIO2
//#define BOARD_BATTERY_CHARGER_WCINOKB_PIN           13
//#define BOARD_BATTERY_CHARGER_WCINOKB_PAD           GPIO_B0_13
//
//#define BOARD_BATTERY_CHARGER_INTB_GPIO             GPIO2
//#define BOARD_BATTERY_CHARGER_INTB_PIN              14
//#define BOARD_BATTERY_CHARGER_INTB_PAD              GPIO_B0_14
//
///**
// * BOARD AMBIENT LIGHT SENSOR
// */
//#define BOARD_ALS_ADC_BASE 							ADC1	//gpio_ad_b1_08 - gpio1.24 - alt5
//#define BOARD_ALS_ADC_USER_CHANNEL 					13U		//ADC_IN13
//#define BOARD_ALS_ADC_CHANNEL_GROUP 				0U
//#define BOARD_ALS_GB1_GPIO							GPIO1
//#define BOARD_ALS_GB1_PIN							26
//#define BOARD_ALS_GB1_PAD							GPIO_AD_B1_10
//#define BOARD_ALS_GB2_GPIO							GPIO1
//#define BOARD_ALS_GB2_PIN							27
//#define BOARD_ALS_GB2_PAD							GPIO_AD_B1_11
//
///**
// * Power switch
// */
//#define BOARD_POWER_SW_GPIO     					GPIO2
//#define BOARD_POWER_SW_GPIO_PIN 					6U
//#define BOARD_POWER_SW_PAD                    		GPIO_B0_06
//#define BOARD_POWER_HOLD_GPIO     					GPIO2
//#define BOARD_POWER_HOLD_GPIO_PIN 					7U

#if defined(__cplusplus)
extern "C"
{
#endif /* __cplusplus */

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _BOARD_H_ */
