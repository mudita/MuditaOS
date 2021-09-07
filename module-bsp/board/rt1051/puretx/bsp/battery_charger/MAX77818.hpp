// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#pragma once

namespace bsp::battery_charger
{

    constexpr inline auto BATTERY_CHARGER_I2C_ADDR = 0xD2 >> 1;
    constexpr inline auto FUEL_GAUGE_I2C_ADDR      = 0x6C >> 1;
    constexpr inline auto TOP_CONTROLLER_I2C_ADDR  = 0xCC >> 1;

    enum class Registers
    {
        TOP_CONTROLL_PMIC_ID_REG  = 0x20,
        TOP_CONTROLL_PMIC_VER_REG = 0x21,
        TOP_CONTROLL_IRQ_SRC_REG  = 0x22,
        TOP_CONTROLL_IRQ_MASK_REG = 0x23,
        SYSTEM_IRQ_REG            = 0x24,

        STATUS_REG     = 0x00,
        VALRT_Th_REG   = 0x01,
        TALRT_Th_REG   = 0x02,
        SALRT_Th_REG   = 0x03,
        AtRate_REG     = 0x04,
        RepCap_REG     = 0x05,
        RepSOC_REG     = 0x06,
        Age_REG        = 0x07,
        TEMP_REG       = 0x08,
        VCELL_REG      = 0x09,
        Current_REG    = 0x0A,
        AvgCurrent_REG = 0x0B,
        QResidual_REG  = 0x0C,
        MixSOC_REG     = 0x0D,
        AvSOC_REG      = 0x0E,
        MixCap_REG     = 0x0F,

        FullCAP_REG    = 0x10,
        TTE_REG        = 0x11,
        QRtable00_REG  = 0x12,
        FullSOCthr_REG = 0x13,
        RSLOW_REG      = 0x14,
        AvgTA_REG      = 0x16,
        Cycles_REG     = 0x17,
        DesignCap_REG  = 0x18,
        AvgVCELL_REG   = 0x19,
        MaxMinTemp_REG = 0x1A,
        MaxMinVolt_REG = 0x1B,
        MaxMinCurr_REG = 0x1C,
        CONFIG_REG     = 0x1D,
        CONFIG2_REG    = 0xBB,
        ICHGTERM_REG   = 0x1E,
        AvCap_REG      = 0x1F,

        TTF_REG        = 0x20,
        DevName_REG    = 0x21,
        QRtable10_REG  = 0x22,
        FullCAPNom_REG = 0x23,
        TempNom_REG    = 0x24,
        TempLim_REG    = 0x25,
        AIN0_REG       = 0x27,
        LearnCFG_REG   = 0x28,
        FilterCFG_REG  = 0x29,
        RelaxCFG_REG   = 0x2A,
        MiscCFG_REG    = 0x2B,
        TGAIN_REG      = 0x2C,
        TOFF_REG       = 0x2D,
        CGAIN_REG      = 0x2E,
        COFF_REG       = 0x2F,

        QRtable20_REG  = 0x32,
        AtTTF_REG      = 0x33,
        FullCapRep_REG = 0x35,
        lavgEmpty_REG  = 0x36,
        FCTC_REG       = 0x37,
        RCOMP0_REG     = 0x38,
        TempCo_REG     = 0x39,
        VEmpty_REG     = 0x3A,
        TIMER_REG      = 0x3E,
        SHDNTIMER_REG  = 0x3F,
        CURVE          = 0xb9,

        QRtable30_REG = 0x42,
        dQ_acc_REG    = 0x45,
        dP_acc_REG    = 0x46,
        ConvgCfg_REG  = 0x49,
        VFRemCap_REG  = 0x4A,
        QH_REG        = 0x4D,

        CHG_INT_REG    = 0xB0,
        CHG_INT_MASK   = 0xB1,
        CHG_INT_OK     = 0xB2,
        CHG_DETAILS_00 = 0xB3,
        CHG_DETAILS_01 = 0xB4,
        CHG_DETAILS_02 = 0xB5,
        CHG_CNFG_00    = 0xB7,
        CHG_CNFG_01    = 0xB8,
        CHG_CNFG_02    = 0xB9,
        CHG_CNFG_03    = 0xBA,
        CHG_CNFG_04    = 0xBB,
        CHG_CNFG_05    = 0xBC,
        CHG_CNFG_06    = 0xBD,
        CHG_CNFG_07    = 0xBE,
        CHG_CNFG_09    = 0xC0,
        CHG_CNFG_10    = 0xC1,
        CHG_CNFG_11    = 0xC2,
        CHG_CNFG_12    = 0xC3

    };

    // STATUS register bits
    enum STATUS
    {
        Inm    = (1 << 0),
        POR    = (1 << 1),
        SPR_2  = (1 << 2),
        BST    = (1 << 3),
        Isysmx = (1 << 4),
        SPR_5  = (1 << 5),
        ThmHot = (1 << 6),
        dSOCi  = (1 << 7),
        Vmn    = (1 << 8),
        Tmn    = (1 << 9),
        Smn    = (1 << 10),
        Bi     = (1 << 11),
        Vmx    = (1 << 12),
        Tmx    = (1 << 13),
        Smx    = (1 << 14),
        Br     = (1 << 15),
    };

    /// CHG_INT register
    enum class CHG_INT
    {
        BYP_I   = (1 << 0),
        RSVD_I  = (1 << 1),
        BATP_I  = (1 << 2),
        BAT_I   = (1 << 3),
        CHG_I   = (1 << 4),
        WCIN_I  = (1 << 5),
        CHGIN_I = (1 << 6),
        AICL_I  = (1 << 7),
    };

    enum class CHG_MASK
    {
        BYP_M   = (1 << 0),
        RSVD_M  = (1 << 1),
        BATP_M  = (1 << 2),
        BAT_M   = (1 << 3),
        CHG_M   = (1 << 4),
        WCIN_M  = (1 << 5),
        CHGIN_M = (1 << 6),
        AICL_M  = (1 << 7)
    };

    /// CHG_DETAILS_01 register
    enum class CHG_DETAILS_01
    {
        CHARGER_PREQUALIFICATION = 0x00,
        CHARGER_CC               = 0x01,
        CHARGER_CV               = 0x02,
        CHARGER_TOPOFF           = 0x03,
        CHARGER_DONE             = 0x04,
        CHARGER_TIMER_FAULT      = 0x06,
        CHARGER_BATTERY_DETECT   = 0x07,
        CHARGER_OFF              = 0x08,
    };

    // CHG_CNFG_09 register
    enum class USBCurrentLimit
    {
        lim500mA  = 0x0F,
        lim1000mA = 0x1E,
    };

    // CHG_CNFG_02 register
    enum class ChargeCurrentLimit
    {
        lim300mA  = 0x06, /// est. 0.2C of battery
        lim450mA  = 0x09, /// default
        lim1600mA = 0x20, /// 1C of battery
    };

    enum class ChargeTerminationVoltage
    {
        mV4100 = 0x12, /// 4.1V
        mV4350 = 0x1D, /// 4.35V nominal battery voltage
    };

    // CONFIG register bits
    enum class CONFIG
    {
        Ber    = 1 << 0,
        Bei    = 1 << 1,
        Aen    = 1 << 2,
        FTHRM  = 1 << 3,
        ETHRM  = 1 << 4,
        SPR_5  = 1 << 5,
        I2CSH  = 1 << 6,
        SHDN   = 1 << 7,
        Tex    = 1 << 8,
        Ten    = 1 << 9,
        AINSH  = 1 << 10,
        SPR_11 = 1 << 11,
        Vs     = 1 << 12,
        Ts     = 1 << 13,
        Ss     = 1 << 14,
        SPR_15 = 1 << 15
    };

    // CONFIG2 register bits
    enum class CONFIG2
    {
        ISysNCurr    = 1 << 0,
        OCVQen       = 1 << 4,
        LdMdl        = 1 << 5,
        TAlrtEn      = 1 << 6,
        dSOCen       = 1 << 7,
        ThmHotAlrtEn = 1 << 8,
        ThmHotEn     = 1 << 9,
        FCThmHot     = 1 << 10,
        SPR          = 1 << 11
    };

} // namespace bsp::battery_charger
