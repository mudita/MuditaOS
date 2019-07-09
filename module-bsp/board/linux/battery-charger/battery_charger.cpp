/*
 * battery_charger.cpp
 *
 *  Created on: Jul 1, 2019
 *      Author: kuba
 */
#include <stdint.h>
#include "FreeRTOS.h"

#include <sys/stat.h>
#include <fcntl.h>

#include "board.h"
#include "battery-charger/battery_charger.hpp"

#define BSP_BATTERY_CHARGER_I2C_ADDR                        (0xD2 >> 1)
#define BSP_FUEL_GAUGE_I2C_ADDR                             (0x6C >> 1)
#define BSP_TOP_CONTROLLER_I2C_ADDR                         (0xCC >> 1)




static const uint16_t chargerRegs [][2] = {
		{static_cast<uint16_t>(bsp::batteryChargerRegisters::RepCap_REG) , 2000},
		{0,0}
};

static const uint32_t secondsToTick = 2;

static xQueueHandle qHandleIrq = NULL;
static TaskHandle_t battery_worker_handle = NULL;

static uint8_t battLevel = 100;
static bool plugged = false;
namespace bsp{

	static void battery_worker(void *pvp);

	int battery_Init(xQueueHandle qHandle)
	{
		qHandleIrq = qHandle;
		if (xTaskCreate(battery_worker, "keyboard", 512, qHandle, 0, &battery_worker_handle) != pdPASS) {
				return 1;
			}
		return 0;
	}

	void battery_getData(uint8_t& levelPercent)
	{
			levelPercent = battLevel;
	}

	void battery_getChargeStatus( bool& status)
	{
		status = plugged;
	}
	int battery_fuelGaugeWrite(bsp::batteryChargerRegisters registerAddress, uint16_t value)
	{

	//    bsp_i2c_inst_t* i2c = (bsp_i2c_inst_t*)BOARD_GetI2CInstance();
	//    return (int)bsp_i2c_Send(i2c, BSP_FUEL_GAUGE_I2C_ADDR, registerAddress, (uint8_t*)&value, sizeof(uint16_t));
	}

	int battery_fuelGaugeRead(bsp::batteryChargerRegisters registerAddress, uint16_t* value)
	{
		uint32_t i = 0;
		while(chargerRegs[i][0] != 0)
		{
			if( chargerRegs[i][0] == static_cast<uint16_t>(registerAddress) )
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

	int battery_chargerWrite(bsp::batteryChargerRegisters registerAddress, uint16_t value)
	{
	//    bsp_i2c_inst_t* i2c = (bsp_i2c_inst_t*)BOARD_GetI2CInstance();
	//
	//    return (int)bsp_i2c_Send(i2c, BSP_BATTERY_CHARGER_I2C_ADDR, registerAddress, (uint8_t*)&value, sizeof(uint16_t));
	}

	int battery_chargerRead(bsp::batteryChargerRegisters registerAddress, uint16_t* value)
	{
	//    if (value == NULL)
	//    {
	//        return -1;
	//    }
	//
	//    bsp_i2c_inst_t* i2c = (bsp_i2c_inst_t*)BOARD_GetI2CInstance();
	//    return (int)bsp_i2c_Receive(i2c, BSP_BATTERY_CHARGER_I2C_ADDR, registerAddress, (uint8_t*)value, sizeof(uint16_t));
	}


	int battery_chargerTopControllerWrite(bsp::batteryChargerRegisters registerAddress, uint16_t value)
	{
	//    bsp_i2c_inst_t* i2c = (bsp_i2c_inst_t*)BOARD_GetI2CInstance();
	//
	//    return (int)bsp_i2c_Send(i2c, BSP_TOP_CONTROLLER_I2C_ADDR, registerAddress, (uint8_t*)&value, sizeof(uint16_t));
	}

	int battery_chargerTopControllerRead(bsp::batteryChargerRegisters registerAddress, uint16_t* value)
	{
	//    if (value == NULL)
	//    {
	//        return -1;
	//    }
	//
	//    bsp_i2c_inst_t* i2c = (bsp_i2c_inst_t*)BOARD_GetI2CInstance();
	//    return (int)bsp_i2c_Receive(i2c, BSP_TOP_CONTROLLER_I2C_ADDR, registerAddress, (uint8_t*)value, sizeof(uint16_t));
	}

	static void battery_worker(void *pvp)
	{

			const char * myfifo = "/tmp/fifoBattKeys";

			// Creating the named file(FIFO)
			// mkfifo(<pathname>, <permission>)
			mkfifo(myfifo, 0666);

			// Open FIFO for write only
			int fd;
			fd = open(myfifo, O_RDONLY | O_NONBLOCK );


			while(1)
			{
				uint8_t buff [10];
				int32_t readedBytes = read(fd, buff, 10);

				if(readedBytes > 0)
				{



					uint8_t notification = 0;
					switch (buff[0])
					{
					case 'p':
						notification = 0x02;
						plugged = 1 - plugged;
						break;
					case ']':
						notification = 0x01;
						if(battLevel < 100)
							battLevel++;
						break;
					case '[':
						notification = 0x01;
						if(battLevel > 1)
							battLevel--;
					break;
					}
					xQueueSend(qHandleIrq, &notification, 100);
				}
				vTaskDelay(50);
		}
	}
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

/*	static void s_BSP_BatteryChargerIrqPinsInit()
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

	}*/

