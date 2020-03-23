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
		TOP_CONTROLL_PMIC_ID_REG = 0x20,
		TOP_CONTROLL_PMIC_VER_REG = 0x21,
		TOP_CONTROLL_IRQ_SRC_REG = 0x22,
		TOP_CONTROLL_IRQ_MASK_REG = 0x23,
		SYSTEM_IRQ_REG = 0x24,

		STATUS_REG = 0x00,
		VALRT_Th_REG = 0x01,
		TALRT_Th_REG = 0x02,
		SALRT_Th_REG = 0x03,
		AtRate_REG = 0x04,
		RepCap_REG = 0x05,
		RepSOC_REG = 0x06,
		Age_REG = 0x07,
		TEMP_REG = 0x08,
		VCELL_REG = 0x09,
		Current_REG = 0x0A,
		AvgCurrent_REG = 0x0B,
		QResidual_REG = 0x0C,
		MixSOC_REG = 0x0D,
		AvSOC_REG = 0x0E,
		MixCap_REG = 0x0F,

		FullCAP_REG = 0x10,
		TTE_REG = 0x11,
		QRtable00_REG = 0x12,
		FullSOCthr_REG = 0x13,
		RSLOW_REG = 0x14,
		AvgTA_REG = 0x16,
		Cycles_REG = 0x17,
		DesignCap_REG = 0x18,
		AvgVCELL_REG = 0x19,
		MaxMinTemp_REG = 0x1A,
		MaxMinVolt_REG = 0x1B,
		MaxMinCurr_REG = 0x1C,
		CONFIG_REG = 0x1D,
		CONFIG2_REG = 0xBB,
		ICHGTERM_REG = 0x1E,
		AvCap_REG = 0x1F,

		TTF_REG = 0x20,
		DevName_REG = 0x21,
		QRtable10_REG = 0x22,
		FullCAPNom_REG = 0x23,
		TempNom_REG = 0x24,
		TempLim_REG = 0x25,
		AIN0_REG = 0x27,
		LearnCFG_REG = 0x28,
		FilterCFG_REG = 0x29,
		RelaxCFG_REG = 0x2A,
		MiscCFG_REG = 0x2B,
		TGAIN_REG = 0x2C,
		TOFF_REG = 0x2D,
		CGAIN_REG = 0x2E,
		COFF_REG = 0x2F,

		QRtable20_REG = 0x32,
		AtTTF_REG = 0x33,
		FullCapRep_REG = 0x35,
		lavgEmpty_REG = 0x36,
		FCTC_REG = 0x37,
		RCOMP0_REG = 0x38,
		TempCo_REG = 0x39,
		VEmpty_REG = 0x3A,
		TIMER_REG = 0x3E,
		SHDNTIMER_REG = 0x3F,

		QRtable30_REG = 0x42,
		dQ_acc_REG = 0x45,
		dP_acc_REG = 0x46,
		ConvgCfg_REG = 0x49,
		VFRemCap_REG = 0x4A,
		QH_REG = 0x4D,
		CHG_INT_REG = 0xb0,
		CHG_INT_OK = 0xb2
	};

	enum class batteryRetval{
		battery_OK = 0,
		battery_ChargerError,
		battery_ChargerNotCharging,
		battery_ChargerCharging
	};

	enum class batteryIRQSource{
		INTB = 0x01,
		INOKB = 0x02
	};

	int battery_Init(xQueueHandle qHandle);

	void battery_Deinit(void);

	void battery_getBatteryLevel(uint8_t& levelPercent);

	void battery_getChargeStatus( bool& status);

	void battery_ClearAllIRQs(void);

	void battery_clearFuelGuageIRQ(void);
}

BaseType_t BSP_BatteryChargerINOKB_IRQHandler();

BaseType_t BSP_BatteryChargerINTB_IRQHandler();


#endif /* MODULE_BSP_BSP_BATTERY_CHARGER_BATTERY_CHARGER_HPP_*/
