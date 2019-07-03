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
namespace bsp{

	int battery_Init()
	{
		battery_loadConfiguration();
		battery_setAvgCalcPeriods();
		battery_setNominalBatteryCapacity(battery_nominalCapacitymAh);
		battery_setChargingDischargingThresholds(battery_fullyChargedPercent, battery_DischargedPercent);
		battery_setTemperatureThresholds(battery_maxTemperatureDegrees, battery_minTemperatureDegrees);
		battery_setServiceVoltageThresholds(battery_maxVoltagemV, battery_minVoltagemV);
		battery_enableAlerts();
		bsp::battery_chargerWrite(static_cast<bsp::batteryChargerRegisters>(0xb1), 0x88);
		battery_enableIRQs();
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

	uint16_t regVal = SOC_ALERT_STICKY | TEMP_ALERT_STICKY | VOLTAGE_ALERT_STICKY | ENABLE_ALERTS;

	if( bsp::battery_fuelGaugeWrite(bsp::batteryChargerRegisters::CONFIG_REG, regVal) != kStatus_Success )
	{
		LOG_ERROR("battery_enableAlerts failed.");
		return bsp::batteryRetval::battery_ChargerError;
	}
	return bsp::batteryRetval::battery_OK;
}

static bsp::batteryRetval battery_enableIRQs(void)
{
	uint16_t regVal;
	if( bsp::battery_chargerTopControllerRead(bsp::batteryChargerRegisters::TOP_CONTROLL_IRQ_MASK_REG, &regVal) != kStatus_Success )
	{
		LOG_ERROR("battery_enableIRQs read failed.");
		return bsp::batteryRetval::battery_ChargerError;
	}

	const uint16_t irqMask = 3;
	regVal &= ~irqMask;

	if( bsp::battery_chargerTopControllerWrite(bsp::batteryChargerRegisters::TOP_CONTROLL_IRQ_MASK_REG, regVal) != kStatus_Success )
	{
		LOG_ERROR("battery_enableIRQs write failed.");
		return bsp::batteryRetval::battery_ChargerError;
	}
	return bsp::batteryRetval::battery_OK;
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




