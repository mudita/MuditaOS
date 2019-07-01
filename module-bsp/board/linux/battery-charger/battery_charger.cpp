/*
 * battery_charger.cpp
 *
 *  Created on: Jul 1, 2019
 *      Author: kuba
 */
#include <stdint.h>
#include "FreeRTOS.h"

#include "board.h"
#include "battery-charger/battery_charger.hpp"

#define BSP_BATTERY_CHARGER_I2C_ADDR                        (0xD2 >> 1)
#define BSP_FUEL_GAUGE_I2C_ADDR                             (0x6C >> 1)
#define BSP_TOP_CONTROLLER_I2C_ADDR                         (0xCC >> 1)

static void s_BSP_BatteryChargerIrqPinsInit();


static const uint16_t chargerRegs [][2] = {
		{BSP_FUEL_GAUGE_RepCap_REG , 2000},
		{0,0}
};
int BSP_BatteryChargerInit()
{
//    s_BSP_BatteryChargerIrqPinsInit();

    return 0;
}

int BSP_FuelGaugeWrite(uint16_t registerAddress, uint16_t value)
{

//    bsp_i2c_inst_t* i2c = (bsp_i2c_inst_t*)BOARD_GetI2CInstance();
//    return (int)bsp_i2c_Send(i2c, BSP_FUEL_GAUGE_I2C_ADDR, registerAddress, (uint8_t*)&value, sizeof(uint16_t));
}

int BSP_FuelGaugeRead(uint16_t registerAddress, uint16_t* value)
{
	uint32_t i = 0;
	while(chargerRegs[i][0] != 0)
	{
		if( chargerRegs[i][0] == registerAddress)
		{
			*value = chargerRegs[i][1];
			break;
		}
		i++;
	}
	return 0;
//    if (value == NULL)
//    {
//        return -1;
//    }
//
//    bsp_i2c_inst_t* i2c = (bsp_i2c_inst_t*)BOARD_GetI2CInstance();
//    return (int)bsp_i2c_Receive(i2c, BSP_FUEL_GAUGE_I2C_ADDR, registerAddress, (uint8_t*)value, sizeof(uint16_t));
}

int BSP_BatteryChargerWrite(uint16_t registerAddress, uint16_t value)
{
//    bsp_i2c_inst_t* i2c = (bsp_i2c_inst_t*)BOARD_GetI2CInstance();
//
//    return (int)bsp_i2c_Send(i2c, BSP_BATTERY_CHARGER_I2C_ADDR, registerAddress, (uint8_t*)&value, sizeof(uint16_t));
}

int BSP_BatteryChargerTopControllerRead(uint16_t registerAddress, uint16_t* value)
{
//    if (value == NULL)
//    {
//        return -1;
//    }
//
//    bsp_i2c_inst_t* i2c = (bsp_i2c_inst_t*)BOARD_GetI2CInstance();
//    return (int)bsp_i2c_Receive(i2c, BSP_TOP_CONTROLLER_I2C_ADDR, registerAddress, (uint8_t*)value, sizeof(uint16_t));
}

int BSP_BatteryChargerTopControllerWrite(uint16_t registerAddress, uint16_t value)
{
//    bsp_i2c_inst_t* i2c = (bsp_i2c_inst_t*)BOARD_GetI2CInstance();
//
//    return (int)bsp_i2c_Send(i2c, BSP_TOP_CONTROLLER_I2C_ADDR, registerAddress, (uint8_t*)&value, sizeof(uint16_t));
}

int BSP_BatteryChargerRead(uint16_t registerAddress, uint16_t* value)
{
//    if (value == NULL)
//    {
//        return -1;
//    }
//
//    bsp_i2c_inst_t* i2c = (bsp_i2c_inst_t*)BOARD_GetI2CInstance();
//    return (int)bsp_i2c_Receive(i2c, BSP_BATTERY_CHARGER_I2C_ADDR, registerAddress, (uint8_t*)value, sizeof(uint16_t));
}

/*BaseType_t BSP_BatteryChargerINOKB_IRQHandler()
{
//	return 0;
}

BaseType_t BSP_BatteryChargerWCINOKB_IRQHandler()
{
//	return 0;
}

BaseType_t BSP_BatteryChargerINTB_IRQHandler()
{
//	return 0;
}*/

/*
 * *****************************************************************************************************************************************************
 * *                                                                                                                                                   *
 * *                                                         STATIC FUNCTIONS                                                                          *
 * *                                                                                                                                                   *
 * *****************************************************************************************************************************************************
 */

	static void s_BSP_BatteryChargerIrqPinsInit()
	{
/*		gpio_pin_config_t pinConfig;

		pinConfig.direction         = kGPIO_DigitalInput;
		pinConfig.interruptMode     = kGPIO_IntRisingOrFallingEdge;
		pinConfig.outputLogic       = 0;

		GPIO_PinInit(BOARD_BATTERY_CHARGER_INOKB_GPIO,      BOARD_BATTERY_CHARGER_INOKB_PIN,    &pinConfig);
		GPIO_PinInit(BOARD_BATTERY_CHARGER_WCINOKB_GPIO,    BOARD_BATTERY_CHARGER_WCINOKB_PIN,  &pinConfig);
		GPIO_PinInit(BOARD_BATTERY_CHARGER_INTB_GPIO,       BOARD_BATTERY_CHARGER_INTB_PIN,     &pinConfig);

		GPIO_PortEnableInterrupts(BOARD_BATTERY_CHARGER_INOKB_GPIO, 1U << BOARD_BATTERY_CHARGER_INOKB_PIN);
		GPIO_PortEnableInterrupts(BOARD_BATTERY_CHARGER_WCINOKB_GPIO, 1U << BOARD_BATTERY_CHARGER_WCINOKB_PIN);
		GPIO_PortEnableInterrupts(BOARD_BATTERY_CHARGER_INTB_GPIO, 1U << BOARD_BATTERY_CHARGER_INTB_PIN);*/

	}

