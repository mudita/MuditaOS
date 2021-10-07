/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

extern "C"
{
#include "fsl_common.h"
}

/********************************************
 * Register List
 *******************************************/
#define AW8898_REG_ID           0x00
#define AW8898_REG_SYSST        0x01
#define AW8898_REG_SYSINT       0x02
#define AW8898_REG_SYSINTM      0x03
#define AW8898_REG_SYSCTRL      0x04
#define AW8898_REG_I2SCTRL      0x05
#define AW8898_REG_I2STXCFG     0x06
#define AW8898_REG_PWMCTRL      0x08
#define AW8898_REG_HAGCCFG1     0x09
#define AW8898_REG_HAGCCFG2     0x0A
#define AW8898_REG_HAGCCFG3     0x0B
#define AW8898_REG_HAGCCFG4     0x0C
#define AW8898_REG_HAGCCFG5     0x0D
#define AW8898_REG_HAGCCFG6     0x0E
#define AW8898_REG_HAGCCFG7     0x0F
#define AW8898_REG_HAGCST       0x10
#define AW8898_REG_DBGCTRL      0x20
#define AW8898_REG_I2SCFG       0x21
#define AW8898_REG_I2SSTAT      0x22
#define AW8898_REG_I2SCAPCNT    0x23
#define AW8898_REG_GENCTRL      0x60
#define AW8898_REG_BSTCTRL1     0x61
#define AW8898_REG_BSTCTRL2     0x62
#define AW8898_REG_PLLCTRL1     0x63
#define AW8898_REG_PLLCTRL2     0x64
#define AW8898_REG_TESTCTRL     0x65
#define AW8898_REG_AMPDBG1      0x66
#define AW8898_REG_AMPDBG2      0x67
#define AW8898_REG_BSTDBG1      0x68
#define AW8898_REG_CDACTRL1     0x69
#define AW8898_REG_CDACTRL2     0x6A
#define AW8898_REG_TESTCTRL2    0x6B

#define AW8898_REG_MAX          0x6F

/********************************************
 * Register Access
 *******************************************/
#define REG_NONE_ACCESS 0
#define REG_RD_ACCESS  1 << 0
#define REG_WR_ACCESS  1 << 1

/******************************************************
 * Register Detail
 *****************************************************/
// SYSST
#define AW8898_BIT_SYSST_UVLOS                      ( 1<<14)
#define AW8898_BIT_SYSST_ADPS                       ( 1<<13)
#define AW8898_BIT_SYSST_DSPS                       ( 1<<12)
#define AW8898_BIT_SYSST_BSTOCS                     ( 1<<11)
#define AW8898_BIT_SYSST_OVPS                       ( 1<<10)
#define AW8898_BIT_SYSST_BSTS                       ( 1<< 9)
#define AW8898_BIT_SYSST_SWS                        ( 1<< 8)
#define AW8898_BIT_SYSST_CLIPS                      ( 1<< 7)
#define AW8898_BIT_SYSST_WDS                        ( 1<< 6)
#define AW8898_BIT_SYSST_NOCLKS                     ( 1<< 5)
#define AW8898_BIT_SYSST_CLKS                       ( 1<< 4)
#define AW8898_BIT_SYSST_OCDS                       ( 1<< 3)
#define AW8898_BIT_SYSST_OTLS                       ( 1<< 2)
#define AW8898_BIT_SYSST_OTHS                       ( 1<< 1)
#define AW8898_BIT_SYSST_PLLS                       ( 1<< 0)

// SYSINT
#define AW8898_BIT_SYSINT_UVLOI                     ( 1<<14)
#define AW8898_BIT_SYSINT_ADPI                      ( 1<<13)
#define AW8898_BIT_SYSINT_DSPI                      ( 1<<12)
#define AW8898_BIT_SYSINT_BSTOCI                    ( 1<<11)
#define AW8898_BIT_SYSINT_OVPI                      ( 1<<10)
#define AW8898_BIT_SYSINT_BSTI                      ( 1<< 9)
#define AW8898_BIT_SYSINT_SWI                       ( 1<< 8)
#define AW8898_BIT_SYSINT_CLIPI                     ( 1<< 7)
#define AW8898_BIT_SYSINT_WDI                       ( 1<< 6)
#define AW8898_BIT_SYSINT_NOCLKI                    ( 1<< 5)
#define AW8898_BIT_SYSINT_CLKI                      ( 1<< 4)
#define AW8898_BIT_SYSINT_OCDI                      ( 1<< 3)
#define AW8898_BIT_SYSINT_OTLI                      ( 1<< 2)
#define AW8898_BIT_SYSINT_OTHI                      ( 1<< 1)
#define AW8898_BIT_SYSINT_PLLI                      ( 1<< 0)

// SYSINTM
#define AW8898_BIT_SYSINTM_UVLOM                    ( 1<<14)
#define AW8898_BIT_SYSINTM_ADPM                     ( 1<<13)
#define AW8898_BIT_SYSINTM_DSPM                     ( 1<<12)
#define AW8898_BIT_SYSINTM_BSTOCM                   ( 1<<11)
#define AW8898_BIT_SYSINTM_OVPM                     ( 1<<10)
#define AW8898_BIT_SYSINTM_BSTM                     ( 1<< 9)
#define AW8898_BIT_SYSINTM_SWM                      ( 1<< 8)
#define AW8898_BIT_SYSINTM_CLIPM                    ( 1<< 7)
#define AW8898_BIT_SYSINTM_WDM                      ( 1<< 6)
#define AW8898_BIT_SYSINTM_NOCLKM                   ( 1<< 5)
#define AW8898_BIT_SYSINTM_CLKM                     ( 1<< 4)
#define AW8898_BIT_SYSINTM_OCDM                     ( 1<< 3)
#define AW8898_BIT_SYSINTM_OTLM                     ( 1<< 2)
#define AW8898_BIT_SYSINTM_OTHM                     ( 1<< 1)
#define AW8898_BIT_SYSINTM_PLLM                     ( 1<< 0)

// SYSCTRL
#define AW8898_BIT_SYSCTRL_INTMODE_MASK             (~( 3<< 8))
#define AW8898_BIT_SYSCTRL_INT_HIGH_PP              ( 3<< 8)
#define AW8898_BIT_SYSCTRL_INT_LOW_PP               ( 2<< 8)
#define AW8898_BIT_SYSCTRL_INT_HIGH_OD              ( 1<< 8)
#define AW8898_BIT_SYSCTRL_INT_LOW_OD               ( 0<< 8)
#define AW8898_BIT_SYSCTRL_MODE_MASK                (~( 1<< 7))
#define AW8898_BIT_SYSCTRL_RCV_MODE                 ( 1<< 7)
#define AW8898_BIT_SYSCTRL_SPK_MODE                 ( 0<< 7)
#define AW8898_BIT_SYSCTRL_I2SEN_MASK               (~( 1<< 6))
#define AW8898_BIT_SYSCTRL_I2S_ENABLE               ( 1<< 6)
#define AW8898_BIT_SYSCTRL_I2S_DISABLE              ( 0<< 6)
#define AW8898_BIT_SYSCTRL_WSINV_MASK               (~( 1<< 5))
#define AW8898_BIT_SYSCTRL_WS_INVERT                ( 1<< 5)
#define AW8898_BIT_SYSCTRL_WS_NO_INVERT             ( 0<< 5)
#define AW8898_BIT_SYSCTRL_BCKINV_MASK              (~( 1<< 4))
#define AW8898_BIT_SYSCTRL_BCK_INVERT               ( 1<< 4)
#define AW8898_BIT_SYSCTRL_BCK_NO_INVERT            ( 0<< 4)
#define AW8898_BIT_SYSCTRL_IPLL_MASK                (~( 1<< 3))
#define AW8898_BIT_SYSCTRL_PLL_WORD                 ( 1<< 3)
#define AW8898_BIT_SYSCTRL_PLL_BIT                  ( 0<< 3)
#define AW8898_BIT_SYSCTRL_DSPBY_MASK               (~( 1<< 2))
#define AW8898_BIT_SYSCTRL_DSP_BYPASS               ( 1<< 2)
#define AW8898_BIT_SYSCTRL_DSP_WORK                 ( 0<< 2)
#define AW8898_BIT_SYSCTRL_CP_MASK                  (~( 1<< 1))
#define AW8898_BIT_SYSCTRL_CP_PDN                   ( 1<< 1)
#define AW8898_BIT_SYSCTRL_CP_ACTIVE                ( 0<< 1)
#define AW8898_BIT_SYSCTRL_PW_MASK                  (~( 1<< 0))
#define AW8898_BIT_SYSCTRL_PW_PDN                   ( 1<< 0)
#define AW8898_BIT_SYSCTRL_PW_ACTIVE                ( 0<< 0)

// I2SCTRL
#define AW8898_BIT_I2SCTRL_INPLEV_MASK              (~( 1<<13))
#define AW8898_BIT_I2SCTRL_INPLEV_0DB               ( 1<<13)
#define AW8898_BIT_I2SCTRL_INPLEV_NEG_6DB           ( 0<<13)
#define AW8898_BIT_I2SCTRL_STEREO_MASK              (~( 1<<12))
#define AW8898_BIT_I2SCTRL_STEREO_ENABLE            ( 1<<12)
#define AW8898_BIT_I2SCTRL_STEREO_DISABLE           ( 0<<12)
#define AW8898_BIT_I2SCTRL_CHS_MASK                 (~( 3<<10))
#define AW8898_BIT_I2SCTRL_CHS_MONO                 ( 3<<10)
#define AW8898_BIT_I2SCTRL_CHS_RIGHT                ( 2<<10)
#define AW8898_BIT_I2SCTRL_CHS_LEFT                 ( 1<<10)
#define AW8898_BIT_I2SCTRL_MD_MASK                  (~( 3<< 8))
#define AW8898_BIT_I2SCTRL_MD_LSB                   ( 2<< 8)
#define AW8898_BIT_I2SCTRL_MD_MSB                   ( 1<< 8)
#define AW8898_BIT_I2SCTRL_MD_STD                   ( 0<< 8)
#define AW8898_BIT_I2SCTRL_FMS_MASK                 (~( 3<< 6))
#define AW8898_BIT_I2SCTRL_FMS_32BIT                ( 3<< 6)
#define AW8898_BIT_I2SCTRL_FMS_24BIT                ( 2<< 6)
#define AW8898_BIT_I2SCTRL_FMS_20BIT                ( 1<< 6)
#define AW8898_BIT_I2SCTRL_FMS_16BIT                ( 0<< 6)
#define AW8898_BIT_I2SCTRL_BCK_MASK                 (~( 3<< 4))
#define AW8898_BIT_I2SCTRL_BCK_64FS                 ( 2<< 4)
#define AW8898_BIT_I2SCTRL_BCK_48FS                 ( 1<< 4)
#define AW8898_BIT_I2SCTRL_BCK_32FS                 ( 0<< 4)
#define AW8898_BIT_I2SCTRL_SR_MASK                  (~(15<< 0))
#define AW8898_BIT_I2SCTRL_SR_192K                  (10<< 0)
#define AW8898_BIT_I2SCTRL_SR_96K                   ( 9<< 0)
#define AW8898_BIT_I2SCTRL_SR_48K                   ( 8<< 0)
#define AW8898_BIT_I2SCTRL_SR_44P1K                 ( 7<< 0)
#define AW8898_BIT_I2SCTRL_SR_32K                   ( 6<< 0)
#define AW8898_BIT_I2SCTRL_SR_24K                   ( 5<< 0)
#define AW8898_BIT_I2SCTRL_SR_22K                   ( 4<< 0)
#define AW8898_BIT_I2SCTRL_SR_16K                   ( 3<< 0)
#define AW8898_BIT_I2SCTRL_SR_12K                   ( 2<< 0)
#define AW8898_BIT_I2SCTRL_SR_11K                   ( 1<< 0)
#define AW8898_BIT_I2SCTRL_SR_8K                    ( 0<< 0)


// I2STXCFG
#define AW8898_BIT_I2STXCFG_FSYNC_MASK              (~( 1<<15))
#define AW8898_BIT_I2STXCFG_FSYNC_BCK_CYCLE         ( 1<<15)
#define AW8898_BIT_I2STXCFG_FSYNC_ONE_SLOT          ( 0<<15)
#define AW8898_BIT_I2STXCFG_SLOT_NUM_MASK           (~( 1<<14))
#define AW8898_BIT_I2STXCFG_SLOT_NUM_4_TIMES        ( 1<<14)
#define AW8898_BIT_I2STXCFG_SLOT_NUM_2_TIMES        ( 0<<14)
#define AW8898_BIT_I2STXCFG_TX_SLOT_VLD_MASK        (~(15<<12))
#define AW8898_BIT_I2STXCFG_TX_SLOT_VLD_3           ( 3<<12)
#define AW8898_BIT_I2STXCFG_TX_SLOT_VLD_2           ( 2<<12)
#define AW8898_BIT_I2STXCFG_TX_SLOT_VLD_1           ( 1<<12)
#define AW8898_BIT_I2STXCFG_TX_SLOT_VLD_0           ( 0<<12)
#define AW8898_BIT_I2STXCFG_RX_SLOT_VLD_MASK        (~(15<< 8))
#define AW8898_BIT_I2STXCFG_RX_SLOT_VLD_3_2         (12<< 8)
#define AW8898_BIT_I2STXCFG_RX_SLOT_VLD_3_1         (10<< 8)
#define AW8898_BIT_I2STXCFG_RX_SLOT_VLD_3_0         ( 9<< 8)
#define AW8898_BIT_I2STXCFG_RX_SLOT_VLD_2_1         ( 6<< 8)
#define AW8898_BIT_I2STXCFG_RX_SLOT_VLD_2_0         ( 5<< 8)
#define AW8898_BIT_I2STXCFG_RX_SLOT_VLD_1_0         ( 3<< 8)
#define AW8898_BIT_I2STXCFG_RX_SLOT_VLD_3           ( 8<< 8)
#define AW8898_BIT_I2STXCFG_RX_SLOT_VLD_2           ( 4<< 8)
#define AW8898_BIT_I2STXCFG_RX_SLOT_VLD_1           ( 2<< 8)
#define AW8898_BIT_I2STXCFG_RX_SLOT_VLD_0           ( 1<< 8)
#define AW8898_BIT_I2STXCFG_DRVSTREN_MASK           (~( 1<< 5))
#define AW8898_BIT_I2STXCFG_DRVSTREN_8MA            ( 1<< 5)
#define AW8898_BIT_I2STXCFG_DRVSTREN_2MA            ( 0<< 5)
#define AW8898_BIT_I2STXCFG_DOHZ_MASK               (~( 1<< 4))
#define AW8898_BIT_I2STXCFG_DOHZ_HIZ                ( 1<< 4)
#define AW8898_BIT_I2STXCFG_DOHZ_GND                ( 0<< 4)
#define AW8898_BIT_I2STXCFG_DSEL_MASK               (~( 3<< 2))
#define AW8898_BIT_I2STXCFG_DSEL_DSP                ( 2<< 2)
#define AW8898_BIT_I2STXCFG_DSEL_GAIN               ( 1<< 2)
#define AW8898_BIT_I2STXCFG_DSEL_ZERO               ( 0<< 2)
#define AW8898_BIT_I2STXCFG_CHS_MASK                (~( 1<< 1))
#define AW8898_BIT_I2STXCFG_CHS_RIGHT               ( 1<< 1)
#define AW8898_BIT_I2STXCFG_CHS_LEFT                ( 0<< 1)
#define AW8898_BIT_I2STXCFG_TX_MASK                 (~( 1<< 0))
#define AW8898_BIT_I2STXCFG_TX_ENABLE               ( 1<< 0)
#define AW8898_BIT_I2STXCFG_TX_DISABLE              ( 0<< 0)

// PWMCTRL
#define AW8898_BIT_PWMCTRL_DSMZTH_MASK              (~(15<<12))
#define AW8898_BIT_PWMCTRL_DSMZTH_UNIT              ( 1<<12)
#define AW8898_BIT_PWMCTRL_PWMDELA_MASK             (~(15<< 8))
#define AW8898_BIT_PWMCTRL_PWMDELA_UNIT             ( 1<< 8)
#define AW8898_BIT_PWMCTRL_PWMDELB_MASK             (~(15<< 4))
#define AW8898_BIT_PWMCTRL_PWMDELB_UNIT             ( 1<< 4)
#define AW8898_BIT_PWMCTRL_PWMSH_MASK               (~( 1<< 3))
#define AW8898_BIT_PWMCTRL_PWMSH_TRIANGLE           ( 1<< 3)
#define AW8898_BIT_PWMCTRL_PWMSH_SAWTOOTH           ( 0<< 3)
#define AW8898_BIT_PWMCTRL_PWMRES_MASK              (~( 1<< 2))
#define AW8898_BIT_PWMCTRL_PWMRES_8BIT              ( 1<< 2)
#define AW8898_BIT_PWMCTRL_PWMRES_7BIT              ( 0<< 2)
#define AW8898_BIT_PWMCTRL_HDCCE_MASK               (~( 1<< 1))
#define AW8898_BIT_PWMCTRL_HDCCE_ENABLE             ( 1<< 1)
#define AW8898_BIT_PWMCTRL_HDCCE_DISABLE            ( 0<< 1)
#define AW8898_BIT_PWMCTRL_HMUTE_MASK               (~( 1<< 0))
#define AW8898_BIT_PWMCTRL_HMUTE_ENABLE             ( 1<< 0)
#define AW8898_BIT_PWMCTRL_HMUTE_DISABLE            ( 0<< 0)

// HAGCCFG1
#define AW8898_BIT_HAGCCFG1_RVTH_MASK               (~(255<<8))
#define AW8898_BIT_HAGCCFG1_RVTH_UNIT               ( 1<< 8)
#define AW8898_BIT_HAGCCFG1_AVTH_MASK               (~(255<<0))
#define AW8898_BIT_HAGCCFG1_AVTH_UNIT               ( 1<< 0)

// HAGCCFG2
#define AW8898_BIT_HAGCCFG2_ATTH_UNIT               ( 1<< 0)

// HAGCCFG3
#define AW8898_BIT_HAGCCFG3_RTTH_UNIT               ( 1<< 0)

// HAGCCFG4
#define AW8898_BIT_HAGCCFG4_MPD_MASK                (~( 1<<14))
#define AW8898_BIT_HAGCCFG4_MPD_ENABLE              ( 1<<14)
#define AW8898_BIT_HAGCCFG4_MPD_DISABLE             ( 0<<14)
#define AW8898_BIT_HAGCCFG4_MPD_TTH_MASK            (~( 3<<12))
#define AW8898_BIT_HAGCCFG4_MPD_TTH_0P047           ( 3<<12)
#define AW8898_BIT_HAGCCFG4_MPD_TTH_0P032           ( 2<<12)
#define AW8898_BIT_HAGCCFG4_MPD_TTH_0P016           ( 1<<12)
#define AW8898_BIT_HAGCCFG4_MPD_TTH_0P008           ( 0<<12)
#define AW8898_BIT_HAGCCFG4_MPD_RTH_MASK            (~( 3<<10))
#define AW8898_BIT_HAGCCFG4_MPD_RTH_0P047           ( 3<<10)
#define AW8898_BIT_HAGCCFG4_MPD_RTH_0P032           ( 2<<10)
#define AW8898_BIT_HAGCCFG4_MPD_RTH_0P016           ( 1<<10)
#define AW8898_BIT_HAGCCFG4_MPD_RTH_0P008           ( 0<<10)
#define AW8898_BIT_HAGCCFG4_MPD_ATH_MASK            (~( 3<< 8))
#define AW8898_BIT_HAGCCFG4_MPD_ATH_0P047           ( 3<< 8)
#define AW8898_BIT_HAGCCFG4_MPD_ATH_0P032           ( 2<< 8)
#define AW8898_BIT_HAGCCFG4_MPD_ATH_0P016           ( 1<< 8)
#define AW8898_BIT_HAGCCFG4_MPD_ATH_0P008           ( 0<< 8)
#define AW8898_BIT_HAGCCFG4_HOLDTH_MASK             (~(255<< 0))

// HAGCCFG5

// HAGCCFG6

// HAGCCFG7
#define AW8898_BIT_HAGCCFG7_VOL_MASK                (~(255< 8))
#define AW8898_VOLUME_MAX                           (0)
#define AW8898_VOLUME_MIN                           (-255)
#define AW8898_VOL_REG_SHIFT                        (8)

// HAGCST
#define AW8898_BIT_BSTVOUT_ST_10P25V                (15<< 0)
#define AW8898_BIT_BSTVOUT_ST_10V                   (14<< 0)
#define AW8898_BIT_BSTVOUT_ST_9P75V                 (13<< 0)
#define AW8898_BIT_BSTVOUT_ST_9P5V                  (12<< 0)
#define AW8898_BIT_BSTVOUT_ST_9P25V                 (11<< 0)
#define AW8898_BIT_BSTVOUT_ST_9V                    (10<< 0)
#define AW8898_BIT_BSTVOUT_ST_8P75V                 ( 9<< 0)
#define AW8898_BIT_BSTVOUT_ST_8P5V                  ( 8<< 0)
#define AW8898_BIT_BSTVOUT_ST_8P25V                 ( 7<< 0)
#define AW8898_BIT_BSTVOUT_ST_8V                    ( 6<< 0)
#define AW8898_BIT_BSTVOUT_ST_7P75V                 ( 5<< 0)
#define AW8898_BIT_BSTVOUT_ST_7P5V                  ( 4<< 0)
#define AW8898_BIT_BSTVOUT_ST_7P25V                 ( 3<< 0)
#define AW8898_BIT_BSTVOUT_ST_7V                    ( 2<< 0)
#define AW8898_BIT_BSTVOUT_ST_6P75V                 ( 1<< 0)
#define AW8898_BIT_BSTVOUT_ST_6P5V                  ( 0<< 0)

// DBGCTRL
#define AW8898_BIT_DBGCTRL_LPBK_FAR_MASK            (~( 1<<15))
#define AW8898_BIT_DBGCTRL_LPBK_FAR_ENABLE          ( 1<<15)
#define AW8898_BIT_DBGCTRL_LPBK_FAR_DISABLE         ( 0<<15)
#define AW8898_BIT_DBGCTRL_LPBK_NEAR_MASK           (~( 1<<14))
#define AW8898_BIT_DBGCTRL_LPBK_NEAR_ENABLE         ( 1<<14)
#define AW8898_BIT_DBGCTRL_LPBK_NEAR_DISABLE        ( 0<<14)
#define AW8898_BIT_DBGCTRL_PDUVL_MASK               (~( 1<<13))
#define AW8898_BIT_DBGCTRL_PDUVL_DISABLE            ( 1<<13)
#define AW8898_BIT_DBGCTRL_PDUVL_ENABLE             ( 0<<13)
#define AW8898_BIT_DBGCTRL_MUTE_MASK                (~( 1<<12))
#define AW8898_BIT_DBGCTRL_MUTE_NO_AUTO             ( 1<<12)
#define AW8898_BIT_DBGCTRL_MUTE_AUTO                ( 0<<12)
#define AW8898_BIT_DBGCTRL_NOCLK_RESET_MASK         (~( 1<<11))
#define AW8898_BIT_DBGCTRL_NOCLK_NO_RESET           ( 1<<11)
#define AW8898_BIT_DBGCTRL_NOCLK_RESET              ( 0<<11)
#define AW8898_BIT_DBGCTRL_PLL_UNLOCK_RESET_MASK    (~( 1<<10))
#define AW8898_BIT_DBGCTRL_PLL_UNLOCK_NO_RESET      ( 1<<10)
#define AW8898_BIT_DBGCTRL_PLL_UNLOCK_RESET         ( 0<<10)
#define AW8898_BIT_DBGCTRL_CLKMD_MASK               (~( 1<< 9))
#define AW8898_BIT_DBGCTRL_CLKMD_HALF               ( 1<< 9)
#define AW8898_BIT_DBGCTRL_CLKMD_NORMAL             ( 0<< 9)
#define AW8898_BIT_DBGCTRL_OSCPD_MASK               (~( 1<< 8))
#define AW8898_BIT_DBGCTRL_OSCPD_ENABLE             ( 1<< 8)
#define AW8898_BIT_DBGCTRL_OSCPD_DISABLE            ( 0<< 8)
#define AW8898_BIT_DBGCTRL_AMPPD_MASK               (~( 1<< 7))
#define AW8898_BIT_DBGCTRL_AMPPD_PDN                ( 1<< 7)
#define AW8898_BIT_DBGCTRL_AMPPD_ACTIVE             ( 0<< 7)
#define AW8898_BIT_DBGCTRL_PLLPD_MASK               (~( 1<< 6))
#define AW8898_BIT_DBGCTRL_PLLPD_PDN                ( 1<< 6)
#define AW8898_BIT_DBGCTRL_PLLPD_ACTIVE             ( 0<< 6)
#define AW8898_BIT_DBGCTRL_I2SRST_MASK              (~( 1<< 5))
#define AW8898_BIT_DBGCTRL_I2SRST_RESET             ( 1<< 5)
#define AW8898_BIT_DBGCTRL_I2SRST_WORK              ( 0<< 5)
#define AW8898_BIT_DBGCTRL_SYSRST_MASK              (~( 1<< 4))
#define AW8898_BIT_DBGCTRL_SYSRST_RESET             ( 1<< 4)
#define AW8898_BIT_DBGCTRL_SYSRST_WORK              ( 0<< 4)
#define AW8898_BIT_DBGCTRL_SYSCE_MASK               (~( 1<< 0))
#define AW8898_BIT_DBGCTRL_SYSCE_ENABLE             ( 1<< 0)
#define AW8898_BIT_DBGCTRL_SYSCE_DISABLE            ( 0<< 0)


// I2SCFG
#define AW8898_BIT_I2SCFG_I2SRX_MASK                (~( 1<< 0))
#define AW8898_BIT_I2SCFG_I2SRX_ENABLE              ( 1<< 0)
#define AW8898_BIT_I2SCFG_I2SRX_DISABLE             ( 0<< 0)

// I2SSAT
#define AW8898_BIT_I2SSAT_DPSTAT                    ( 1<< 2)
#define AW8898_BIT_I2SSAT_I2SROVS                   ( 1<< 1)
#define AW8898_BIT_I2SSAT_I2STOVS                   ( 1<< 0)

// GENCTRL
#define AW8898_BIT_GENCTRL_BURST_PEAK_MASK          (~( 3<<14))
#define AW8898_BIT_GENCTRL_BURST_PEAK_200MA         ( 3<<14)
#define AW8898_BIT_GENCTRL_BURST_PEAK_160MA         ( 2<<14)
#define AW8898_BIT_GENCTRL_BURST_PEAK_100MA         ( 1<<14)
#define AW8898_BIT_GENCTRL_BURST_PEAK_130MA         ( 0<<14)
#define AW8898_BIT_GENCTRL_BST_TDEG2_MASK           (~( 7<< 9))
#define AW8898_BIT_GENCTRL_BST_TDEG2_2P7S           ( 7<< 9)
#define AW8898_BIT_GENCTRL_BST_TDEG2_1P3S           ( 6<< 9)
#define AW8898_BIT_GENCTRL_BST_TDEG2_672MS          ( 5<< 9)
#define AW8898_BIT_GENCTRL_BST_TDEG2_336MS          ( 4<< 9)
#define AW8898_BIT_GENCTRL_BST_TDEG2_168MS          ( 3<< 9)
#define AW8898_BIT_GENCTRL_BST_TDEG2_84MS           ( 2<< 9)
#define AW8898_BIT_GENCTRL_BST_TDEG2_42MS           ( 1<< 9)
#define AW8898_BIT_GENCTRL_BST_TDEG2_21MS           ( 0<< 9)
#define AW8898_BIT_GENCTRL_BST_OCAP_MASK            (~( 1<< 8))
#define AW8898_BIT_GENCTRL_BST_OCAP_SLOW            ( 1<< 8)
#define AW8898_BIT_GENCTRL_BST_OCAP_FAST            ( 0<< 8)
#define AW8898_BIT_GENCTRL_BST_EN_MASK              (~( 1<< 7))
#define AW8898_BIT_GENCTRL_BST_ENABLE               ( 1<< 7)
#define AW8898_BIT_GENCTRL_BST_DISABLE              ( 0<< 7)
#define AW8898_BIT_GENCTRL_BST_ILIMIT_MASK          (~( 7<< 4))
#define AW8898_BIT_GENCTRL_BST_ILIMIT_4P5A          ( 7<< 4)
#define AW8898_BIT_GENCTRL_BST_ILIMIT_4P25A         ( 6<< 4)
#define AW8898_BIT_GENCTRL_BST_ILIMIT_4A            ( 5<< 4)
#define AW8898_BIT_GENCTRL_BST_ILIMIT_3P75A         ( 4<< 4)
#define AW8898_BIT_GENCTRL_BST_ILIMIT_3P5A          ( 3<< 4)
#define AW8898_BIT_GENCTRL_BST_ILIMIT_3P25A         ( 2<< 4)
#define AW8898_BIT_GENCTRL_BST_ILIMIT_3A            ( 1<< 4)
#define AW8898_BIT_GENCTRL_BST_ILIMIT_2P75A         ( 0<< 4)
#define AW8898_BIT_GENCTRL_BST_VOUT_MASK            (~(15<< 0))
#define AW8898_BIT_GENCTRL_BST_VOUT_10P25V          (15<< 0)
#define AW8898_BIT_GENCTRL_BST_VOUT_10V             (14<< 0)
#define AW8898_BIT_GENCTRL_BST_VOUT_9P75V           (13<< 0)
#define AW8898_BIT_GENCTRL_BST_VOUT_9P5V            (12<< 0)
#define AW8898_BIT_GENCTRL_BST_VOUT_9P25V           (11<< 0)
#define AW8898_BIT_GENCTRL_BST_VOUT_9V              (10<< 0)
#define AW8898_BIT_GENCTRL_BST_VOUT_8P75V           ( 9<< 0)
#define AW8898_BIT_GENCTRL_BST_VOUT_8P5V            ( 8<< 0)
#define AW8898_BIT_GENCTRL_BST_VOUT_8P25V           ( 7<< 0)
#define AW8898_BIT_GENCTRL_BST_VOUT_8V              ( 6<< 0)
#define AW8898_BIT_GENCTRL_BST_VOUT_7P75V           ( 5<< 0)
#define AW8898_BIT_GENCTRL_BST_VOUT_7P5V            ( 4<< 0)
#define AW8898_BIT_GENCTRL_BST_VOUT_7P25V           ( 3<< 0)
#define AW8898_BIT_GENCTRL_BST_VOUT_7V              ( 2<< 0)
#define AW8898_BIT_GENCTRL_BST_VOUT_6P75V           ( 1<< 0)
#define AW8898_BIT_GENCTRL_BST_VOUT_6P5V            ( 0<< 0)

// BSTCTRL1
#define AW8898_BIT_BSTCTRL1_RTH_MASK                (~(64<< 8))
#define AW8898_BIT_BSTCTRL1_ATH_MASK                (~(64<< 0))

// BSTCTRL2
#define AW8898_BIT_BST_MODE_MASK                    (~( 7<< 3))
#define AW8898_BIT_BST_MODE_SMART_BOOST             ( 6<< 3)
#define AW8898_BIT_BST_MODE_ADAPT_BOOST             ( 5<< 3)
#define AW8898_BIT_BST_MODE_FORCE_BOOST             ( 1<< 3)
#define AW8898_BIT_BST_MODE_TRANSP_BOOST            ( 0<< 3)
#define AW8898_BIT_BST_TDEG_MASK                    (~( 7<< 0))
#define AW8898_BIT_BST_TDEG_2P7S                    ( 7<< 0)
#define AW8898_BIT_BST_TDEG_1P3S                    ( 6<< 0)
#define AW8898_BIT_BST_TDEG_672MS                   ( 5<< 0)
#define AW8898_BIT_BST_TDEG_336MS                   ( 4<< 0)
#define AW8898_BIT_BST_TDEG_168MS                   ( 3<< 0)
#define AW8898_BIT_BST_TDEG_84MS                    ( 2<< 0)
#define AW8898_BIT_BST_TDEG_42MS                    ( 1<< 0)
#define AW8898_BIT_BST_TDEG_21MS                    ( 0<< 0)

/*! @brief Define the register address of AW8898. */
#define AW8898_LINVOL  0x0U
#define AW8898_RINVOL  0x1U
#define AW8898_LOUT1   0x2U
#define AW8898_ROUT1   0x3U
#define AW8898_CLOCK1  0x4U
#define AW8898_DACCTL1 0x5U
#define AW8898_DACCTL2 0x6U
#define AW8898_IFACE1  0x7U
#define AW8898_CLOCK2  0x8U
#define AW8898_IFACE2  0x9U
#define AW8898_LDAC    0xaU
#define AW8898_RDAC    0xbU

#define AW8898_RESET   0xfU
#define AW8898_3D      0x10U
#define AW8898_ALC1    0x11U
#define AW8898_ALC2    0x12U
#define AW8898_ALC3    0x13U
#define AW8898_NOISEG  0x14U
#define AW8898_LADC    0x15U
#define AW8898_RADC    0x16U
#define AW8898_ADDCTL1 0x17U
#define AW8898_ADDCTL2 0x18U
#define AW8898_POWER1  0x19U
#define AW8898_POWER2  0x1aU
#define AW8898_ADDCTL3 0x1bU
#define AW8898_APOP1   0x1cU
#define AW8898_APOP2   0x1dU

#define AW8898_LINPATH 0x20U
#define AW8898_RINPATH 0x21U
#define AW8898_LOUTMIX 0x22U

#define AW8898_ROUTMIX  0x25U
#define AW8898_MONOMIX1 0x26U
#define AW8898_MONOMIX2 0x27U
#define AW8898_LOUT2    0x28U
#define AW8898_ROUT2    0x29U
#define AW8898_MONO     0x2aU
#define AW8898_INBMIX1  0x2bU
#define AW8898_INBMIX2  0x2cU
#define AW8898_BYPASS1  0x2dU
#define AW8898_BYPASS2  0x2eU
#define AW8898_POWER3   0x2fU
#define AW8898_ADDCTL4  0x30U
#define AW8898_CLASSD1  0x31U

#define AW8898_CLASSD3 0x33U
#define AW8898_PLL1    0x34U
#define AW8898_PLL2    0x35U
#define AW8898_PLL3    0x36U
#define AW8898_PLL4    0x37U

/*! @brief Cache register number */
#define AW8898_CACHEREGNUM 56U

/*! @brief AW8898_IFACE1 FORMAT bits */
#define AW8898_IFACE1_FORMAT_MASK  0x03U
#define AW8898_IFACE1_FORMAT_SHIFT 0x00U
#define AW8898_IFACE1_FORMAT_RJ    0x00U
#define AW8898_IFACE1_FORMAT_LJ    0x01U
#define AW8898_IFACE1_FORMAT_I2S   0x02U
#define AW8898_IFACE1_FORMAT_DSP   0x03U
#define AW8898_IFACE1_FORMAT(x)    (((x) << AW8898_IFACE1_FORMAT_SHIFT) & AW8898_IFACE1_FORMAT_MASK)

/*! @brief AW8898_IFACE1 WL bits */
#define AW8898_IFACE1_WL_MASK   0x0CU
#define AW8898_IFACE1_WL_SHIFT  0x02U
#define AW8898_IFACE1_WL_16BITS 0x00U
#define AW8898_IFACE1_WL_20BITS 0x01U
#define AW8898_IFACE1_WL_24BITS 0x02U
#define AW8898_IFACE1_WL_32BITS 0x03U
#define AW8898_IFACE1_WL(x)     (((x) << AW8898_IFACE1_WL_SHIFT) & AW8898_IFACE1_WL_MASK)

/*! @brief AW8898_IFACE1 LRP bit */
#define AW8898_IFACE1_LRP_MASK         0x10U
#define AW8898_IFACE1_LRP_SHIFT        0x04U
#define AW8898_IFACE1_LRCLK_NORMAL_POL 0x00U
#define AW8898_IFACE1_LRCLK_INVERT_POL 0x01U
#define AW8898_IFACE1_DSP_MODEA        0x00U
#define AW8898_IFACE1_DSP_MODEB        0x01U
#define AW8898_IFACE1_LRP(x)           (((x) << AW8898_IFACE1_LRP_SHIFT) & AW8898_IFACE1_LRP_MASK)

/*! @brief AW8898_IFACE1 DLRSWAP bit */
#define AW8898_IFACE1_DLRSWAP_MASK  0x20U
#define AW8898_IFACE1_DLRSWAP_SHIFT 0x05U
#define AW8898_IFACE1_DACCH_NORMAL  0x00U
#define AW8898_IFACE1_DACCH_SWAP    0x01U
#define AW8898_IFACE1_DLRSWAP(x)    (((x) << AW8898_IFACE1_DLRSWAP_SHIFT) & AW8898_IFACE1_DLRSWAP_MASK)

/*! @brief AW8898_IFACE1 MS bit */
#define AW8898_IFACE1_MS_MASK  0x40U
#define AW8898_IFACE1_MS_SHIFT 0x06U
#define AW8898_IFACE1_SLAVE    0x00U
#define AW8898_IFACE1_MASTER   0x01U
#define AW8898_IFACE1_MS(x)    (((x) << AW8898_IFACE1_MS_SHIFT) & AW8898_IFACE1_MS_MASK)

/*! @brief AW8898_IFACE1 BCLKINV bit */
#define AW8898_IFACE1_BCLKINV_MASK   0x80U
#define AW8898_IFACE1_BCLKINV_SHIFT  0x07U
#define AW8898_IFACE1_BCLK_NONINVERT 0x00U
#define AW8898_IFACE1_BCLK_INVERT    0x01U
#define AW8898_IFACE1_BCLKINV(x)     (((x) << AW8898_IFACE1_BCLKINV_SHIFT) & AW8898_IFACE1_BCLKINV_MASK)

/*! @brief AW8898_IFACE1 ALRSWAP bit */
#define AW8898_IFACE1_ALRSWAP_MASK  0x100U
#define AW8898_IFACE1_ALRSWAP_SHIFT 0x08U
#define AW8898_IFACE1_ADCCH_NORMAL  0x00U
#define AW8898_IFACE1_ADCCH_SWAP    0x01U
#define AW8898_IFACE1_ALRSWAP(x)    (((x) << AW8898_IFACE1_ALRSWAP_SHIFT) & AW8898_IFACE1_ALRSWAP_MASK)

/*! @brief AW8898_POWER1 */
#define AW8898_POWER1_VREF_MASK  0x40U
#define AW8898_POWER1_VREF_SHIFT 0x06U

#define AW8898_POWER1_AINL_MASK  0x20U
#define AW8898_POWER1_AINL_SHIFT 0x05U

#define AW8898_POWER1_AINR_MASK  0x10U
#define AW8898_POWER1_AINR_SHIFT 0x04U

#define AW8898_POWER1_ADCL_MASK  0x08U
#define AW8898_POWER1_ADCL_SHIFT 0x03U

#define AW8898_POWER1_ADCR_MASK  0x0U
#define AW8898_POWER1_ADCR_SHIFT 0x02U

#define AW8898_POWER1_MICB_MASK  0x02U
#define AW8898_POWER1_MICB_SHIFT 0x01U

#define AW8898_POWER1_DIGENB_MASK  0x01U
#define AW8898_POWER1_DIGENB_SHIFT 0x00U

/*! @brief AW8898_POWER2 */
#define AW8898_POWER2_DACL_MASK  0x100U
#define AW8898_POWER2_DACL_SHIFT 0x08U

#define AW8898_POWER2_DACR_MASK  0x80U
#define AW8898_POWER2_DACR_SHIFT 0x07U

#define AW8898_POWER2_LOUT1_MASK  0x40U
#define AW8898_POWER2_LOUT1_SHIFT 0x06U

#define AW8898_POWER2_ROUT1_MASK  0x20U
#define AW8898_POWER2_ROUT1_SHIFT 0x05U

#define AW8898_POWER2_SPKL_MASK  0x10U
#define AW8898_POWER2_SPKL_SHIFT 0x04U

#define AW8898_POWER2_SPKR_MASK  0x08U
#define AW8898_POWER2_SPKR_SHIFT 0x03U

#define AW8898_POWER3_LMIC_MASK   0x20U
#define AW8898_POWER3_LMIC_SHIFT  0x05U
#define AW8898_POWER3_RMIC_MASK   0x10U
#define AW8898_POWER3_RMIC_SHIFT  0x04U
#define AW8898_POWER3_LOMIX_MASK  0x08U
#define AW8898_POWER3_LOMIX_SHIFT 0x03U
#define AW8898_POWER3_ROMIX_MASK  0x04U
#define AW8898_POWER3_ROMIX_SHIFT 0x02U
/*! @brief AW8898 I2C address. */
#define AW8898_I2C_ADDR 0x34
/*! @brief AW8898 I2C baudrate */
#define AW8898_I2C_BAUDRATE (100000U)
/*! @brief AW8898 maximum volume value */
#define AW8898_ADC_MAX_VOLUME_vALUE       0xFFU
#define AW8898_DAC_MAX_VOLUME_vALUE       0xFFU
#define AW8898_HEADPHONE_MAX_VOLUME_vALUE 0xFFU
#define AW8898_HEADPHONE_MIN_VOLUME_vALUE 0x0U
#define AW8898_LINEIN_MAX_VOLUME_vALUE    0x3FU
#define AW8898_SPEAKER_MAX_VOLUME_vALUE   0x7FU
#define AW8898_SPEAKER_MIN_VOLUME_vALUE   0x30U
