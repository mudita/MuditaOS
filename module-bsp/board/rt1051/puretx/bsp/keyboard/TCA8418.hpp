// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

/**
 * @file TCA8418.h
 * @brief Basic IO driver for TCA8418 keyboard controller
 */

#ifndef INCLUDE_TCA8418_H_
#define INCLUDE_TCA8418_H_

#include <stdint.h>
#include "FreeRTOS.h"
#include "fsl_common.h"

/* TCA8418 I2C bus address */
#define TCA8418_I2C_ADDRESS      (0x34)
#define TCA8418_I2C_ADDRESS_SIZE 1
#define TCA8418_ROWS_COUNT       7
#define TCA8418_COL_COUNT        4

/* TCA8418 hardware limits */
#define TCA8418_MAX_ROWS 8
#define TCA8418_MAX_COLS 10

#define BIT(x) (1 << (x))

/* TCA8418 bit definitions */
#define CFG_AI           BIT(7)
#define CFG_GPI_E_CFG    BIT(6)
#define CFG_OVR_FLOW_M   BIT(5)
#define CFG_INT_CFG      BIT(4)
#define CFG_OVR_FLOW_IEN BIT(3)
#define CFG_K_LCK_IEN    BIT(2)
#define CFG_GPI_IEN      BIT(1)
#define CFG_KE_IEN       BIT(0)

#define INT_STAT_CAD_INT      BIT(4)
#define INT_STAT_OVR_FLOW_INT BIT(3)
#define INT_STAT_K_LCK_INT    BIT(2)
#define INT_STAT_GPI_INT      BIT(1)
#define INT_STAT_K_INT        BIT(0)

#define KEY_LCK_EC BIT(6)

/* TCA8418 register masks */
#define KEY_LCK_EC_KEC  0x7
#define KEY_EVENT_CODE  0x7f
#define KEY_EVENT_VALUE 0x80

/* TCA8418 register definitions */
#define REG_CFG            0x01
#define REG_INT_STAT       0x02
#define REG_KEY_LCK_EC     0x03
#define REG_KEY_EVENT_A    0x04
#define REG_KEY_EVENT_B    0x05
#define REG_KEY_EVENT_C    0x06
#define REG_KEY_EVENT_D    0x07
#define REG_KEY_EVENT_E    0x08
#define REG_KEY_EVENT_F    0x09
#define REG_KEY_EVENT_G    0x0A
#define REG_KEY_EVENT_H    0x0B
#define REG_KEY_EVENT_I    0x0C
#define REG_KEY_EVENT_J    0x0D
#define REG_KP_LCK_TIMER   0x0E
#define REG_UNLOCK1        0x0F
#define REG_UNLOCK2        0x10
#define REG_GPIO_INT_STAT1 0x11
#define REG_GPIO_INT_STAT2 0x12
#define REG_GPIO_INT_STAT3 0x13
#define REG_GPIO_DAT_STAT1 0x14
#define REG_GPIO_DAT_STAT2 0x15
#define REG_GPIO_DAT_STAT3 0x16
#define REG_GPIO_DAT_OUT1  0x17
#define REG_GPIO_DAT_OUT2  0x18
#define REG_GPIO_DAT_OUT3  0x19
#define REG_GPIO_INT_EN1   0x1A
#define REG_GPIO_INT_EN2   0x1B
#define REG_GPIO_INT_EN3   0x1C
#define REG_KP_GPIO1       0x1D
#define REG_KP_GPIO2       0x1E
#define REG_KP_GPIO3       0x1F
#define REG_GPI_EM1        0x20
#define REG_GPI_EM2        0x21
#define REG_GPI_EM3        0x22
#define REG_GPIO_DIR1      0x23
#define REG_GPIO_DIR2      0x24
#define REG_GPIO_DIR3      0x25
#define REG_GPIO_INT_LVL1  0x26
#define REG_GPIO_INT_LVL2  0x27
#define REG_GPIO_INT_LVL3  0x28
#define REG_DEBOUNCE_DIS1  0x29
#define REG_DEBOUNCE_DIS2  0x2A
#define REG_DEBOUNCE_DIS3  0x2B
#define REG_GPIO_PULL1     0x2C
#define REG_GPIO_PULL2     0x2D
#define REG_GPIO_PULL3     0x2E

#endif /*INCLUDE_TCA8418_H_*/
