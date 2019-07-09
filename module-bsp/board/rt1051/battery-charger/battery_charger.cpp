/*
 * battery_charger.cpp
 *
 *  Created on: Jun 28, 2019
 *      Author: kuba
 */
#include "../common/i2c.h"
#include "fsl_gpio.h"
#include "board.h"
#include "battery-charger/battery_charger.hpp"
#include "vfs.hpp"

#define BSP_BATTERY_CHARGER_I2C_ADDR                        (0xD2 >> 1)
#define BSP_FUEL_GAUGE_I2C_ADDR                             (0x6C >> 1)
#define BSP_TOP_CONTROLLER_I2C_ADDR                         (0xCC >> 1)

const char* battery_cfgFile = "batteryAdjustementConfig.cfg";
const char* battery_cfgFileTemp = "batteryAdjustementConfig_new.cfg";
const char* battery_cfgFilePrev = "batteryAdjustementConfig_old.cfg";

static const uint16_t BATT_SERVICE_AVG_CURRENT_PERIOD = 0x00;    //< 0.1758 ms * 2^(2 + BATT_SERVICE_AVG_CURRENT_PERIOD)         == 700ms
static const uint16_t BATT_SERVICE_AVG_CELL_VOLTAGE_PERIOD = 0x00;    //< 0.1758 ms * 2^(6 + BATT_SERVICE_AVG_CELL_VOLTAGE_PERIOD)    == 11.25 s
static const uint16_t BATT_SERVICE_AVG_MIXING_PERIOD = 0x0D;    //< 0.1758 ms * 2^(5 + BATT_SERVICE_AVG_MIXING_PERIOD)          == 12.8 h
static const uint16_t BATT_SERVICE_AVG_TEMP_PERIOD = 0x01;    //< 0.1758 ms * 2^(11 + BATT_SERVICE_AVG_TEMP_PERIOD)           == 12 min
static const uint16_t BATT_SERVICE_AVG_NEMPTY_PERIOD = 0x00;

static const uint16_t battery_nominalCapacitymAh = 3000;

static const uint8_t battery_fullyChargedPercent = 100;
static const uint8_t battery_DischargedPercent = 15;

static const uint8_t battery_maxTemperatureDegrees = 50;
static const uint8_t battery_minTemperatureDegrees = 5;

static const uint16_t battery_maxVoltagemV = 4200;
static const uint16_t battery_minVoltagemV = 3700;


static void s_BSP_BatteryChargerIrqPinsInit();
static bsp::batteryRetval battery_loadConfiguration(void);

static bsp::batteryRetval battery_setAvgCalcPeriods(void);
static bsp::batteryRetval battery_setAvgCalcPeriods(void);
static bsp::batteryRetval battery_setNominalBatteryCapacity(uint16_t capacity);
static bsp::batteryRetval battery_setChargingDischargingThresholds(uint8_t chargedThresholdPercent, uint8_t dischargedThresholdPercent);
static bsp::batteryRetval battery_setTemperatureThresholds(uint8_t maxTemperatureDegrees, uint8_t minTemperatureDegrees);
static bsp::batteryRetval battery_setServiceVoltageThresholds(uint16_t maxVoltage_mV, uint16_t minVoltage_mV);
static bsp::batteryRetval battery_enableAlerts(void);
static bsp::batteryRetval battery_enableIRQs(void);


static xQueueHandle qHandleIrq = NULL;

namespace bsp{

	int battery_Init(xQueueHandle qHandle)
	{
		qHandleIrq = qHandle;

		/*battery_loadConfiguration();
		battery_setAvgCalcPeriods();
		battery_setNominalBatteryCapacity(battery_nominalCapacitymAh);
		battery_setChargingDischargingThresholds(battery_fullyChargedPercent, battery_DischargedPercent);
		battery_setTemperatureThresholds(battery_maxTemperatureDegrees, battery_minTemperatureDegrees);
		battery_setServiceVoltageThresholds(battery_maxVoltagemV, battery_minVoltagemV);*/
		//battery_enableAlerts();


		//battery mask
		uint32_t val;
		//top controller
		val = 0;
		bsp_i2c_Receive(BOARD_GetI2CInstance(), BSP_TOP_CONTROLLER_I2C_ADDR, 0x20, (uint8_t*)&val, 1);
		LOG_INFO("Charger id %x", val);
		//interrupt source
		val = 0;
		bsp_i2c_Send(BOARD_GetI2CInstance(), BSP_TOP_CONTROLLER_I2C_ADDR, 0x23, (uint8_t*)&val, 1 );
		val = 0;
		bsp_i2c_Receive(BOARD_GetI2CInstance(), BSP_TOP_CONTROLLER_I2C_ADDR, 0x23, (uint8_t*)&val, 1);
		LOG_INFO("0x23 interrupt source mask 0x%x", val);



		//charger
		val = 0xb7;
		bsp_i2c_Send(BOARD_GetI2CInstance(), BSP_BATTERY_CHARGER_I2C_ADDR, 0xb1, (uint8_t*)&val, 1 );
		val = 0x0c;
		bsp_i2c_Send(BOARD_GetI2CInstance(), BSP_BATTERY_CHARGER_I2C_ADDR, 0xbd, (uint8_t*)&val, 1 );
		val = 0;
		bsp_i2c_Receive(BOARD_GetI2CInstance(), BSP_BATTERY_CHARGER_I2C_ADDR, 0xb1, (uint8_t*)&val, 1);
		LOG_INFO("0xb1 charger interrupt mask 0x%x", val);


		//fuel guage
		val = 0;
		bsp_i2c_Receive(BOARD_GetI2CInstance(), BSP_FUEL_GAUGE_I2C_ADDR, 0x0, (uint8_t*)&val, 2);
		LOG_INFO("0x00 status reg 0x%x", val);

		val = 0;
				bsp_i2c_Receive(BOARD_GetI2CInstance(), BSP_FUEL_GAUGE_I2C_ADDR, 0x21, (uint8_t*)&val, 2);
				LOG_INFO("0x21 device name reg 0x%x", val);


		val = 0;
		bsp_i2c_Receive(BOARD_GetI2CInstance(), BSP_FUEL_GAUGE_I2C_ADDR, 0x2b, (uint8_t*)&val, 2);
		LOG_INFO("0x2b misc cfg 0x%x", val);
		val |= (1 << 10);

		val = 0;
		bsp_i2c_Receive(BOARD_GetI2CInstance(), BSP_FUEL_GAUGE_I2C_ADDR, 0xbb, (uint8_t*)&val, 2);
		LOG_INFO("0xbb config2 reg 0x%x", val);
		val = 0xffee;
		uint8_t regVal[2] = {  0xff, 0xee };
	//	bsp_i2c_Send(BOARD_GetI2CInstance(), BSP_FUEL_GAUGE_I2C_ADDR, 0xbb, (uint8_t*)&val, 3 );
		bsp_i2c_Send(BOARD_GetI2CInstance(), BSP_FUEL_GAUGE_I2C_ADDR, 0xbb, (uint8_t*)&regVal[0], 1 );
		bsp_i2c_Send(BOARD_GetI2CInstance(), BSP_FUEL_GAUGE_I2C_ADDR, 0xbb, (uint8_t*)&regVal[1], 1 );
		val = 0;
		bsp_i2c_Receive(BOARD_GetI2CInstance(), BSP_FUEL_GAUGE_I2C_ADDR, 0xbb, (uint8_t*)&val, 2);
		LOG_INFO("0xbb config2 reg 0x%x", val);

		val = 0;
				bsp_i2c_Receive(BOARD_GetI2CInstance(), BSP_FUEL_GAUGE_I2C_ADDR, 0x1d, (uint8_t*)&val, 2);
				LOG_INFO("0x1d config reg 0x%x", val);

		val = 0x4005;
		bsp_i2c_Send(BOARD_GetI2CInstance(), BSP_FUEL_GAUGE_I2C_ADDR, 0x1d, (uint8_t*)&val, 2 );
		val = 0;
		bsp_i2c_Receive(BOARD_GetI2CInstance(), BSP_FUEL_GAUGE_I2C_ADDR, 0x1d, (uint8_t*)&val, 2);
		LOG_INFO("0x1d config reg 0x%x", val);



		battery_enableIRQs();
		/*	bsp::battery_fuelGaugeWrite(static_cast<bsp::batteryChargerRegisters>(0xbb), config2);
			bsp::battery_fuelGaugeRead(static_cast<bsp::batteryChargerRegisters>(0xbb), &val);
			LOG_INFO("Config2 reg 0x%x", val);
			mask = 1<<7;
	*/
		//irq status
/*
		bsp::battery_chargerTopControllerRead(bsp::batteryChargerRegisters::TOP_CONTROLL_IRQ_SRC_REG, &val);
		LOG_INFO("TOP_CONTROLL_IRQ_SRC_REG reg 0x%x", val);
*/

		s_BSP_BatteryChargerIrqPinsInit();

		return 0;
	}

	int battery_fuelGaugeWrite(bsp::batteryChargerRegisters registerAddress, uint16_t value)
	{
		bsp_i2c_inst_t* i2c = (bsp_i2c_inst_t*)BOARD_GetI2CInstance();
		return (int)bsp_i2c_Send(i2c, BSP_FUEL_GAUGE_I2C_ADDR, static_cast<uint32_t>(registerAddress), (uint8_t*)&value, sizeof(uint16_t));
	}

	int battery_fuelGaugeRead(bsp::batteryChargerRegisters registerAddress, uint16_t* value)
	{
		if (value == NULL)
		{
			return -1;
		}

		bsp_i2c_inst_t* i2c = (bsp_i2c_inst_t*)BOARD_GetI2CInstance();
		return (int)bsp_i2c_Receive(i2c, BSP_FUEL_GAUGE_I2C_ADDR, static_cast<uint32_t>(registerAddress), (uint8_t*)value, sizeof(uint16_t));
	}

	int battery_chargerWrite(bsp::batteryChargerRegisters registerAddress, uint16_t value)
	{
		bsp_i2c_inst_t* i2c = (bsp_i2c_inst_t*)BOARD_GetI2CInstance();

		return (int)bsp_i2c_Send(i2c, BSP_BATTERY_CHARGER_I2C_ADDR, static_cast<uint32_t>(registerAddress), (uint8_t*)&value, sizeof(uint16_t));
	}

	int battery_chargerRead(bsp::batteryChargerRegisters registerAddress, uint16_t* value)
	{
		if (value == NULL)
		{
			return -1;
		}

		bsp_i2c_inst_t* i2c = (bsp_i2c_inst_t*)BOARD_GetI2CInstance();
		return (int)bsp_i2c_Receive(i2c, BSP_BATTERY_CHARGER_I2C_ADDR, static_cast<uint32_t>(registerAddress), (uint8_t*)value, sizeof(uint16_t));
	}

	int battery_chargerTopControllerWrite(bsp::batteryChargerRegisters registerAddress, uint16_t value)
	{
		bsp_i2c_inst_t* i2c = (bsp_i2c_inst_t*)BOARD_GetI2CInstance();

		return (int)bsp_i2c_Send(i2c, BSP_TOP_CONTROLLER_I2C_ADDR, static_cast<uint32_t>(registerAddress), (uint8_t*)&value, sizeof(uint16_t));
	}

	int battery_chargerTopControllerRead(bsp::batteryChargerRegisters registerAddress, uint16_t* value)
	{
		if (value == NULL)
		{
			return -1;
		}

		bsp_i2c_inst_t* i2c = (bsp_i2c_inst_t*)BOARD_GetI2CInstance();
		return (int)bsp_i2c_Receive(i2c, BSP_TOP_CONTROLLER_I2C_ADDR, static_cast<uint32_t>(registerAddress), (uint8_t*)value, sizeof(uint16_t));
	}

	void battery_getData(uint8_t& levelPercent)
	{
			levelPercent = 50;
	}

	void battery_getChargeStatus( bool& status)
	{
		uint16_t val = 0;
		val = 0;
		bsp_i2c_Receive(BOARD_GetI2CInstance(), BSP_BATTERY_CHARGER_I2C_ADDR, 0xb2, (uint8_t*)&val, 1);
		LOG_INFO("0xb4 chg_details reg 0x%x", val);
		if(val & 0x40)
			status = true;
		else
			status = false;
	}
}

static bsp::batteryRetval battery_loadConfiguration(void)
{
	vfs::FILE* fd = NULL;

	fd = vfs.fopen(battery_cfgFile, "r");
	if(fd == NULL)
	{
		LOG_WARN("Configuration file [%s] not found. Searching for file [%s]", battery_cfgFile, battery_cfgFilePrev);
		fd = vfs.fopen(battery_cfgFilePrev, "r");
		if(fd == NULL)
		{
			LOG_WARN("Configuration file [%s] not found. Searching for file [%s]", battery_cfgFilePrev, battery_cfgFileTemp);
			fd = vfs.fopen(battery_cfgFileTemp, "r");
			if(fd == NULL)
			{
				LOG_WARN("Configuration file [%s] not found.", battery_cfgFileTemp);
				return bsp::batteryRetval::battery_ChargerError;
			}
		}
	}

	uint16_t regValue = 0;;
	for(uint8_t i = 0; i < 0xff; ++i)
	{
		if( vfs.fread(&regValue, sizeof(regValue), 1, fd) != sizeof(regValue) )
		{
			LOG_ERROR("Reading register 0x%x failed.", i);
			vfs.fclose(fd);
			return bsp::batteryRetval::battery_ChargerError;
		}

		if( battery_fuelGaugeWrite(static_cast<bsp::batteryChargerRegisters>(i), regValue) !=kStatus_Success )
		{
			LOG_ERROR("Writing register 0x%x failed.", i);
			vfs.fclose(fd);
			return bsp::batteryRetval::battery_ChargerError;
		}
	}
	return bsp::batteryRetval::battery_OK;
}

static bsp::batteryRetval battery_setAvgCalcPeriods(void)
{
	uint16_t regVal = 0;
	regVal |= (BATT_SERVICE_AVG_CURRENT_PERIOD          << 0);
	regVal |= (BATT_SERVICE_AVG_CELL_VOLTAGE_PERIOD     << 4);
	regVal |= (BATT_SERVICE_AVG_MIXING_PERIOD           << 7);
	regVal |= (BATT_SERVICE_AVG_TEMP_PERIOD             << 11);
	regVal |= (BATT_SERVICE_AVG_NEMPTY_PERIOD           << 14);

	if( bsp::battery_fuelGaugeWrite(bsp::batteryChargerRegisters::FilterCFG_REG, regVal) != kStatus_Success )
	{
		LOG_ERROR("battery_setAvgCalcPeriods failed.");
		return bsp::batteryRetval::battery_ChargerError;
	}
	return bsp::batteryRetval::battery_OK;
}

static bsp::batteryRetval battery_setNominalBatteryCapacity(uint16_t capacity)
{
	uint16_t regVal = capacity * 2;

	if( bsp::battery_fuelGaugeWrite(bsp::batteryChargerRegisters::DesignCap_REG, regVal) != kStatus_Success )
	{
		LOG_ERROR("battery_setNominalBatteryCapacity failed.");
		return bsp::batteryRetval::battery_ChargerError;
	}
	return bsp::batteryRetval::battery_OK;
}

static bsp::batteryRetval battery_setChargingDischargingThresholds(uint8_t chargedThresholdPercent, uint8_t dischargedThresholdPercent)
{
	uint16_t regVal = (chargedThresholdPercent << 8) | dischargedThresholdPercent;

	if( bsp::battery_fuelGaugeWrite(bsp::batteryChargerRegisters::SALRT_Th_REG, regVal) != kStatus_Success )
	{
		LOG_ERROR("battery_setChargingDischargingThresholds failed.");
		return bsp::batteryRetval::battery_ChargerError;
	}
	return bsp::batteryRetval::battery_OK;
}

static bsp::batteryRetval battery_setTemperatureThresholds(uint8_t maxTemperatureDegrees, uint8_t minTemperatureDegrees)
{
	uint16_t regVal = (maxTemperatureDegrees << 8) | minTemperatureDegrees;

	if( bsp::battery_fuelGaugeWrite(bsp::batteryChargerRegisters::TALRT_Th_REG, regVal) != kStatus_Success )
	{
		LOG_ERROR("battery_setTemperatureThresholds failed.");
		return bsp::batteryRetval::battery_ChargerError;
	}
	return bsp::batteryRetval::battery_OK;
}

static bsp::batteryRetval battery_setServiceVoltageThresholds(uint16_t maxVoltage_mV, uint16_t minVoltage_mV)
{
	uint16_t regVal =  ((maxVoltage_mV / 20) << 8) | (minVoltage_mV / 20);

	if( bsp::battery_fuelGaugeWrite(bsp::batteryChargerRegisters::VALRT_Th_REG, regVal) != kStatus_Success )
	{

		LOG_ERROR("battery_setServiceVoltageThresholds failed.");
		return bsp::batteryRetval::battery_ChargerError;
	}
	return bsp::batteryRetval::battery_OK;
}

static bsp::batteryRetval battery_enableAlerts(void)
{
	const uint16_t SOC_ALERT_STICKY     = (1 << 14);
	const uint16_t TEMP_ALERT_STICKY    = (1 << 13);
	const uint16_t VOLTAGE_ALERT_STICKY = (1 << 12);
	const uint16_t ENABLE_ALERTS        = (1 << 2);

	uint16_t val;
	bsp::battery_fuelGaugeRead(bsp::batteryChargerRegisters::CONFIG_REG, &val);
	LOG_INFO("ALERTS: 0x%x", val);
	uint16_t regVal = SOC_ALERT_STICKY | TEMP_ALERT_STICKY | VOLTAGE_ALERT_STICKY | ENABLE_ALERTS;

	if( bsp::battery_fuelGaugeWrite(bsp::batteryChargerRegisters::CONFIG_REG, regVal) != kStatus_Success )
	{
		LOG_ERROR("battery_enableAlerts failed.");
		return bsp::batteryRetval::battery_ChargerError;
	}
	bsp::battery_fuelGaugeRead(bsp::batteryChargerRegisters::CONFIG_REG, &val);
		LOG_INFO("ALERTS: 0x%x", val);
	return bsp::batteryRetval::battery_OK;
}

static bsp::batteryRetval battery_enableIRQs(void)
{
	uint16_t val = 0xf8;

	if( bsp::battery_chargerTopControllerRead(bsp::batteryChargerRegisters::TOP_CONTROLL_IRQ_MASK_REG, &val) != kStatus_Success )
	{
		LOG_ERROR("battery_enableIRQs read failed.");
		return bsp::batteryRetval::battery_ChargerError;
	}

	return bsp::batteryRetval::battery_OK;
}

BaseType_t BSP_BatteryChargerINOKB_IRQHandler()
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	 if(qHandleIrq != NULL){
	        	uint8_t val = 0x02;
	        	xQueueSendFromISR(qHandleIrq, &val, &xHigherPriorityTaskWoken );
	        }
	return xHigherPriorityTaskWoken;


}

BaseType_t BSP_BatteryChargerWCINOKB_IRQHandler()
{
	return 0;
}

BaseType_t BSP_BatteryChargerINTB_IRQHandler()
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	 if(qHandleIrq != NULL){
	        	uint8_t val = 0x01;
	        	xQueueSendFromISR(qHandleIrq, &val, &xHigherPriorityTaskWoken );
	        }
	return xHigherPriorityTaskWoken;
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




