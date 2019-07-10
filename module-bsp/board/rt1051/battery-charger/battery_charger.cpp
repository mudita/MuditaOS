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

static bsp::batteryRetval battery_loadConfiguration(void);
static bsp::batteryRetval battery_storeConfiguration(void);
static int battery_fuelGaugeWrite(bsp::batteryChargerRegisters registerAddress, uint16_t value);
static int battery_fuelGaugeRead(bsp::batteryChargerRegisters registerAddress, uint16_t* value);
static int battery_chargerWrite(bsp::batteryChargerRegisters registerAddress, uint8_t value);
static int battery_chargerRead(bsp::batteryChargerRegisters registerAddress, uint8_t* value);
static int battery_chargerTopControllerWrite(bsp::batteryChargerRegisters registerAddress, uint8_t value);
static int battery_chargerTopControllerRead(bsp::batteryChargerRegisters registerAddress, uint8_t* value);
static bsp::batteryRetval battery_setAvgCalcPeriods(void);
static bsp::batteryRetval battery_setAvgCalcPeriods(void);
static bsp::batteryRetval battery_setNominalBatteryCapacity(uint16_t capacity);
static bsp::batteryRetval battery_setChargingDischargingThresholds(uint8_t chargedThresholdPercent, uint8_t dischargedThresholdPercent);
static bsp::batteryRetval battery_setTemperatureThresholds(uint8_t maxTemperatureDegrees, uint8_t minTemperatureDegrees);
static bsp::batteryRetval battery_setServiceVoltageThresholds(uint16_t maxVoltage_mV, uint16_t minVoltage_mV);
static bsp::batteryRetval battery_enableFuelGuageIRQs(void);
static bsp::batteryRetval battery_enableTopIRQs(void);
static bsp::batteryRetval battery_disableAlerts(void);
static void s_BSP_BatteryChargerIrqPinsInit();

static xQueueHandle qHandleIrq = NULL;

namespace bsp{


	int battery_Init(xQueueHandle qHandle)
	{
		qHandleIrq = qHandle;

		//check Power-On reset bit
		uint16_t status;
		const uint16_t porMask = 0x0002;
		battery_fuelGaugeRead(bsp::batteryChargerRegisters::STATUS_REG, &status);

		if(status & porMask)
		{
			LOG_INFO("Initializing battery charger");
			battery_loadConfiguration();
			battery_setAvgCalcPeriods();
			battery_setNominalBatteryCapacity(battery_nominalCapacitymAh);
			battery_setChargingDischargingThresholds(battery_fullyChargedPercent, battery_DischargedPercent);
			battery_setTemperatureThresholds(battery_maxTemperatureDegrees, battery_minTemperatureDegrees);
			battery_setServiceVoltageThresholds(battery_maxVoltagemV, battery_minVoltagemV);
		}

		battery_disableAlerts();
		battery_enableFuelGuageIRQs();

		battery_ClearAllIRQs();
		battery_enableTopIRQs();

		s_BSP_BatteryChargerIrqPinsInit();

		return 0;
	}

	void battery_Deinit(void)
	{
		battery_storeConfiguration();

		GPIO_PortDisableInterrupts(BOARD_BATTERY_CHARGER_INOKB_GPIO, 1U << BOARD_BATTERY_CHARGER_INOKB_PIN);
		GPIO_PortDisableInterrupts(BOARD_BATTERY_CHARGER_WCINOKB_GPIO, 1U << BOARD_BATTERY_CHARGER_WCINOKB_PIN);
		GPIO_PortDisableInterrupts(BOARD_BATTERY_CHARGER_INTB_GPIO, 1U << BOARD_BATTERY_CHARGER_INTB_PIN);

		qHandleIrq = NULL;
	}


	void battery_getBatteryLevel(uint8_t& levelPercent)
	{
        uint16_t val;
        battery_fuelGaugeRead(bsp::batteryChargerRegisters::RepSOC_REG, &val);
        uint16_t percent = val & 0xff00;
        percent = percent >> 8;
        levelPercent = percent;
	}

	void battery_getChargeStatus( bool& status)
	{
		uint8_t val = 0;
		val = 0;
		battery_chargerRead(bsp::batteryChargerRegisters::CHG_INT_OK, &val);
		if(val & 0x40)
			status = true;
		else
			status = false;
	}

	void battery_ClearAllIRQs(void)
	{
		uint8_t val;
		battery_chargerRead(bsp::batteryChargerRegisters::CHG_INT_REG, &val);
		LOG_INFO("charger interrupts 0x%x", val);
		if(val != 0 )
		{
			//write zero to clear irq source
			battery_chargerWrite(bsp::batteryChargerRegisters::CHG_INT_REG, 0);
			LOG_INFO("Clear charger interrupts");
		}

		uint16_t status = 0;
		battery_fuelGaugeRead(bsp::batteryChargerRegisters::STATUS_REG, &status);
		LOG_INFO("fuelGuage interrupts 0x%x", status);
		if(status != 0)
		{
			//write zero to clear irq source
			battery_fuelGaugeWrite(bsp::batteryChargerRegisters::STATUS_REG, 0);
			LOG_INFO("Clear fuelGuage interrupts");
		}
	}

	void battery_clearFuelGuageIRQ(void)
	{
		//write zero to clear interrupt source
		battery_fuelGaugeWrite(bsp::batteryChargerRegisters::STATUS_REG, 0x0000);
	}
}

static int battery_fuelGaugeWrite(bsp::batteryChargerRegisters registerAddress, uint16_t value)
{
	bsp_i2c_inst_t* i2c = (bsp_i2c_inst_t*)BOARD_GetI2CInstance();
	return (int)bsp_i2c_Send(i2c, BSP_FUEL_GAUGE_I2C_ADDR, static_cast<uint32_t>(registerAddress), (uint8_t*)&value, sizeof(uint16_t));
}

static int battery_fuelGaugeRead(bsp::batteryChargerRegisters registerAddress, uint16_t* value)
{
	if (value == NULL)
	{
		return -1;
	}

	bsp_i2c_inst_t* i2c = (bsp_i2c_inst_t*)BOARD_GetI2CInstance();
	return (int)bsp_i2c_Receive(i2c, BSP_FUEL_GAUGE_I2C_ADDR, static_cast<uint32_t>(registerAddress), (uint8_t*)value, sizeof(uint16_t));
}

static int battery_chargerWrite(bsp::batteryChargerRegisters registerAddress, uint8_t value)
{
	bsp_i2c_inst_t* i2c = (bsp_i2c_inst_t*)BOARD_GetI2CInstance();

	return (int)bsp_i2c_Send(i2c, BSP_BATTERY_CHARGER_I2C_ADDR, static_cast<uint32_t>(registerAddress), &value, 1);
}

static int battery_chargerRead(bsp::batteryChargerRegisters registerAddress, uint8_t* value)
{
	if (value == NULL)
	{
		return -1;
	}

	bsp_i2c_inst_t* i2c = (bsp_i2c_inst_t*)BOARD_GetI2CInstance();
	return (int)bsp_i2c_Receive(i2c, BSP_BATTERY_CHARGER_I2C_ADDR, static_cast<uint32_t>(registerAddress), (uint8_t*)value, 1);
}

static int battery_chargerTopControllerWrite(bsp::batteryChargerRegisters registerAddress, uint8_t value)
{
	bsp_i2c_inst_t* i2c = (bsp_i2c_inst_t*)BOARD_GetI2CInstance();

	return (int)bsp_i2c_Send(i2c, BSP_TOP_CONTROLLER_I2C_ADDR, static_cast<uint32_t>(registerAddress), (uint8_t*)&value, 1);
}

static int battery_chargerTopControllerRead(bsp::batteryChargerRegisters registerAddress, uint8_t* value)
{
	if (value == NULL)
	{
		return -1;
	}

	bsp_i2c_inst_t* i2c = (bsp_i2c_inst_t*)BOARD_GetI2CInstance();
	return (int)bsp_i2c_Receive(i2c, BSP_TOP_CONTROLLER_I2C_ADDR, static_cast<uint32_t>(registerAddress), (uint8_t*)value, 1);
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
			LOG_WARN("Configuration file [%s] not found.", battery_cfgFilePrev);
			return bsp::batteryRetval::battery_ChargerError;
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

		if( battery_fuelGaugeWrite(static_cast<bsp::batteryChargerRegisters>(i), regValue) != kStatus_Success )
		{
			LOG_ERROR("Writing register 0x%x failed.", i);
			vfs.fclose(fd);
			return bsp::batteryRetval::battery_ChargerError;
		}
	}

	return bsp::batteryRetval::battery_OK;
}

static bsp::batteryRetval battery_storeConfiguration(void)
{
	ff_rename(battery_cfgFile, battery_cfgFilePrev, false);

	vfs::FILE* fd = NULL;

	fd = vfs.fopen(battery_cfgFile, "w");
	if(fd == NULL)
	{
		LOG_ERROR("Could not open configuration file");
		return bsp::batteryRetval::battery_ChargerError;
	}

	uint16_t regVal = 0;
	for(uint32_t i = 0; i < 0xff; ++i)
	{
		if( battery_fuelGaugeRead(static_cast<bsp::batteryChargerRegisters>(i), &regVal) != kStatus_Success )
		{
			LOG_ERROR("Reading register 0x%x failed.", i);
			vfs.fclose(fd);
			return bsp::batteryRetval::battery_ChargerError;
		}

		if( vfs.fwrite(&regVal, sizeof(regVal), 1, fd) != sizeof(regVal) )
		{
			LOG_ERROR("Storing register 0x%x failed.", i);
			vfs.fclose(fd);
			vfs.remove(battery_cfgFile);
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

	if( battery_fuelGaugeWrite(bsp::batteryChargerRegisters::FilterCFG_REG, regVal) != kStatus_Success )
	{
		LOG_ERROR("battery_setAvgCalcPeriods failed.");
		return bsp::batteryRetval::battery_ChargerError;
	}
	return bsp::batteryRetval::battery_OK;
}

static bsp::batteryRetval battery_setNominalBatteryCapacity(uint16_t capacity)
{
	uint16_t regVal = capacity * 2;

	if( battery_fuelGaugeWrite(bsp::batteryChargerRegisters::DesignCap_REG, regVal) != kStatus_Success )
	{
		LOG_ERROR("battery_setNominalBatteryCapacity failed.");
		return bsp::batteryRetval::battery_ChargerError;
	}
	return bsp::batteryRetval::battery_OK;
}

static bsp::batteryRetval battery_setChargingDischargingThresholds(uint8_t chargedThresholdPercent, uint8_t dischargedThresholdPercent)
{
	uint16_t regVal = (chargedThresholdPercent << 8) | dischargedThresholdPercent;

	if( battery_fuelGaugeWrite(bsp::batteryChargerRegisters::SALRT_Th_REG, regVal) != kStatus_Success )
	{
		LOG_ERROR("battery_setChargingDischargingThresholds failed.");
		return bsp::batteryRetval::battery_ChargerError;
	}
	return bsp::batteryRetval::battery_OK;
}

static bsp::batteryRetval battery_setTemperatureThresholds(uint8_t maxTemperatureDegrees, uint8_t minTemperatureDegrees)
{
	uint16_t regVal = (maxTemperatureDegrees << 8) | minTemperatureDegrees;

	if( battery_fuelGaugeWrite(bsp::batteryChargerRegisters::TALRT_Th_REG, regVal) != kStatus_Success )
	{
		LOG_ERROR("battery_setTemperatureThresholds failed.");
		return bsp::batteryRetval::battery_ChargerError;
	}
	return bsp::batteryRetval::battery_OK;
}

static bsp::batteryRetval battery_setServiceVoltageThresholds(uint16_t maxVoltage_mV, uint16_t minVoltage_mV)
{
	uint16_t regVal =  ((maxVoltage_mV / 20) << 8) | (minVoltage_mV / 20);


	if( battery_fuelGaugeWrite(bsp::batteryChargerRegisters::VALRT_Th_REG, regVal) != kStatus_Success )
	{

		LOG_ERROR("battery_setServiceVoltageThresholds failed.");
		return bsp::batteryRetval::battery_ChargerError;
	}
	return bsp::batteryRetval::battery_OK;
}

static bsp::batteryRetval battery_enableFuelGuageIRQs(void)
{
	uint16_t regVal = 0;
	// set dSOCen bit
	regVal |= ( 1 << 7);
	if( battery_fuelGaugeWrite(bsp::batteryChargerRegisters::CONFIG2_REG, regVal) != kStatus_Success )
	{
		LOG_ERROR("battery_enableFuelGuageIRQs failed.");
		return bsp::batteryRetval::battery_ChargerError;
	}

	return bsp::batteryRetval::battery_OK;
}
static bsp::batteryRetval battery_disableAlerts(void)
{
	uint16_t regVal = 0;

	if( battery_fuelGaugeWrite(bsp::batteryChargerRegisters::CONFIG_REG, regVal) != kStatus_Success )
	{
		LOG_ERROR("battery_disableAlerts failed.");
		return bsp::batteryRetval::battery_ChargerError;
	}

	return bsp::batteryRetval::battery_OK;
}
static bsp::batteryRetval battery_enableTopIRQs(void)
{
	uint8_t val = 0xf8;

	if( battery_chargerTopControllerWrite(bsp::batteryChargerRegisters::TOP_CONTROLL_IRQ_MASK_REG, val) != kStatus_Success )
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
		 uint8_t val = static_cast<uint8_t>(bsp::batteryIRQSource::INOKB);
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
		uint8_t val = static_cast<uint8_t>(bsp::batteryIRQSource::INTB);
		xQueueSendFromISR(qHandleIrq, &val, &xHigherPriorityTaskWoken );
	}
	return xHigherPriorityTaskWoken;
}


static void s_BSP_BatteryChargerIrqPinsInit()
{
	gpio_pin_config_t pinConfig;

	pinConfig.direction         = kGPIO_DigitalInput;
	pinConfig.interruptMode     = kGPIO_IntRisingOrFallingEdge;
	pinConfig.outputLogic       = 0;

	GPIO_PinInit(BOARD_BATTERY_CHARGER_INOKB_GPIO,      BOARD_BATTERY_CHARGER_INOKB_PIN,    &pinConfig);
	GPIO_PinInit(BOARD_BATTERY_CHARGER_WCINOKB_GPIO,    BOARD_BATTERY_CHARGER_WCINOKB_PIN,  &pinConfig);

	pinConfig.direction         = kGPIO_DigitalInput;
	pinConfig.interruptMode     = kGPIO_IntFallingEdge;
	pinConfig.outputLogic       = 0;

	GPIO_PinInit(BOARD_BATTERY_CHARGER_INTB_GPIO,       BOARD_BATTERY_CHARGER_INTB_PIN,     &pinConfig);

	GPIO_PortEnableInterrupts(BOARD_BATTERY_CHARGER_INOKB_GPIO, 1U << BOARD_BATTERY_CHARGER_INOKB_PIN);
	GPIO_PortEnableInterrupts(BOARD_BATTERY_CHARGER_WCINOKB_GPIO, 1U << BOARD_BATTERY_CHARGER_WCINOKB_PIN);
	GPIO_PortEnableInterrupts(BOARD_BATTERY_CHARGER_INTB_GPIO, 1U << BOARD_BATTERY_CHARGER_INTB_PIN);

}




