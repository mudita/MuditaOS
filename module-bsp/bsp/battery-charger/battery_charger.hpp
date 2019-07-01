/*
 * battery_charger.hpp
 *
 *  Created on: Jun 28, 2019
 *      Author: kuba
 */

#ifndef MODULE_BSP_BSP_BATTERY_CHARGER_BATTERY_CHARGER_HPP_
#define MODULE_BSP_BSP_BATTERY_CHARGER_BATTERY_CHARGER_HPP_

#define BSP_BATT_CHG_TOP_CONTROLL_PMIC_ID_REG       0x0020
#define BSP_BATT_CHG_TOP_CONTROLL_PMIC_VER_REG      0x0021
#define BSP_BATT_CHG_TOP_CONTROLL_IRQ_SRC_REG       0x0022
#define BSP_BATT_CHG_TOP_CONTROLL_IRQ_MASK_REG      0x0023

#define BSP_FUEL_GAUGE_STATUS_REG                   0x0000
#define BSP_FUEL_GAUGE_VALRT_Th_REG                 0x0001
#define BSP_FUEL_GAUGE_TALRT_Th_REG                 0x0002
#define BSP_FUEL_GAUGE_SALRT_Th_REG                 0x0003
#define BSP_FUEL_GAUGE_AtRate_REG                   0x0004
#define BSP_FUEL_GAUGE_RepCap_REG                   0x0005
#define BSP_FUEL_GAUGE_RepSOC_REG                   0x0006
#define BSP_FUEL_GAUGE_Age_REG                      0x0007
#define BSP_FUEL_GAUGE_TEMP_REG                     0x0008
#define BSP_FUEL_GAUGE_VCELL_REG                    0x0009
#define BSP_FUEL_GAUGE_Current_REG                  0x000A
#define BSP_FUEL_GAUGE_AvgCurrent_REG               0x000B
#define BSP_FUEL_GAUGE_QResidual_REG                0x000C
#define BSP_FUEL_GAUGE_MixSOC_REG                   0x000D
#define BSP_FUEL_GAUGE_AvSOC_REG                    0x000E
#define BSP_FUEL_GAUGE_MixCap_REG                   0x000F

#define BSP_FUEL_GAUGE_FullCAP_REG                  0x0010
#define BSP_FUEL_GAUGE_TTE_REG                      0x0011
#define BSP_FUEL_GAUGE_QRtable00_REG                0x0012
#define BSP_FUEL_GAUGE_FullSOCthr_REG               0x0013
#define BSP_FUEL_GAUGE_RSLOW_REG                    0x0014
#define BSP_FUEL_GAUGE_AvgTA_REG                    0x0016
#define BSP_FUEL_GAUGE_Cycles_REG                   0x0017
#define BSP_FUEL_GAUGE_DesignCap_REG                0x0018
#define BSP_FUEL_GAUGE_AvgVCELL_REG                 0x0019
#define BSP_FUEL_GAUGE_MaxMinTemp_REG               0x001A
#define BSP_FUEL_GAUGE_MaxMinVolt_REG               0x001B
#define BSP_FUEL_GAUGE_MaxMinCurr_REG               0x001C
#define BSP_FUEL_GAUGE_CONFIG_REG                   0x001D
#define BSP_FUEL_GAUGE_ICHGTERM_REG                 0x001E
#define BSP_FUEL_GAUGE_AvCap_REG                    0x001F

#define BSP_FUEL_GAUGE_TTF_REG                      0x0020
#define BSP_FUEL_GAUGE_DevName_REG                  0x0021
#define BSP_FUEL_GAUGE_QRtable10_REG                0x0022
#define BSP_FUEL_GAUGE_FullCAPNom_REG               0x0023
#define BSP_FUEL_GAUGE_TempNom_REG                  0x0024
#define BSP_FUEL_GAUGE_TempLim_REG                  0x0025
#define BSP_FUEL_GAUGE_AIN0_REG                     0x0027
#define BSP_FUEL_GAUGE_LearnCFG_REG                 0x0028
#define BSP_FUEL_GAUGE_FilterCFG_REG                0x0029
#define BSP_FUEL_GAUGE_RelaxCFG_REG                 0x002A
#define BSP_FUEL_GAUGE_MiscCFG_REG                  0x002B
#define BSP_FUEL_GAUGE_TGAIN_REG                    0x002C
#define BSP_FUEL_GAUGE_TOFF_REG                     0x002D
#define BSP_FUEL_GAUGE_CGAIN_REG                    0x002E
#define BSP_FUEL_GAUGE_COFF_REG                     0x002F

#define BSP_FUEL_GAUGE_QRtable20_REG                0x0032
#define BSP_FUEL_GAUGE_AtTTF_REG                    0x0033
#define BSP_FUEL_GAUGE_FullCapRep_REG               0x0035
#define BSP_FUEL_GAUGE_lavgEmpty_REG                0x0036
#define BSP_FUEL_GAUGE_FCTC_REG                     0x0037
#define BSP_FUEL_GAUGE_RCOMP0_REG                   0x0038
#define BSP_FUEL_GAUGE_TempCo_REG                   0x0039
#define BSP_FUEL_GAUGE_VEmpty_REG                   0x003A
#define BSP_FUEL_GAUGE_TIMER_REG                    0x003E
#define BSP_FUEL_GAUGE_SHDNTIMER_REG                0x003F

#define BSP_FUEL_GAUGE_QRtable30_REG                0x0042
#define BSP_FUEL_GAUGE_dQ_acc_REG                   0x0045
#define BSP_FUEL_GAUGE_dP_acc_REG                   0x0046
#define BSP_FUEL_GAUGE_ConvgCfg_REG                 0x0049
#define BSP_FUEL_GAUGE_VFRemCap_REG                 0x004A
#define BSP_FUEL_GAUGE_QH_REG                       0x004D

int BSP_BatteryChargerInit();

int BSP_FuelGaugeWrite(uint16_t registerAddress, uint16_t value);

int BSP_FuelGaugeRead(uint16_t registerAddress, uint16_t* value);

int BSP_BatteryChargerWrite(uint16_t registerAddress, uint16_t value);

int BSP_BatteryChargerRead(uint16_t registerAddress, uint16_t* value);

int BSP_BatteryChargerTopControllerWrite(uint16_t registerAddress, uint16_t value);

int BSP_BatteryChargerTopControllerRead(uint16_t registerAddress, uint16_t* value);



BaseType_t BSP_BatteryChargerINOKB_IRQHandler();

BaseType_t BSP_BatteryChargerWCINOKB_IRQHandler();

BaseType_t BSP_BatteryChargerINTB_IRQHandler();


#endif /* MODULE_BSP_BSP_BATTERY_CHARGER_BATTERY_CHARGER_HPP_*/
