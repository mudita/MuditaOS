/*
 * battery_charger.cpp
 *
 *  Created on: Jun 28, 2019
 *      Author: kuba
 */
#include "../common/i2c.h"
#include "fsl_gpio.h"
#include "board.h"
//#include "peripherals.h"

#define BSP_BATTERY_CHARGER_I2C_ADDR                        (0xD2 >> 1)
#define BSP_FUEL_GAUGE_I2C_ADDR                             (0x6C >> 1)
#define BSP_TOP_CONTROLLER_I2C_ADDR                         (0xCC >> 1)

static void s_BSP_BatteryChargerIrqPinsInit();

int BSP_BatteryChargerInit()
{
    s_BSP_BatteryChargerIrqPinsInit();

    return 0;
}

int BSP_FuelGaugeWrite(uint16_t registerAddress, uint16_t value)
{
    bsp_i2c_inst_t* i2c = (bsp_i2c_inst_t*)BOARD_GetI2CInstance();
    return (int)bsp_i2c_Send(i2c, BSP_FUEL_GAUGE_I2C_ADDR, registerAddress, (uint8_t*)&value, sizeof(uint16_t));
}

int BSP_FuelGaugeRead(uint16_t registerAddress, uint16_t* value)
{
    if (value == NULL)
    {
        return -1;
    }

    bsp_i2c_inst_t* i2c = (bsp_i2c_inst_t*)BOARD_GetI2CInstance();
    return (int)bsp_i2c_Receive(i2c, BSP_FUEL_GAUGE_I2C_ADDR, registerAddress, (uint8_t*)value, sizeof(uint16_t));
}

int BSP_BatteryChargerWrite(uint16_t registerAddress, uint16_t value)
{
    bsp_i2c_inst_t* i2c = (bsp_i2c_inst_t*)BOARD_GetI2CInstance();

    return (int)bsp_i2c_Send(i2c, BSP_BATTERY_CHARGER_I2C_ADDR, registerAddress, (uint8_t*)&value, sizeof(uint16_t));
}

int BSP_BatteryChargerTopControllerRead(uint16_t registerAddress, uint16_t* value)
{
    if (value == NULL)
    {
        return -1;
    }

    bsp_i2c_inst_t* i2c = (bsp_i2c_inst_t*)BOARD_GetI2CInstance();
    return (int)bsp_i2c_Receive(i2c, BSP_TOP_CONTROLLER_I2C_ADDR, registerAddress, (uint8_t*)value, sizeof(uint16_t));
}

int BSP_BatteryChargerTopControllerWrite(uint16_t registerAddress, uint16_t value)
{
    bsp_i2c_inst_t* i2c = (bsp_i2c_inst_t*)BOARD_GetI2CInstance();

    return (int)bsp_i2c_Send(i2c, BSP_TOP_CONTROLLER_I2C_ADDR, registerAddress, (uint8_t*)&value, sizeof(uint16_t));
}

int BSP_BatteryChargerRead(uint16_t registerAddress, uint16_t* value)
{
    if (value == NULL)
    {
        return -1;
    }

    bsp_i2c_inst_t* i2c = (bsp_i2c_inst_t*)BOARD_GetI2CInstance();
    return (int)bsp_i2c_Receive(i2c, BSP_BATTERY_CHARGER_I2C_ADDR, registerAddress, (uint8_t*)value, sizeof(uint16_t));
}

BaseType_t BSP_BatteryChargerINOKB_IRQHandler()
{
	return 0;
}

BaseType_t BSP_BatteryChargerWCINOKB_IRQHandler()
{
	return 0;
}

BaseType_t BSP_BatteryChargerINTB_IRQHandler()
{
	return 0;
}

/*
 * *****************************************************************************************************************************************************
 * *                                                                                                                                                   *
 * *                                                         STATIC FUNCTIONS                                                                          *
 * *                                                                                                                                                   *
 * *****************************************************************************************************************************************************
 */

	static void s_BSP_BatteryChargerIrqPinsInit()
	{
		gpio_pin_config_t pinConfig;

		pinConfig.direction         = kGPIO_DigitalInput;
		pinConfig.interruptMode     = kGPIO_IntRisingOrFallingEdge;
		pinConfig.outputLogic       = 0;

		GPIO_PinInit(BOARD_BATTERY_CHARGER_INOKB_GPIO,      BOARD_BATTERY_CHARGER_INOKB_PIN,    &pinConfig);
		GPIO_PinInit(BOARD_BATTERY_CHARGER_WCINOKB_GPIO,    BOARD_BATTERY_CHARGER_WCINOKB_PIN,  &pinConfig);
		GPIO_PinInit(BOARD_BATTERY_CHARGER_INTB_GPIO,       BOARD_BATTERY_CHARGER_INTB_PIN,     &pinConfig);

		GPIO_PortEnableInterrupts(BOARD_BATTERY_CHARGER_INOKB_GPIO, 1U << BOARD_BATTERY_CHARGER_INOKB_PIN);
		GPIO_PortEnableInterrupts(BOARD_BATTERY_CHARGER_WCINOKB_GPIO, 1U << BOARD_BATTERY_CHARGER_WCINOKB_PIN);
		GPIO_PortEnableInterrupts(BOARD_BATTERY_CHARGER_INTB_GPIO, 1U << BOARD_BATTERY_CHARGER_INTB_PIN);

	}




