/*
 * battery_charger.hpp
 *
 *  Created on: Jun 28, 2019
 *      Author: kuba
 */

#ifndef MODULE_BSP_BSP_BATTERY_CHARGER_BATTERY_CHARGER_HPP_
#define MODULE_BSP_BSP_BATTERY_CHARGER_BATTERY_CHARGER_HPP_

namespace bsp{

enum class batteryChargerRegisters{
	TOP_CONTROLL_PMIC_ID_REG = 0x0020,
	TOP_CONTROLL_PMIC_VER_REG = 0x0021,
	TOP_CONTROLL_IRQ_SRC_REG = 0x0022,
	TOP_CONTROLL_IRQ_MASK_REG = 0x0023,

	STATUS_REG = 0x0000,
	VALRT_Th_REG = 0x0001,
	TALRT_Th_REG = 0x0002,
	SALRT_Th_REG = 0x0003,
	AtRate_REG = 0x0004,
	RepCap_REG = 0x0005,
	RepSOC_REG = 0x0006,
	Age_REG = 0x0007,
	TEMP_REG = 0x0008,
	VCELL_REG = 0x0009,
	Current_REG = 0x000A,
	AvgCurrent_REG = 0x000B,
	QResidual_REG = 0x000C,
	MixSOC_REG = 0x000D,
	AvSOC_REG = 0x000E,
	MixCap_REG = 0x000F,

	FullCAP_REG = 0x0010,
	TTE_REG = 0x0011,
	QRtable00_REG = 0x0012,
	FullSOCthr_REG = 0x0013,
	RSLOW_REG = 0x0014,
	AvgTA_REG = 0x0016,
	Cycles_REG = 0x0017,
	DesignCap_REG = 0x0018,
	AvgVCELL_REG = 0x0019,
	MaxMinTemp_REG = 0x001A,
	MaxMinVolt_REG = 0x001B,
	MaxMinCurr_REG = 0x001C,
	CONFIG_REG = 0x001D,
	ICHGTERM_REG = 0x001E,
	AvCap_REG = 0x001F,

	TTF_REG = 0x0020,
	DevName_REG = 0x0021,
	QRtable10_REG = 0x0022,
	FullCAPNom_REG = 0x0023,
	TempNom_REG = 0x0024,
	TempLim_REG = 0x0025,
	AIN0_REG = 0x0027,
	LearnCFG_REG = 0x0028,
	FilterCFG_REG = 0x0029,
	RelaxCFG_REG = 0x002A,
	MiscCFG_REG = 0x002B,
	TGAIN_REG = 0x002C,
	TOFF_REG = 0x002D,
	CGAIN_REG = 0x002E,
	COFF_REG = 0x002F,

	QRtable20_REG = 0x0032,
	AtTTF_REG = 0x0033,
	FullCapRep_REG = 0x0035,
	lavgEmpty_REG = 0x0036,
	FCTC_REG = 0x0037,
	RCOMP0_REG = 0x0038,
	TempCo_REG = 0x0039,
	VEmpty_REG = 0x003A,
	TIMER_REG = 0x003E,
	SHDNTIMER_REG = 0x003F,

	QRtable30_REG = 0x0042,
	dQ_acc_REG = 0x0045,
	dP_acc_REG = 0x0046,
	ConvgCfg_REG = 0x0049,
	VFRemCap_REG = 0x004A,
	QH_REG = 0x004D,
};

	enum class batteryRetval{
		battery_OK = 0,
		battery_ChargerError,
		battery_ChargerNotCharging,
		battery_ChargerCharging
	};

	int battery_Init();

	int battery_fuelGaugeWrite(bsp::batteryChargerRegisters registerAddress, uint16_t value);

	int battery_fuelGaugeRead(bsp::batteryChargerRegisters registerAddress, uint16_t* value);

	int battery_chargerWrite(bsp::batteryChargerRegisters registerAddress, uint16_t value);

	int battery_chargerRead(bsp::batteryChargerRegisters registerAddress, uint16_t* value);

	int battery_chargerTopControllerWrite(bsp::batteryChargerRegisters registerAddress, uint16_t value);

	int battery_chargerTopControllerRead(bsp::batteryChargerRegisters registerAddress, uint16_t* value);


}

BaseType_t BSP_BatteryChargerINOKB_IRQHandler();

BaseType_t BSP_BatteryChargerWCINOKB_IRQHandler();

BaseType_t BSP_BatteryChargerINTB_IRQHandler();


#endif /* MODULE_BSP_BSP_BATTERY_CHARGER_BATTERY_CHARGER_HPP_*/
