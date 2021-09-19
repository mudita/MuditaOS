// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

/*
 * The Clear BSD License
 * Copyright 2017-2018 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 *  that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * How to setup clock using clock driver functions:
 *
 * 1. Call CLOCK_InitXXXPLL() to configure corresponding PLL clock.
 *
 * 2. Call CLOCK_InitXXXpfd() to configure corresponding PLL pfd clock.
 *
 * 3. Call CLOCK_SetMux() to configure corresponding clock source for target clock out.
 *
 * 4. Call CLOCK_SetDiv() to configure corresponding clock divider for target clock out.
 *
 * 5. Call CLOCK_SetXtalFreq() to set XTAL frequency based on board settings.
 *
 */

#include <board/clock_config.h>
#include <board/board.h>
#include "fsl_iomuxc.h"
#include "fsl_dcdc.h"
#include "fsl_gpc.h"
#include <log.hpp>

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
/* System clock frequency. */
extern uint32_t SystemCoreClock;

/*******************************************************************************
 ************************ BOARD_InitBootClocks function ************************
 ******************************************************************************/
void BOARD_InitBootClocks(void)
{
    BOARD_BootClockRUN();
}

/*
 * Default clock tree routing:
 *  No. | Source             | Peripheral      | Frequency
 * ---------------------------------------------------------
 *  1   | OSC_24M            | CSI             | 12MHz
 *      |                    | LPI2C           | 12MHz
 *      |                    | UART            | 24MHz
 * ---------------------------------------------------------
 *  2   | PLL1 (ARM)         | NOT USED        |
 * ---------------------------------------------------------
 *  3   | PLL2 (SYS)         | CORE            | 528MHz
 * ---------------------------------------------------------
 *  4   | PLL2_PFD0          | NOT USED
 * ---------------------------------------------------------
 *  5   | PLL2_PFD1          | NOT USED
 * ---------------------------------------------------------
 *  6   | PLL2_PFD2          | USDHC1          | 163,86MHz (PLL2_PFD2/2)
 *      |                    | USDHC2          | 109,24MHz (PLL2_PFD2/3)
 *      |                    | LPSPI           | 40,9625MHz (PLL2_PFD2/8)
 *      |                    | TRACE           | 117,33MHz
 *      |                    | SEMC            | 163,86MHz (PLL2_PFD2/2)
 * ---------------------------------------------------------
 *  7   | PLL2_PFD3          | NOT USED
 * ---------------------------------------------------------
 *  8   | PLL3_PFD0 (USB1)   | NOT USED        |
 * ---------------------------------------------------------
 *  9   | PLL3               | NOT USED        |
 * ---------------------------------------------------------
 *  10  | PLL3_PFD1          | NOT USED        |
 * ---------------------------------------------------------
 *  11  | PLL3_PFD2          | NOT USED        |
 * ---------------------------------------------------------
 *  12  | PLL4 (audio)       | SAI1            | 12,28125MHz (PLL4/64)
 *      |                    | SAI2            | 12,28125MHz (PLL4/64)
 *      |                    | SAI1_MCLK1      | 12,28125MHz (PLL4/64)
 *      |                    | SAI1_MCLK2      | 12,28125MHz (PLL4/64)
 *      |                    | SAI2_MCLK1      | 12,28125MHz (PLL4/64)
 * ---------------------------------------------------------
 *  13  | PLL5 (Video)       | NOT USED        |
 * ---------------------------------------------------------
 *  14  | PLL6 (ENET)        | NOT USED        |
 * ---------------------------------------------------------
 *  15  | PLL7 (USB2)        | NOT USED        |
 * ---------------------------------------------------------
 */

void BOARD_BootClockRUN(void)
{
    /* Init RTC OSC clock frequency. */
    CLOCK_SetRtcXtalFreq(32768U);
    /* Disable 1MHz clock output. */
    XTALOSC24M->OSC_CONFIG2 &= ~XTALOSC24M_OSC_CONFIG2_ENABLE_1M_MASK;
    /* Set XTAL 24MHz clock frequency. */
    CLOCK_SetXtalFreq(24000000U);
    /* Enable XTAL 24MHz clock source. */
    CLOCK_InitExternalClk(0);
    /* Enable internal RC. */
    CLOCK_InitRcOsc24M();
    /* Switch clock source to external OSC. */
    CLOCK_SwitchOsc(kCLOCK_XtalOsc);
    /* Set Oscillator ready counter value. */
    CCM->CCR = (CCM->CCR & (~CCM_CCR_OSCNT_MASK)) | CCM_CCR_OSCNT(127);
    /* Setting PeriphClk2Mux and PeriphMux to provide stable clock before PLLs are initialed */
    /* OSC_CLK (24M) */
    CLOCK_SetMux(kCLOCK_PeriphClk2Mux,
                 1); // CBCMR (13-12) 0 - pll3_sw_clk, 1 - osc_clk (pll1_ref_clk), 2 - pll2_bypass_clk, 3 - reserved
    /* PERIPH_CLK2_SEL */
    CLOCK_SetMux(kCLOCK_PeriphMux, 1); // CBCDR (25) 0 - pre_periph_clk_sel, 1 - periph_clk2_clk_divided

    /* Set AHB_PODF. */
    CLOCK_SetDiv(kCLOCK_AhbDiv, 0); // CBCDR

    /* Set IPG_PODF. */
    /* AHB_CLK/4 */
    CLOCK_SetDiv(kCLOCK_IpgDiv, 3); // CBCDR

    /* Set ARM_PODF. */
    /* PLL1/2 (864MHz / 2 = 432 MHz) */
    CLOCK_SetDiv(kCLOCK_ArmDiv, 1); // CACRR

    /* Set PERCLK_PODF. */
    /* IPG_CLK_ROOT/2 */
    CLOCK_SetDiv(kCLOCK_PerclkDiv, 0); // CSCMR1
    /* Set per clock source. */
    CLOCK_SetMux(kCLOCK_PerclkMux, 1); // CSCMR1  (6) 0 - ipg_clk_root, 1 - osc_clk - PIT, GPT

    /* Set USDHC1_PODF. */
    CLOCK_SetDiv(kCLOCK_Usdhc1Div, 1); // CSCDR1
    /* Set Usdhc1 clock source. */
    /* PLL2_PFD2/2 = 396MHz/2 = 198MHz */
    CLOCK_SetMux(kCLOCK_Usdhc1Mux, 0); // CSCMR1  (16) 0 - PLL2_PFD2, 1 - PLL2_PFD0

    /* Set USDHC2_PODF. */
    CLOCK_SetDiv(kCLOCK_Usdhc2Div, 2); // CSCDR1
    /* Set Usdhc2 clock source. */
    /* PLL2_PFD2/3 = 396MHz/3 = 132MHz */
    CLOCK_SetMux(kCLOCK_Usdhc2Mux, 0); // CSCMR1  (17) 0 - PLL2_PFD2, 1 - PLL2_PFD0

    /* Set FLEXSPI_PODF. */
    CLOCK_SetDiv(kCLOCK_FlexspiDiv, 0); // CSCMR1
    /* Set Flexspi clock source. */
    CLOCK_SetMux(kCLOCK_FlexspiMux,
                 3); // CSCMR1  (30-29) 0 - semc_clk_reet_pre, 1 - pll3_sw_clk, 2 - PLL2_PFD2, 3 - PLL3_PFD0

    /* Set CSI_PODF. */
    CLOCK_SetDiv(kCLOCK_CsiDiv, 1); // CSCDR3
    /* Set Csi clock source. */
    CLOCK_SetMux(kCLOCK_CsiMux, 0); // CSCDR3  (10-9) 0 - osc_clk, 1 - PLL2_PFD2, 2 - pll3_120M, 3 - PLL3_PFD1

    /* Set LPSPI_PODF. */
    CLOCK_SetDiv(kCLOCK_LpspiDiv, 7); // CBCMR
    /* Set Lpspi clock source. */
    /* PLL2_PFD2/8 = 396MHz / 8 = 49,5MHz */
    CLOCK_SetMux(kCLOCK_LpspiMux, 3); // CBCMR  (5-4) 0 - PLL3_PFD1, 1 - PLL3_PFD0, 2 - PLL2, 3 - PLL2_PFD2

    /* Set TRACE_PODF. */
    CLOCK_SetDiv(kCLOCK_TraceDiv, 2); // CSCDR1
    /* Set Trace clock source. */
    /* PLL2_PFD0/4 disabled*/
    CLOCK_SetMux(kCLOCK_TraceMux, 2); // CBCMR  (15-14) 0 - PLL2, 1 - PLL2_PFD2, 2 - PLL2_PFD0, 3 - PLL2_PFD1

    /* Set SAI1_CLK_PRED. */
    CLOCK_SetDiv(kCLOCK_Sai1PreDiv, 0); // CS1CDR
    /* Set SAI1_CLK_PODF. */
    CLOCK_SetDiv(kCLOCK_Sai1Div, 63); // CS1CDR
    /* Set Sai1 clock source. */
    CLOCK_SetMux(kCLOCK_Sai1Mux, 2); // CSCMR1  (11-10) 0 - PLL3_PFD2, 1 - PLL5, 2 - PLL4, 3 - reserved

    /* Set SAI2_CLK_PRED. */
    CLOCK_SetDiv(kCLOCK_Sai2PreDiv, 0); // CS2CDR
    /* Set SAI2_CLK_PODF. */
    CLOCK_SetDiv(kCLOCK_Sai2Div, 63); // CS2CDR
    /* Set Sai2 clock source. */
    CLOCK_SetMux(kCLOCK_Sai2Mux, 2); // CSCMR1  (13-12) 0 - PLL3_PFD2, 1 - PLL5, 2 - PLL4, 3 - reserved

    /* Set SAI3_CLK_PRED. */
    CLOCK_SetDiv(kCLOCK_Sai3PreDiv, 0); // CS1CDR
    /* Set SAI3_CLK_PODF. */
    CLOCK_SetDiv(kCLOCK_Sai3Div, 63); // CS1CDR
    /* Set Sai3 clock source. */
    CLOCK_SetMux(kCLOCK_Sai3Mux, 2); // CSCMR1  (15-14) 0 - PLL3_PFD2, 1 - PLL5, 2 - PLL4, 3 - reserved

    /* Set LPI2C_CLK_PODF. */
    CLOCK_SetDiv(kCLOCK_Lpi2cDiv, 1); // CSCDR2
    /* Set Lpi2c clock source. */
    /* OSC_CLK (24MHz/2 = 12MHz */
    CLOCK_SetMux(kCLOCK_Lpi2cMux, 1); // CSCDR2  (18) 0 - pll3_60m, 1 - osc_clk

    /* Set CAN_CLK_PODF. */
    CLOCK_SetDiv(kCLOCK_CanDiv, 1); // CSCMR2
    /* Set Can clock source. */
    CLOCK_SetMux(kCLOCK_CanMux,
                 2); // CSCMR2  (9-8) 0 - pll3_sw_clk (divided clock 60M), 1 - osc_clk, 2 - pll3_sw_clk (divided clock
                     // 80M), 3 - disable

    /* Set UART_CLK_PODF. */
    CLOCK_SetDiv(kCLOCK_UartDiv, 0); // CSCDR1
    /* Set Uart clock source. */
    /* OSC_CLK (24MHz)/1 */
    CLOCK_SetMux(kCLOCK_UartMux, 1); // CSCDR1  (6) 0 - pll3_80m, 1 - osc_clk

    /* Set LCDIF_PRED. */
    CLOCK_SetDiv(kCLOCK_LcdifPreDiv, 1); // CSCDR2
    /* Set LCDIF_CLK_PODF. */
    CLOCK_SetDiv(kCLOCK_LcdifDiv, 3); // CBCMR
    /* Set Lcdif pre clock source. */
    /* PLL3_PFD1 (664.62MHz/4/2 = 83.08MHz disabled */
    CLOCK_SetMux(kCLOCK_LcdifPreMux,
                 5); // CSCDR2  (17-15) 0 - Pll2, 1 - PLL3_PFD3, 2 - PLL5, 3 - PLL2_PFD0, 4 - PLL2_PFD1, 5 - PLL3_PFD1,
                     // 6,7 - reserved

    /* Set SPDIF0_CLK_PRED. */
    CLOCK_SetDiv(kCLOCK_Spdif0PreDiv, 1); // CDCDR
    /* Set SPDIF0_CLK_PODF. */
    CLOCK_SetDiv(kCLOCK_Spdif0Div, 7); // CDCDR
    /* Set Spdif clock source. */
    /* PLL3_main/8/2 = 480MHz / 8 / 2 = 30MHz disabled*/
    CLOCK_SetMux(kCLOCK_SpdifMux, 3); // CDCDR  (21-20) 0 - PLL4, 1 - PLL3_PFD2, 2 - PLL5, 3 - pll3_sw_clk

    /* Set FLEXIO1_CLK_PRED. */
    CLOCK_SetDiv(kCLOCK_Flexio1PreDiv, 1); // CDCDR
    /* Set FLEXIO1_CLK_PODF. */
    CLOCK_SetDiv(kCLOCK_Flexio1Div, 7); // CDCDR
    /* Set Flexio1 clock source. */
    /* PLL3_main/8/2 = 480MHz / 8 / 2 = 30MHz disabled*/
    CLOCK_SetMux(kCLOCK_Flexio1Mux, 3); // CDCDR (8-7) 0 - PLL4, 1 - PlLL3_PFD2, 2 - PLL5, 3 - pll3_sw_clk

    /* Set FLEXIO2_CLK_PRED. */
    CLOCK_SetDiv(kCLOCK_Flexio2PreDiv, 1); // CS1CDR
    /* Set FLEXIO2_CLK_PODF. */
    CLOCK_SetDiv(kCLOCK_Flexio2Div, 7); // CS1CDR
    /* Set Flexio2 clock source. */
    /* PLL3_main/8/2 = 480MHz / 8 / 2 = 30MHz disabled*/
    CLOCK_SetMux(kCLOCK_Flexio2Mux, 3); // CSCMR2 (20-19) 0 - PLL4, 1 - PLL3_PFD2, 2 - PLL5, 3 - pll3_sw_clk

    clkFLEXSPIsetup(CLK_INSTANCE_ALL, CLK_DISABLE);

    /* ADC */
    clkADCsetup(CLK_INSTANCE_ALL, CLK_DISABLE);
    /* XBAR */
    clkXBARsetup(CLK_INSTANCE_ALL, CLK_DISABLE);
    /* GPT */
    clkGPTsetup(CLK_INSTANCE_ALL, CLK_DISABLE);
    /* PIT */
    clkPITsetup(CLK_INSTANCE_ALL, CLK_DISABLE);
    /* USDHC */
    clkUSDHCsetup(CLK_INSTANCE_ALL, CLK_DISABLE);

    /* CSI */
    clkCSIsetup(CLK_INSTANCE_ALL, CLK_DISABLE);
    /* LPSPI */
    clkLPSPICsetup(CLK_INSTANCE_ALL, CLK_DISABLE);
    /* TRACE */
    clkTRACEsetup(CLK_INSTANCE_ALL, CLK_DISABLE);
    /* SAI */
    clkSAIsetup(CLK_INSTANCE_ALL, CLK_DISABLE);
    /* LPI2C */
    clkLPI2Csetup(CLK_INSTANCE_ALL, CLK_DISABLE);
    /* CAN */
    clkCANsetup(CLK_INSTANCE_ALL, CLK_DISABLE);
    /* LPUART */
    clkLPUARTsetup(CLK_INSTANCE_ALL, CLK_DISABLE);
    /* LCDIF */
    clkLCDIFsetup(CLK_INSTANCE_ALL, CLK_DISABLE);
    /* SPDIF */
    clkSPDIFsetup(CLK_INSTANCE_ALL, CLK_DISABLE);
    /* FLEXIO */
    clkFLEXIOsetup(CLK_INSTANCE_ALL, CLK_DISABLE);

    /* Set Pll3 sw clock source. */
    /* PLL3_main = 480 MHz */
    CLOCK_SetMux(kCLOCK_Pll3SwMux, 0); // CCSR  (0) 0 - pll3_main_clk, 1 - pll3_bypass_clock
    /* Set lvds1 clock source. */

    CCM_ANALOG->MISC1 =
        (CCM_ANALOG->MISC1 & (~CCM_ANALOG_MISC1_LVDS1_CLK_SEL_MASK)) | CCM_ANALOG_MISC1_LVDS1_CLK_SEL(0);
    /* Set clock out1 divider. */
    CCM->CCOSR = (CCM->CCOSR & (~CCM_CCOSR_CLKO1_DIV_MASK)) | CCM_CCOSR_CLKO1_DIV(0);
    /* Set clock out1 source. */
    CCM->CCOSR = (CCM->CCOSR & (~CCM_CCOSR_CLKO1_SEL_MASK)) | CCM_CCOSR_CLKO1_SEL(1);
    /* Set clock out2 divider. */
    CCM->CCOSR = (CCM->CCOSR & (~CCM_CCOSR_CLKO2_DIV_MASK)) | CCM_CCOSR_CLKO2_DIV(0);
    /* Set clock out2 source. */
    CCM->CCOSR = (CCM->CCOSR & (~CCM_CCOSR_CLKO2_SEL_MASK)) | CCM_CCOSR_CLKO2_SEL(18);
    /* Set clock out1 drives clock out1. */
    CCM->CCOSR &= ~CCM_CCOSR_CLK_OUT_SEL_MASK;
    /* Disable clock out1. */
    CCM->CCOSR &= ~CCM_CCOSR_CLKO1_EN_MASK;
    /* Disable clock out2. */
    CCM->CCOSR &= ~CCM_CCOSR_CLKO2_EN_MASK;

    /* SAI MCLK */
    /* Set SAI1 MCLK1 clock source. */
    IOMUXC_SetSaiMClkClockSource(IOMUXC_GPR, kIOMUXC_GPR_SAI1MClk1Sel, 0);
    /* Set SAI1 MCLK2 clock source. */
    IOMUXC_SetSaiMClkClockSource(IOMUXC_GPR, kIOMUXC_GPR_SAI1MClk2Sel, 0);
    /* Set SAI1 MCLK3 clock source. */
    IOMUXC_SetSaiMClkClockSource(IOMUXC_GPR, kIOMUXC_GPR_SAI1MClk3Sel, 0);
    /* Set SAI2 MCLK3 clock source. */
    IOMUXC_SetSaiMClkClockSource(IOMUXC_GPR, kIOMUXC_GPR_SAI2MClk3Sel, 0);
    /* Set SAI3 MCLK3 clock source. */
    IOMUXC_SetSaiMClkClockSource(IOMUXC_GPR, kIOMUXC_GPR_SAI3MClk3Sel, 0);
    /* Set MQS configuration. */
    IOMUXC_MQSConfig(IOMUXC_GPR, kIOMUXC_MqsPwmOverSampleRate32, 0);

    /* Set ENET Tx clock source. */
    IOMUXC_EnableMode(IOMUXC_GPR, kIOMUXC_GPR_ENET1RefClkMode, false);
    /* Set GPT1 High frequency reference clock source. */
    IOMUXC_GPR->GPR5 &= ~IOMUXC_GPR_GPR5_VREF_1M_CLK_GPT1_MASK;
    /* Set GPT2 High frequency reference clock source. */
    IOMUXC_GPR->GPR5 &= ~IOMUXC_GPR_GPR5_VREF_1M_CLK_GPT2_MASK;

    /*
     * ------------------------------------------------------------------
     * PLL - ARM PLL
     * ------------------------------------------------------------------
     */

    clkPLL1setup(CLK_DISABLE);
    CLOCK_SetPllBypass(CCM_ANALOG, kCLOCK_PllArm, 1);

    /*
     * ------------------------------------------------------------------
     * PLL2 - System PLL
     * ------------------------------------------------------------------
     */
    /* Deinit System pfd0. */
    clkPLL2_PFD0setup(CLK_DISABLE);
    clkPLL2_PFD1setup(CLK_DISABLE);
    clkPLL2_PFD3setup(CLK_DISABLE);

    /* Disable pfd offset. */
    // CCM_ANALOG->PLL_SYS &= ~CCM_ANALOG_PLL_SYS_PFD_OFFSET_EN_MASK;

    /*
     * ------------------------------------------------------------------
     * PLL3 - USB1 PLL
     * ------------------------------------------------------------------
     */

    /* Init Usb1 PLL. */
    clkPLL3setup(CLK_DISABLE);
    /* Init Usb1 pfd0. */
    clkPLL3_PFD0setup(CLK_DISABLE);
    /* Init Usb1 pfd1. */
    clkPLL3_PFD1setup(CLK_DISABLE);
    /* Init Usb1 pfd2. */
    clkPLL3_PFD2setup(CLK_DISABLE);
    /* Init Usb1 pfd3. */
    clkPLL3_PFD3setup(CLK_DISABLE);
    /* Disable Usb1 PLL output for USBPHY1. */
    CCM_ANALOG->PLL_USB1 &= ~CCM_ANALOG_PLL_USB1_EN_USB_CLKS_MASK;
    /* Bypass Usb1 PLL */
    CLOCK_SetPllBypass(CCM_ANALOG, kCLOCK_PllUsb1, 1);

    /*
     * ------------------------------------------------------------------
     * PLL4 - Audio PLL
     * ------------------------------------------------------------------
     */

    /* DeInit Audio PLL */
    clkPLL4setup(CLK_DISABLE);
    CLOCK_SetPllBypass(CCM_ANALOG, kCLOCK_PllAudio, 1);

    /*
     * ------------------------------------------------------------------
     * PLL5 - Video PLL
     * ------------------------------------------------------------------
     */

    /* DeInit Video PLL. */
    clkPLL5setup(CLK_DISABLE);
    /* Bypass Video PLL. */
    CCM_ANALOG->PLL_VIDEO |= CCM_ANALOG_PLL_VIDEO_BYPASS_MASK;
    /* Set divider for Video PLL. */
    CCM_ANALOG->MISC2 = (CCM_ANALOG->MISC2 & (~CCM_ANALOG_MISC2_VIDEO_DIV_MASK)) | CCM_ANALOG_MISC2_VIDEO_DIV(0);
    /* Disable Video PLL output. */
    CCM_ANALOG->PLL_VIDEO &= ~CCM_ANALOG_PLL_VIDEO_ENABLE_MASK;

    /*
     * ------------------------------------------------------------------
     * PLL6 - ENET PLL
     * ------------------------------------------------------------------
     */

    /* DeInit Enet PLL. */
    clkPLL6setup(CLK_DISABLE);
    /* Bypass Enet PLL. */
    CLOCK_SetPllBypass(CCM_ANALOG, kCLOCK_PllEnet, 1);
    /* Set Enet output divider. */
    CCM_ANALOG->PLL_ENET =
        (CCM_ANALOG->PLL_ENET & (~CCM_ANALOG_PLL_ENET_DIV_SELECT_MASK)) | CCM_ANALOG_PLL_ENET_DIV_SELECT(1);
    /* Disable Enet output. */
    CCM_ANALOG->PLL_ENET &= ~CCM_ANALOG_PLL_ENET_ENABLE_MASK;
    /* Disable Enet25M output. */
    CCM_ANALOG->PLL_ENET &= ~CCM_ANALOG_PLL_ENET_ENET_25M_REF_EN_MASK;

    /*
     * ------------------------------------------------------------------
     * PLL7 - USB2 PLL
     * ------------------------------------------------------------------
     */

    /* DeInit Usb2 PLL. */
    clkPLL7setup(CLK_DISABLE);
    /* Bypass Usb2 PLL. */
    CLOCK_SetPllBypass(CCM_ANALOG, kCLOCK_PllUsb2, 1);
    /* Disable Usb2 PLL output. */
    CCM_ANALOG->PLL_USB2 &= ~CCM_ANALOG_PLL_USB2_ENABLE_MASK;

    /* Set preperiph clock source. */
    /* PLL1/2 = 432MHz */
    CLOCK_SetMux(kCLOCK_PrePeriphMux, 0); // CBCMR  (19-18) 0 - PLL2, 1 - PLL2_PFD2, 2 - PLL2_PFD0, 3 - PLL1
    /* Set periph clock source. */
    /* PRE_PERIPH_CLK <- PLL1/2 = 432MHz */
    CLOCK_SetMux(kCLOCK_PeriphMux, 0); // CBCDR  (25) 0 - pre_periph_clk_sel, 1 - periph_clk2_clk_divided

    /* Set SystemCoreClock variable. */
    SystemCoreClockUpdate();
}

/* ADC */
void clkADCsetup(uint8_t instance, uint8_t enabled)
{

    if (enabled) {
        if (instance == 1)
            CLOCK_EnableClock(kCLOCK_Adc1);
        if (instance == 2)
            CLOCK_EnableClock(kCLOCK_Adc2);
        if (instance == 0) {
            CLOCK_EnableClock(kCLOCK_Adc1);
            CLOCK_EnableClock(kCLOCK_Adc2);
        }
    }
    else {
        if (instance == 1)
            CLOCK_DisableClock(kCLOCK_Adc1);
        if (instance == 2)
            CLOCK_DisableClock(kCLOCK_Adc2);
        if (instance == 0) {
            CLOCK_DisableClock(kCLOCK_Adc1);
            CLOCK_DisableClock(kCLOCK_Adc2);
        }
    }
}

/* XBAR */
void clkXBARsetup(uint8_t instance, uint8_t enabled)
{

    if (enabled) {
        if (instance == 1)
            CLOCK_EnableClock(kCLOCK_Xbar1);
        if (instance == 2)
            CLOCK_EnableClock(kCLOCK_Xbar2);
        if (instance == 3)
            CLOCK_EnableClock(kCLOCK_Xbar3);
        if (instance == 0) {
            CLOCK_EnableClock(kCLOCK_Xbar1);
            CLOCK_EnableClock(kCLOCK_Xbar2);
            CLOCK_EnableClock(kCLOCK_Xbar3);
        }
    }
    else {
        if (instance == 1)
            CLOCK_DisableClock(kCLOCK_Xbar1);
        if (instance == 2)
            CLOCK_DisableClock(kCLOCK_Xbar2);
        if (instance == 3)
            CLOCK_DisableClock(kCLOCK_Xbar3);
        if (instance == 0) {
            CLOCK_DisableClock(kCLOCK_Xbar1);
            CLOCK_DisableClock(kCLOCK_Xbar2);
            CLOCK_DisableClock(kCLOCK_Xbar3);
        }
    }
}

/* GPT */
void clkGPTsetup(uint8_t instance, uint8_t enabled)
{

    if (enabled) {
        if (instance == 1) {
            CLOCK_EnableClock(kCLOCK_Gpt1);
            CLOCK_EnableClock(kCLOCK_Gpt1S);
        }
        if (instance == 2) {
            CLOCK_EnableClock(kCLOCK_Gpt2);
            CLOCK_EnableClock(kCLOCK_Gpt2S);
        }
        if (instance == 0) {
            CLOCK_EnableClock(kCLOCK_Gpt1);
            CLOCK_EnableClock(kCLOCK_Gpt1S);
            CLOCK_EnableClock(kCLOCK_Gpt2);
            CLOCK_EnableClock(kCLOCK_Gpt2S);
        }
    }
    else {
        if (instance == 1) {
            CLOCK_DisableClock(kCLOCK_Gpt1);
            CLOCK_DisableClock(kCLOCK_Gpt1S);
        }
        if (instance == 2) {
            CLOCK_DisableClock(kCLOCK_Gpt2);
            CLOCK_DisableClock(kCLOCK_Gpt2S);
        }
        if (instance == 0) {
            CLOCK_DisableClock(kCLOCK_Gpt1);
            CLOCK_DisableClock(kCLOCK_Gpt1S);
            CLOCK_DisableClock(kCLOCK_Gpt2);
            CLOCK_DisableClock(kCLOCK_Gpt2S);
        }
    }
}

/* PIT */
void clkPITsetup(uint8_t instance, uint8_t enabled)
{

    if (enabled) {
        CLOCK_EnableClock(kCLOCK_Pit);
    }
    else {
        /* PIT */
        CLOCK_DisableClock(kCLOCK_Pit);
    }
}

/* USDHC */
void clkUSDHCsetup(uint8_t instance, uint8_t enabled)
{

    if (enabled) {
        if ((instance == 1) || (instance == 0)) {
            /* USDHC */
            CLOCK_EnableClock(kCLOCK_Usdhc1);
        }

        if ((instance == 2) || (instance == 0)) {
            CLOCK_EnableClock(kCLOCK_Usdhc2);
        }
    }
    else {
        if ((instance == 1) || (instance == 0)) {
            /* USDHC */
            CLOCK_DisableClock(kCLOCK_Usdhc1);
        }

        if ((instance == 2) || (instance == 0)) {
            /* Disable USDHC2 clock gate. */
            CLOCK_DisableClock(kCLOCK_Usdhc2);
        }
    }
}

/* FLEXSPI */
void clkFLEXSPIsetup(uint8_t instance, uint8_t enabled)
{

    if (enabled) {
        /* FLEXSPI */
        CLOCK_EnableClock(kCLOCK_FlexSpi);
    }
    else {
        /* FLEXSPI */
        /* Disable Flexspi clock gate. */
        CLOCK_DisableClock(kCLOCK_FlexSpi);
    }
}

/* CSI */
void clkCSIsetup(uint8_t instance, uint8_t enabled)
{

    if (enabled) {
        CLOCK_EnableClock(kCLOCK_Csi);
    }
    else {
        /* CSI */
        /* Disable CSI clock gate. */
        CLOCK_DisableClock(kCLOCK_Csi);
    }
}

/* LPSPI */
void clkLPSPICsetup(uint8_t instance, uint8_t enabled)
{

    if (enabled) {
        if ((instance == 1) || (instance == 0))
            CLOCK_EnableClock(kCLOCK_Lpspi1);
        if ((instance == 2) || (instance == 0))
            CLOCK_EnableClock(kCLOCK_Lpspi2);
        if ((instance == 3) || (instance == 0))
            CLOCK_EnableClock(kCLOCK_Lpspi3);
        if ((instance == 4) || (instance == 0))
            CLOCK_EnableClock(kCLOCK_Lpspi4);
    }
    else {
        if ((instance == 1) || (instance == 0))
            CLOCK_DisableClock(kCLOCK_Lpspi1);
        if ((instance == 2) || (instance == 0))
            CLOCK_DisableClock(kCLOCK_Lpspi2);
        if ((instance == 3) || (instance == 0))
            CLOCK_DisableClock(kCLOCK_Lpspi3);
        if ((instance == 4) || (instance == 0))
            CLOCK_DisableClock(kCLOCK_Lpspi4);
    }
}

/* TRACE */
void clkTRACEsetup(uint8_t instance, uint8_t enabled)
{

    if (enabled) {
        CLOCK_EnableClock(kCLOCK_Trace);
    }
    else {
        /* TRACE */
        /* Disable TRACE clock gate. */
        CLOCK_DisableClock(kCLOCK_Trace);
    }
}

/* SAI */
void clkSAIsetup(uint8_t instance, uint8_t enabled)
{

    if (enabled) {
        if ((instance == 1) || (instance == 0))
            CLOCK_EnableClock(kCLOCK_Sai1);
        if ((instance == 2) || (instance == 0))
            CLOCK_EnableClock(kCLOCK_Sai2);
        if ((instance == 3) || (instance == 0))
            CLOCK_EnableClock(kCLOCK_Sai3);
    }
    else {
        /* Disable SAI1 clock gate. */
        if ((instance == 1) || (instance == 0))
            CLOCK_DisableClock(kCLOCK_Sai1);

        /* Disable SAI2 clock gate. */
        if ((instance == 2) || (instance == 0))
            CLOCK_DisableClock(kCLOCK_Sai2);

        /* Disable SAI3 clock gate. */
        if ((instance == 3) || (instance == 0))
            CLOCK_DisableClock(kCLOCK_Sai3);
    }
}

/* LPI2C */
void clkLPI2Csetup(uint8_t instance, uint8_t enabled)
{

    if (enabled) {
        if ((instance == 1) || (instance == 0))
            CLOCK_EnableClock(kCLOCK_Lpi2c1);
        if ((instance == 2) || (instance == 0))
            CLOCK_EnableClock(kCLOCK_Lpi2c2);
        if ((instance == 3) || (instance == 0))
            CLOCK_EnableClock(kCLOCK_Lpi2c3);
        if ((instance == 4) || (instance == 0))
            CLOCK_EnableClock(kCLOCK_Lpi2c4);
    }
    else {
        /* LPI2C */
        /* Disable Lpi2c clock gate. */
        if ((instance == 1) || (instance == 0))
            CLOCK_DisableClock(kCLOCK_Lpi2c1);
        if ((instance == 2) || (instance == 0))
            CLOCK_DisableClock(kCLOCK_Lpi2c2);
        if ((instance == 3) || (instance == 0))
            CLOCK_DisableClock(kCLOCK_Lpi2c3);
        if ((instance == 4) || (instance == 0))
            CLOCK_DisableClock(kCLOCK_Lpi2c4);
    }
}

/* CAN */
void clkCANsetup(uint8_t instance, uint8_t enabled)
{

    if (enabled) {
        if ((instance == 1) || (instance == 0)) {
            CLOCK_EnableClock(kCLOCK_Can1);
            CLOCK_EnableClock(kCLOCK_Can1S);
        }
        if ((instance == 2) || (instance == 0)) {
            CLOCK_EnableClock(kCLOCK_Can2);
            CLOCK_EnableClock(kCLOCK_Can2S);
        }
    }
    else {
        CLOCK_SetMux(kCLOCK_CanMux,
                     3); // CSCMR2  (9-8) 0 - pll3_sw_clk (divided clock 60M), 1 - osc_clk, 2 - pll3_sw_clk (divided
                         // clock 80M), 3 - disable
        if ((instance == 1) || (instance == 0)) {
            CLOCK_DisableClock(kCLOCK_Can1);
            CLOCK_DisableClock(kCLOCK_Can1S);
        }
        if ((instance == 2) || (instance == 0)) {
            CLOCK_DisableClock(kCLOCK_Can2);
            CLOCK_DisableClock(kCLOCK_Can2S);
        }
    }
}

/* LPUART */
void clkLPUARTsetup(uint8_t instance, uint8_t enabled)
{

    if (enabled) {
        if ((instance == 1) || (instance == 0))
            CLOCK_EnableClock(kCLOCK_Lpuart1);
        if ((instance == 2) || (instance == 0))
            CLOCK_EnableClock(kCLOCK_Lpuart2);
        if ((instance == 3) || (instance == 0))
            CLOCK_EnableClock(kCLOCK_Lpuart3);
        if ((instance == 4) || (instance == 0))
            CLOCK_EnableClock(kCLOCK_Lpuart4);
        if ((instance == 5) || (instance == 0))
            CLOCK_EnableClock(kCLOCK_Lpuart5);
        if ((instance == 6) || (instance == 0))
            CLOCK_EnableClock(kCLOCK_Lpuart6);
        if ((instance == 7) || (instance == 0))
            CLOCK_EnableClock(kCLOCK_Lpuart7);
        if ((instance == 8) || (instance == 0))
            CLOCK_EnableClock(kCLOCK_Lpuart8);
    }
    else {
        /* LPUART */
        if ((instance == 1) || (instance == 0))
            CLOCK_DisableClock(kCLOCK_Lpuart1);
        if ((instance == 2) || (instance == 0))
            CLOCK_DisableClock(kCLOCK_Lpuart2);
        if ((instance == 3) || (instance == 0))
            CLOCK_DisableClock(kCLOCK_Lpuart3);
        if ((instance == 4) || (instance == 0))
            CLOCK_DisableClock(kCLOCK_Lpuart4);
        if ((instance == 5) || (instance == 0))
            CLOCK_DisableClock(kCLOCK_Lpuart5);
        if ((instance == 6) || (instance == 0))
            CLOCK_DisableClock(kCLOCK_Lpuart6);
        if ((instance == 7) || (instance == 0))
            CLOCK_DisableClock(kCLOCK_Lpuart7);
        if ((instance == 8) || (instance == 0))
            CLOCK_DisableClock(kCLOCK_Lpuart8);
    }
}

/* LCDIF */
void clkLCDIFsetup(uint8_t instance, uint8_t enabled)
{

    if (enabled) {
        CLOCK_EnableClock(kCLOCK_LcdPixel);
    }
    else {
        /* LCDIF */
        /* Disable LCDIF clock gate. */
        CLOCK_DisableClock(kCLOCK_LcdPixel);
    }
}

/* SPDIF */
void clkSPDIFsetup(uint8_t instance, uint8_t enabled)
{

    if (enabled) {
        CLOCK_EnableClock(kCLOCK_Spdif);
    }
    else {
        /* SPDIF */
        /* Disable SPDIF clock gate. */
        CLOCK_DisableClock(kCLOCK_Spdif);
    }
}

/* FLEXIO */
void clkFLEXIOsetup(uint8_t instance, uint8_t enabled)
{

    if (enabled) {
        if ((instance == 1) || (instance == 0))
            CLOCK_EnableClock(kCLOCK_Flexio1);
        if ((instance == 2) || (instance == 0))
            CLOCK_EnableClock(kCLOCK_Flexio2);
    }
    else {
        /* FLEXIO */
        /* Disable Flexio1 clock gate. */
        if ((instance == 1) || (instance == 0))
            CLOCK_DisableClock(kCLOCK_Flexio1);

        /* Disable Flexio2 clock gate. */
        if ((instance == 2) || (instance == 0))
            CLOCK_DisableClock(kCLOCK_Flexio2);
    }
}

void clkPLL1setup(uint8_t enabled)
{
    const clock_arm_pll_config_t armPllConfig_BOARD_BootClockRUN = {
        .loopDivider = 86, /* PLL loop divider, Fout = Fin * 43 */
        .src         = 0,  /* Bypass clock source, 0 - OSC 24M, 1 - CLK1_P and CLK1_N */
    };

    if (enabled) {
        /* Init ARM PLL. */
        CLOCK_InitArmPll(&armPllConfig_BOARD_BootClockRUN);
    }
    else {
        CLOCK_DeinitArmPll();
    }
}

void clkPLL2setup(uint8_t enabled)
{
    const clock_sys_pll_config_t sysPllConfig_BOARD_BootClockRUN = {
        .loopDivider = 1, /* PLL loop divider, Fout = Fin * ( 20 + loopDivider*2 + numerator / denominator ) */
        .numerator   = 0, /* 30 bit numerator of fractional loop divider */
        .denominator = 1, /* 30 bit denominator of fractional loop divider */
        .src         = 0, /* Bypass clock source, 0 - OSC 24M, 1 - CLK1_P and CLK1_N */
    };
    if (enabled) {
        CLOCK_InitSysPll(&sysPllConfig_BOARD_BootClockRUN);
    }
    else {
        CLOCK_DeinitSysPll();
    }
}

void clkPLL2_PFD0setup(uint8_t enabled)
{
    if (enabled) {
        CLOCK_InitSysPfd(kCLOCK_Pfd0, 27);
    }
    else {
        CLOCK_DeinitSysPfd(kCLOCK_Pfd0);
    }
}

void clkPLL2_PFD1setup(uint8_t enabled)
{
    if (enabled) {
        CLOCK_InitSysPfd(kCLOCK_Pfd1, 16);
    }
    else {
        CLOCK_DeinitSysPfd(kCLOCK_Pfd1);
    }
}

void clkPLL2_PFD2setup(uint8_t enabled)
{
    if (enabled) {
        CLOCK_InitSysPfd(kCLOCK_Pfd2, 24);
    }
    else {
        CLOCK_DeinitSysPfd(kCLOCK_Pfd2);
    }
}

void clkPLL2_PFD3setup(uint8_t enabled)
{
    if (enabled) {
        CLOCK_InitSysPfd(kCLOCK_Pfd3, 16);
    }
    else {
        CLOCK_DeinitSysPfd(kCLOCK_Pfd3);
    }
}

void clkPLL3setup(uint8_t enabled)
{
    const clock_usb_pll_config_t usb1PllConfig_BOARD_BootClockRUN = {
        .loopDivider = 0, /* PLL loop divider, Fout = Fin * 20 */
        .src         = 0, /* Bypass clock source, 0 - OSC 24M, 1 - CLK1_P and CLK1_N */
    };
    if (enabled) {
        /* Init Usb1 PLL. */
        CLOCK_InitUsb1Pll(&usb1PllConfig_BOARD_BootClockRUN);
    }
    else {
        CLOCK_DeinitUsb1Pll();
    }
}

void clkPLL3_PFD0setup(uint8_t enabled)
{
    if (enabled) {
        /* Init Usb1 pfd0. */
        CLOCK_InitUsb1Pfd(kCLOCK_Pfd0, 33);
    }
    else {
        CLOCK_DeinitUsb1Pfd(kCLOCK_Pfd0);
    }
}

void clkPLL3_PFD1setup(uint8_t enabled)
{
    if (enabled) {
        /* Init Usb1 pfd1. */
        CLOCK_InitUsb1Pfd(kCLOCK_Pfd1, 16);
    }
    else {
        CLOCK_DeinitUsb1Pfd(kCLOCK_Pfd1);
    }
}

void clkPLL3_PFD2setup(uint8_t enabled)
{
    if (enabled) {
        /* Init Usb1 pfd2. */
        CLOCK_InitUsb1Pfd(kCLOCK_Pfd2, 17);
    }
    else {
        CLOCK_DeinitUsb1Pfd(kCLOCK_Pfd2);
    }
}

void clkPLL3_PFD3setup(uint8_t enabled)
{
    if (enabled) {
        /* Init Usb1 pfd3. */
        CLOCK_InitUsb1Pfd(kCLOCK_Pfd3, 19);
    }
    else {
        CLOCK_DeinitUsb1Pfd(kCLOCK_Pfd3);
    }
}

void clkPLL4setup(uint8_t enabled)
{
    const clock_audio_pll_config_t audioPllConfig_BOARD_BootClockRUN = {
        .loopDivider = 32,  /* PLL loop divider. Valid range for DIV_SELECT divider value: 27~54. */
        .postDivider = 1,   /* Divider after the PLL, should only be 1, 2, 4, 8, 16. */
        .numerator   = 77,  /* 30 bit numerator of fractional loop divider. */
        .denominator = 100, /* 30 bit denominator of fractional loop divider */
        .src         = 0,
    };
    if (enabled) {
        CLOCK_InitAudioPll(&audioPllConfig_BOARD_BootClockRUN);
    }
    else {
        /* DeInit Audio PLL */
        CLOCK_DeinitAudioPll();
        CLOCK_SetPllBypass(CCM_ANALOG, kCLOCK_PllAudio, 1);
    }
}

void clkPLL5setup(uint8_t enabled)
{
    const clock_video_pll_config_t videoPllConfig_BOARD_BootClockRUN = {
        .loopDivider = 54,                 /*!< PLL loop divider. Valid range for DIV_SELECT divider value: 27~54. */
        .postDivider = 16,                 /*!< Divider after the PLL, should only be 1, 2, 4, 8, 16. */
        .numerator   = 77,                 /*!< 30 bit numerator of fractional loop divider.*/
        .denominator = 100,                /*!< 30 bit denominator of fractional loop divider */
        .src         = kCLOCK_PllClkSrc24M /*!< Pll clock source, reference _clock_pll_clk_src */
    };
    if (enabled) {
        CLOCK_InitVideoPll(&videoPllConfig_BOARD_BootClockRUN);
    }
    else {
        /* DeInit Video PLL. */
        CLOCK_DeinitVideoPll();
    }
}

void clkPLL6setup(uint8_t enabled)
{
    const clock_enet_pll_config_t ethernetPllConfig_BOARD_BootClockRUN = {
        .enableClkOutput    = false, /*!< Power on and enable PLL clock output for ENET0 (ref_enetpll0). */
        .enableClkOutput25M = false, /*!< Power on and enable PLL clock output for ENET1 (ref_enetpll1). */
        .loopDivider        = 0,     /*!< Controls the frequency of the ENET0 reference clock.
                                 b00 25MHz
                                 b01 50MHz
                                 b10 100MHz (not 50% duty cycle)
                                 b11 125MHz */
        .src = kCLOCK_PllClkSrc24M   /*!< Pll clock source, reference _clock_pll_clk_src */

    };
    if (enabled) {
        CLOCK_InitEnetPll(&ethernetPllConfig_BOARD_BootClockRUN);
    }
    else {
        /* DeInit Enet PLL. */
        CLOCK_DeinitEnetPll();
    }
}

void clkPLL7setup(uint8_t enabled)
{
    const clock_usb_pll_config_t usb2PllConfig_BOARD_BootClockRUN = {
        .loopDivider = 0, /* PLL loop divider, Fout = Fin * 20 */
        .src         = 0, /* Bypass clock source, 0 - OSC 24M, 1 - CLK1_P and CLK1_N */
    };
    if (enabled) {
        CLOCK_InitUsb2Pll(&usb2PllConfig_BOARD_BootClockRUN);
    }
    else {
        /* DeInit Usb2 PLL. */
        CLOCK_DeinitUsb2Pll();
    }
}

uint32_t GetPerphSourceClock(PerphClock_t clock)
{
    switch (clock) {

    case PerphClock_I2C:
        return CLOCK_GetFreq(kCLOCK_OscClk) / (CLOCK_GetDiv(kCLOCK_Lpi2cDiv) + 1);
    case PerphClock_LPSPI:
        return CLOCK_GetFreq(kCLOCK_SysPllPfd2Clk) / (CLOCK_GetDiv(kCLOCK_LpspiDiv) + 1);
    case PerphClock_LPUART:
        return CLOCK_GetFreq(kCLOCK_OscClk) / (CLOCK_GetDiv(kCLOCK_UartDiv) + 1);
    case PerphClock_SAI1:
        return CLOCK_GetFreq(kCLOCK_AudioPllClk) / (CLOCK_GetDiv(kCLOCK_Sai1Div) + 1) /
               (CLOCK_GetDiv(kCLOCK_Sai1PreDiv) + 1);
    case PerphClock_SAI2:
        return CLOCK_GetFreq(kCLOCK_AudioPllClk) / (CLOCK_GetDiv(kCLOCK_Sai2Div) + 1) /
               (CLOCK_GetDiv(kCLOCK_Sai2PreDiv) + 1);
    case PerphClock_USDHC2:
        return CLOCK_GetFreq(kCLOCK_SysPllPfd2Clk) / (CLOCK_GetDiv(kCLOCK_Usdhc2Div) + 1U);
    default:
        return 0;
    }
}

void PrintSystemClocks()
{

    const char *_PLLNames[22] = {
        "kCLOCK_CpuClk",  /*!< CPU clock */
        "kCLOCK_AhbClk",  /*!< AHB clock */
        "kCLOCK_SemcClk", /*!< SEMC clock */
        "kCLOCK_IpgClk",  /*!< IPG clock */

        "kCLOCK_OscClk", /*!< OSC clock selected by PMU_LOWPWR_CTRL[OSC_SEL]. */
        "kCLOCK_RtcClk", /*!< RTC clock. (RTCCLK) */

        "kCLOCK_ArmPllClk", /*!< ARMPLLCLK. */

        "kCLOCK_Usb1PllClk",     /*!< USB1PLLCLK. */
        "kCLOCK_Usb1PllPfd0Clk", /*!< USB1PLLPDF0CLK. */
        "kCLOCK_Usb1PllPfd1Clk", /*!< USB1PLLPFD1CLK. */
        "kCLOCK_Usb1PllPfd2Clk", /*!< USB1PLLPFD2CLK. */
        "kCLOCK_Usb1PllPfd3Clk", /*!< USB1PLLPFD3CLK. */

        "kCLOCK_Usb2PllClk", /*!< USB2PLLCLK. */

        "kCLOCK_SysPllClk",     /*!< SYSPLLCLK. */
        "kCLOCK_SysPllPfd0Clk", /*!< SYSPLLPDF0CLK. */
        "kCLOCK_SysPllPfd1Clk", /*!< SYSPLLPFD1CLK. */
        "kCLOCK_SysPllPfd2Clk", /*!< SYSPLLPFD2CLK. */
        "kCLOCK_SysPllPfd3Clk", /*!< SYSPLLPFD3CLK. */

        "kCLOCK_EnetPll0Clk", /*!< Enet PLLCLK ref_enetpll0. */
        "kCLOCK_EnetPll1Clk", /*!< Enet PLLCLK ref_enetpll1. */

        "kCLOCK_AudioPllClk", /*!< Audio PLLCLK. */
        "kCLOCK_VideoPllClk", /*!< Video PLLCLK. */
    };
    int i;

    for (i = 0; i < 22; i++) {
        LOG_PRINTF("%s: %lu Hz\r\n", _PLLNames[i], CLOCK_GetFreq(static_cast<clock_name_t>(i)));
    }

    LOG_PRINTF("PerphSourceClock_I2C: %lu\r\n", GetPerphSourceClock(PerphClock_I2C));
    LOG_PRINTF("PerphSourceClock_LPSPI: %lu\r\n", GetPerphSourceClock(PerphClock_LPSPI));
    LOG_PRINTF("PerphSourceClock_LPUART: %lu\r\n", GetPerphSourceClock(PerphClock_LPUART));
    LOG_PRINTF("PerphSourceClock_SAI1: %lu\r\n", GetPerphSourceClock(PerphClock_SAI1));
    LOG_PRINTF("PerphSourceClock_SAI2: %lu\r\n", GetPerphSourceClock(PerphClock_SAI2));
    LOG_PRINTF("PerphSourceClock_USDHC2: %lu\r\n", GetPerphSourceClock(PerphClock_USDHC2));
}

#define CLOCK_CCM_HANDSHAKE_WAIT()                                                                                     \
                                                                                                                       \
    do {                                                                                                               \
        while (CCM->CDHIPR != 0) {}                                                                                    \
                                                                                                                       \
    } while (0)

#define LPM_DELAY(value)                                                                                               \
                                                                                                                       \
    do {                                                                                                               \
        for (uint32_t i = 0; i < 5 * value; i++) {                                                                     \
            __NOP();                                                                                                   \
        }                                                                                                              \
                                                                                                                       \
    } while (0)

void EnableWeakLDO(void)
{
    /* Enable Weak LDO 2P5 and 1P1 */
    PMU->REG_2P5_SET = PMU_REG_2P5_ENABLE_WEAK_LINREG_MASK;
    PMU->REG_1P1_SET = PMU_REG_1P1_ENABLE_WEAK_LINREG_MASK;

    LPM_DELAY(40);
}

void DisableWeakLDO(void)
{
    /* Disable Weak LDO 2P5 and 1P1 */
    PMU->REG_2P5_CLR = PMU_REG_2P5_ENABLE_WEAK_LINREG_MASK;
    PMU->REG_1P1_CLR = PMU_REG_1P1_ENABLE_WEAK_LINREG_MASK;
}

void EnableRegularLDO(void)
{
    /* Enable Regular LDO 2P5 and 1P1 */
    PMU->REG_2P5_SET = PMU_REG_2P5_ENABLE_LINREG_MASK;
    PMU->REG_1P1_SET = PMU_REG_1P1_ENABLE_LINREG_MASK;
}

void DisableRegularLDO(void)
{
    /* Disable Regular LDO 2P5 and 1P1 */
    PMU->REG_2P5_CLR = PMU_REG_2P5_ENABLE_LINREG_MASK;
    PMU->REG_1P1_CLR = PMU_REG_1P1_ENABLE_LINREG_MASK;
}

void BandgapOff(void)
{
    XTALOSC24M->LOWPWR_CTRL_SET = XTALOSC24M_LOWPWR_CTRL_LPBG_SEL_MASK;
    PMU->MISC0_SET |= PMU_MISC0_REFTOP_PWD_MASK;
}

void BandgapOn(void)
{
    /* Turn on regular bandgap and wait for stable */
    PMU->MISC0_CLR |= PMU_MISC0_REFTOP_PWD_MASK;
    while ((PMU->MISC0 & PMU_MISC0_REFTOP_VBGUP_MASK) == 0) {}
    /* Low power band gap disable */
    XTALOSC24M->LOWPWR_CTRL_CLR = XTALOSC24M_LOWPWR_CTRL_LPBG_SEL_MASK;
}

void LPM_SetWaitModeConfig(void)
{
    uint32_t clpcr;

    /*
     * ERR007265: CCM: When improper low-power sequence is used,
     * the SoC enters low power mode before the ARM core executes WFI.
     *
     * Software workaround:
     * 1) Software should trigger IRQ #41 (GPR_IRQ) to be always pending
     *      by setting IOMUXC_GPR_GPR1_GINT.
     * 2) Software should then unmask IRQ #41 in GPC before setting CCM
     *      Low-Power mode.
     * 3) Software should mask IRQ #41 right after CCM Low-Power mode
     *      is set (set bits 0-1 of CCM_CLPCR).
     */
    GPC_EnableIRQ(GPC, GPR_IRQ_IRQn);
    clpcr      = CCM->CLPCR & (~(CCM_CLPCR_LPM_MASK | CCM_CLPCR_ARM_CLK_DIS_ON_LPM_MASK));
    CCM->CLPCR = clpcr | CCM_CLPCR_LPM(kCLOCK_ModeWait) | CCM_CLPCR_MASK_SCU_IDLE_MASK | CCM_CLPCR_MASK_L2CC_IDLE_MASK |
                 CCM_CLPCR_ARM_CLK_DIS_ON_LPM_MASK | CCM_CLPCR_STBY_COUNT_MASK | CCM_CLPCR_BYPASS_LPM_HS0_MASK |
                 CCM_CLPCR_BYPASS_LPM_HS1_MASK;
    GPC_DisableIRQ(GPC, GPR_IRQ_IRQn);
}

void SetLowPowerClockGate(void)
{
    CCM->CCGR0 = CCM_CCGR0_CG0(1) | CCM_CCGR0_CG1(1) | CCM_CCGR0_CG3(3) | CCM_CCGR0_CG11(1) | CCM_CCGR0_CG12(1);
    CCM->CCGR1 = CCM_CCGR1_CG9(3) | CCM_CCGR1_CG10(1) | CCM_CCGR1_CG13(1) | CCM_CCGR1_CG14(1) /*| CCM_CCGR1_CG15(1)*/;
    CCM->CCGR2 = CCM_CCGR2_CG2(1) | CCM_CCGR2_CG8(1) | CCM_CCGR2_CG9(1) | CCM_CCGR2_CG10(1);
    CCM->CCGR3 =
        /*CCM_CCGR3_CG2(1) |*/ CCM_CCGR3_CG4(1) /*| CCM_CCGR3_CG9(1) */ /*| CCM_CCGR3_CG14(1) | CCM_CCGR3_CG15(1)*/;
    CCM->CCGR4 =
        CCM_CCGR4_CG1(1) | CCM_CCGR4_CG2(1) | CCM_CCGR4_CG4(1) | CCM_CCGR4_CG5(1) | CCM_CCGR4_CG6(1) | CCM_CCGR4_CG7(1);
    CCM->CCGR5 = CCM_CCGR5_CG0(1) | CCM_CCGR5_CG1(1) | CCM_CCGR5_CG4(1) | CCM_CCGR5_CG6(1) | CCM_CCGR5_CG12(1) |
                 CCM_CCGR5_CG14(1) /*| CCM_CCGR5_CG15(1)*/;
    /* We can enable DCDC when need to config it and close it after configuration */
    CCM->CCGR6 = CCM_CCGR6_CG3(1) | CCM_CCGR6_CG4(1) | CCM_CCGR6_CG5(1) | CCM_CCGR6_CG9(1) | CCM_CCGR6_CG10(1) |
                 CCM_CCGR6_CG11(1);
}

#define GPR8_DOZE_BITS                                                                                                 \
    (IOMUXC_GPR_GPR8_LPI2C1_IPG_DOZE_MASK | IOMUXC_GPR_GPR8_LPI2C2_IPG_DOZE_MASK |                                     \
     IOMUXC_GPR_GPR8_LPI2C3_IPG_DOZE_MASK | IOMUXC_GPR_GPR8_LPI2C4_IPG_DOZE_MASK |                                     \
     IOMUXC_GPR_GPR8_LPSPI1_IPG_DOZE_MASK | IOMUXC_GPR_GPR8_LPSPI2_IPG_DOZE_MASK |                                     \
     IOMUXC_GPR_GPR8_LPSPI3_IPG_DOZE_MASK | IOMUXC_GPR_GPR8_LPSPI4_IPG_DOZE_MASK |                                     \
     IOMUXC_GPR_GPR8_LPUART1_IPG_DOZE_MASK | IOMUXC_GPR_GPR8_LPUART2_IPG_DOZE_MASK |                                   \
     IOMUXC_GPR_GPR8_LPUART3_IPG_DOZE_MASK | IOMUXC_GPR_GPR8_LPUART4_IPG_DOZE_MASK |                                   \
     IOMUXC_GPR_GPR8_LPUART5_IPG_DOZE_MASK | IOMUXC_GPR_GPR8_LPUART6_IPG_DOZE_MASK |                                   \
     IOMUXC_GPR_GPR8_LPUART7_IPG_DOZE_MASK | IOMUXC_GPR_GPR8_LPUART8_IPG_DOZE_MASK)

#define GPR8_STOP_MODE_BITS                                                                                            \
    (IOMUXC_GPR_GPR8_LPI2C1_IPG_STOP_MODE_MASK | IOMUXC_GPR_GPR8_LPI2C2_IPG_STOP_MODE_MASK |                           \
     IOMUXC_GPR_GPR8_LPI2C3_IPG_STOP_MODE_MASK | IOMUXC_GPR_GPR8_LPI2C4_IPG_STOP_MODE_MASK |                           \
     IOMUXC_GPR_GPR8_LPSPI1_IPG_STOP_MODE_MASK | IOMUXC_GPR_GPR8_LPSPI2_IPG_STOP_MODE_MASK |                           \
     IOMUXC_GPR_GPR8_LPSPI3_IPG_STOP_MODE_MASK | IOMUXC_GPR_GPR8_LPSPI4_IPG_STOP_MODE_MASK |                           \
     IOMUXC_GPR_GPR8_LPUART2_IPG_STOP_MODE_MASK | IOMUXC_GPR_GPR8_LPUART3_IPG_STOP_MODE_MASK |                         \
     IOMUXC_GPR_GPR8_LPUART4_IPG_STOP_MODE_MASK | IOMUXC_GPR_GPR8_LPUART5_IPG_STOP_MODE_MASK |                         \
     IOMUXC_GPR_GPR8_LPUART6_IPG_STOP_MODE_MASK | IOMUXC_GPR_GPR8_LPUART7_IPG_STOP_MODE_MASK |                         \
     IOMUXC_GPR_GPR8_LPUART8_IPG_STOP_MODE_MASK)

#define GPR12_DOZE_BITS (IOMUXC_GPR_GPR12_FLEXIO1_IPG_DOZE_MASK | IOMUXC_GPR_GPR12_FLEXIO2_IPG_DOZE_MASK)

#define GPR12_STOP_MODE_BITS (IOMUXC_GPR_GPR12_FLEXIO1_IPG_STOP_MODE_MASK | IOMUXC_GPR_GPR12_FLEXIO2_IPG_STOP_MODE_MASK)

void PeripheralEnterDozeMode(void)
{
    IOMUXC_GPR->GPR8  = GPR8_DOZE_BITS;
    IOMUXC_GPR->GPR12 = GPR12_DOZE_BITS;
}

#define GPR4_STOP_REQ_BITS                                                                                             \
    (IOMUXC_GPR_GPR4_ENET_STOP_REQ_MASK | IOMUXC_GPR_GPR4_SAI1_STOP_REQ_MASK | IOMUXC_GPR_GPR4_SAI2_STOP_REQ_MASK |    \
     IOMUXC_GPR_GPR4_SAI3_STOP_REQ_MASK | IOMUXC_GPR_GPR4_SEMC_STOP_REQ_MASK | IOMUXC_GPR_GPR4_PIT_STOP_REQ_MASK |     \
     IOMUXC_GPR_GPR4_FLEXIO1_STOP_REQ_MASK | IOMUXC_GPR_GPR4_FLEXIO2_STOP_REQ_MASK)

#define GPR4_STOP_ACK_BITS                                                                                             \
    (IOMUXC_GPR_GPR4_ENET_STOP_ACK_MASK | IOMUXC_GPR_GPR4_SAI1_STOP_ACK_MASK | IOMUXC_GPR_GPR4_SAI2_STOP_ACK_MASK |    \
     IOMUXC_GPR_GPR4_SAI3_STOP_ACK_MASK | IOMUXC_GPR_GPR4_SEMC_STOP_ACK_MASK | IOMUXC_GPR_GPR4_PIT_STOP_ACK_MASK |     \
     IOMUXC_GPR_GPR4_FLEXIO1_STOP_ACK_MASK | IOMUXC_GPR_GPR4_FLEXIO2_STOP_ACK_MASK)

#define GPR7_STOP_REQ_BITS                                                                                             \
    (IOMUXC_GPR_GPR7_LPI2C1_STOP_REQ_MASK | IOMUXC_GPR_GPR7_LPI2C2_STOP_REQ_MASK |                                     \
     IOMUXC_GPR_GPR7_LPI2C3_STOP_REQ_MASK | IOMUXC_GPR_GPR7_LPI2C4_STOP_REQ_MASK |                                     \
     IOMUXC_GPR_GPR7_LPSPI1_STOP_REQ_MASK | IOMUXC_GPR_GPR7_LPSPI2_STOP_REQ_MASK |                                     \
     IOMUXC_GPR_GPR7_LPSPI3_STOP_REQ_MASK | IOMUXC_GPR_GPR7_LPSPI4_STOP_REQ_MASK |                                     \
     IOMUXC_GPR_GPR7_LPUART1_STOP_REQ_MASK | IOMUXC_GPR_GPR7_LPUART2_STOP_REQ_MASK |                                   \
     IOMUXC_GPR_GPR7_LPUART3_STOP_REQ_MASK | IOMUXC_GPR_GPR7_LPUART4_STOP_REQ_MASK |                                   \
     IOMUXC_GPR_GPR7_LPUART5_STOP_REQ_MASK | IOMUXC_GPR_GPR7_LPUART6_STOP_REQ_MASK |                                   \
     IOMUXC_GPR_GPR7_LPUART7_STOP_REQ_MASK | IOMUXC_GPR_GPR7_LPUART8_STOP_REQ_MASK)

#define GPR7_STOP_ACK_BITS                                                                                             \
    (IOMUXC_GPR_GPR7_LPI2C1_STOP_ACK_MASK | IOMUXC_GPR_GPR7_LPI2C2_STOP_ACK_MASK |                                     \
     IOMUXC_GPR_GPR7_LPI2C3_STOP_ACK_MASK | IOMUXC_GPR_GPR7_LPI2C4_STOP_ACK_MASK |                                     \
     IOMUXC_GPR_GPR7_LPSPI1_STOP_ACK_MASK | IOMUXC_GPR_GPR7_LPSPI2_STOP_ACK_MASK |                                     \
     IOMUXC_GPR_GPR7_LPSPI3_STOP_ACK_MASK | IOMUXC_GPR_GPR7_LPSPI4_STOP_ACK_MASK |                                     \
     IOMUXC_GPR_GPR7_LPUART1_STOP_ACK_MASK | IOMUXC_GPR_GPR7_LPUART2_STOP_ACK_MASK |                                   \
     IOMUXC_GPR_GPR7_LPUART3_STOP_ACK_MASK | IOMUXC_GPR_GPR7_LPUART4_STOP_ACK_MASK |                                   \
     IOMUXC_GPR_GPR7_LPUART5_STOP_ACK_MASK | IOMUXC_GPR_GPR7_LPUART6_STOP_ACK_MASK |                                   \
     IOMUXC_GPR_GPR7_LPUART7_STOP_ACK_MASK | IOMUXC_GPR_GPR7_LPUART8_STOP_ACK_MASK)

void PeripheralEnterStopMode(void)
{
    IOMUXC_GPR->GPR4 = IOMUXC_GPR_GPR4_ENET_STOP_REQ_MASK;
    while ((IOMUXC_GPR->GPR4 & IOMUXC_GPR_GPR4_ENET_STOP_ACK_MASK) != IOMUXC_GPR_GPR4_ENET_STOP_ACK_MASK) {}
    IOMUXC_GPR->GPR4  = GPR4_STOP_REQ_BITS;
    IOMUXC_GPR->GPR7  = GPR7_STOP_REQ_BITS;
    IOMUXC_GPR->GPR8  = GPR8_DOZE_BITS | GPR8_STOP_MODE_BITS;
    IOMUXC_GPR->GPR12 = GPR12_DOZE_BITS | GPR12_STOP_MODE_BITS;
    while ((IOMUXC_GPR->GPR4 & GPR4_STOP_ACK_BITS) != GPR4_STOP_ACK_BITS) {}
    while ((IOMUXC_GPR->GPR7 & GPR7_STOP_ACK_BITS) != GPR7_STOP_ACK_BITS) {}
}

void PowerDownUSBPHY(void)
{
    USBPHY1->CTRL = 0xFFFFFFFF;
    USBPHY2->CTRL = 0xFFFFFFFF;
}

void LPM_EnterLowPowerRun()
{
    /* Switch DCDC to use DCDC internal OSC */
    // DCDC_SetClockSource(DCDC, kDCDC_ClockInternalOsc);

    CLOCK_SetDiv(kCLOCK_PeriphClk2Div, 0);

    /* OSC_CLK (24M) */
    CLOCK_SetMux(kCLOCK_PeriphClk2Mux,
                 1); // CBCMR (13-12) 0 - pll3_sw_clk, 1 - osc_clk (pll1_ref_clk), 2 - pll2_bypass_clk, 3 - reserved
    /* PERIPH_CLK2_SEL */
    CLOCK_SetMux(kCLOCK_PeriphMux, 1); // CBCDR (25) 0 - pre_periph_clk_sel, 1 - periph_clk2_clk_divided

    /* SET AHB, IPG to 12MHz */
    CLOCK_SetDiv(kCLOCK_IpgDiv, 0);
    CLOCK_SetDiv(kCLOCK_AhbDiv, 1);

    /*Set PERCLK to 12Mhz*/
    CLOCK_SetMux(kCLOCK_PerclkMux, 0x0);
    CLOCK_SetDiv(kCLOCK_PerclkDiv, 0x0);

    /* Power down USBPHY */
    USBPHY1->CTRL = 0xFFFFFFFF;
    USBPHY2->CTRL = 0xFFFFFFFF;

    /* Adjust SOC voltage to 0.95V */
    DCDC_AdjustTargetVoltage(DCDC, 0x6, 0x1);
    /* DCM Mode */
    DCDC_BootIntoDCM(DCDC);
    /* Disconnect internal the load resistor */
    DCDC->REG1 &= ~DCDC_REG1_REG_RLOAD_SW_MASK;
    /* Power Down output range comparator */
    DCDC->REG0 |= DCDC_REG0_PWD_CMP_OFFSET_MASK;

    /* Enable FET ODRIVE */
    PMU->REG_CORE_SET = PMU_REG_CORE_FET_ODRIVE_MASK;
    /* Connect vdd_high_in and connect vdd_snvs_in */
    PMU->MISC0_CLR = PMU_MISC0_DISCON_HIGH_SNVS_MASK;

    EnableWeakLDO();
    DisableRegularLDO();
    BandgapOff();
}

/*

#include "fsl_semc.h"
#include "pin_mux.h"
status_t BOARD_InitSEMC(void)
{
    semc_config_t config;
    semc_sdram_config_t sdramconfig;
    uint32_t clockFrq = CLOCK_GetFreq(kCLOCK_SemcClk);

    */
/* Initializes the MAC configure structure to zero. */                                              /*
                                             
                                                  memset(&config, 0, sizeof(semc_config_t));
                                                  memset(&sdramconfig, 0, sizeof(semc_sdram_config_t));
                                             
                                                  */
/* Initialize SEMC. */                                                                              /*
                                                                             
                                                                                  SEMC_GetDefaultConfig(&config);
                                                                                  config.dqsMode = kSEMC_Loopbackdqspad; */
/* For more accurate timing. */                                                                     /*
                                                                    
                                                                         SEMC_Init(SEMC, &config);
                                                                    
                                                                         */
/* Configure SDRAM. */                                                                              /*
                                                                             
                                                                                  sdramconfig.csxPinMux           = kSEMC_MUXCSX0;
                                                                                  sdramconfig.address             = 0x80000000;
                                                                                  sdramconfig.memsize_kbytes      = 16 * 1024; */
/* 16MB = 32*1024*1KBytes*/                                                                         /*
                                                                        
                                                                             sdramconfig.portSize            = kSEMC_PortSize16Bit;
                                                                             sdramconfig.burstLen            = kSEMC_Sdram_BurstLen8;
                                                                             sdramconfig.columnAddrBitNum    = kSEMC_SdramColunm_9bit;
                                                                             sdramconfig.casLatency          = kSEMC_LatencyThree;
                                                                             sdramconfig.tPrecharge2Act_Ns   = 18; */
/* Trp 18ns */                                                                                      /*
                                                                                     
                                                                                          sdramconfig.tAct2ReadWrite_Ns   = 18; */
/* Trcd 18ns */                                                                                     /*
                                                                                    
                                                                                         sdramconfig.tRefreshRecovery_Ns = 67; */
/* Use the maximum of the (Trfc , Txsr). */                                                         /*
                                                        
                                                             sdramconfig.tWriteRecovery_Ns   = 12; */
/* 12ns */                                                                                          /*
                                                                                         
                                                                                              sdramconfig.tCkeOff_Ns =
                                                                                                      42; */
/* The minimum cycle of SDRAM CLK off state. CKE is off in self refresh at a minimum period tRAS.*/ /*

     sdramconfig.tAct2Prechage_Ns       = 42; */
/* Tras 42ns */                                                                                     /*
                                                                                    
                                                                                         sdramconfig.tSelfRefRecovery_Ns    = 67;
                                                                                         sdramconfig.tRefresh2Refresh_Ns    = 60;
                                                                                         sdramconfig.tAct2Act_Ns            = 60;
                                                                                         sdramconfig.tPrescalePeriod_Ns     = 160 * (1000000000 / clockFrq);
                                                                                         sdramconfig.refreshPeriod_nsPerRow = 64 * 1000000 / 8192; */
/* 64ms/8192 */                                                                                     /*
                                                                                    
                                                                                         sdramconfig.refreshUrgThreshold    = sdramconfig.refreshPeriod_nsPerRow;
                                                                                         sdramconfig.refreshBurstLen        = 1;
                                                                                         return SEMC_ConfigureSDRAM(SEMC, kSEMC_SDRAM_CS0, &sdramconfig, clockFrq);
                                                                                     }
                                                                                     */

void LPM_EnterFullSpeed(void)
{
    CLOCK_InitExternalClk(0);

    /* Switch DCDC to use DCDC external OSC */
    DCDC_SetClockSource(DCDC, kDCDC_ClockExternalOsc);

    /* Connect internal the load resistor */
    DCDC->REG1 |= DCDC_REG1_REG_RLOAD_SW_MASK;

    /* Adjust SOC voltage to 1.275V */
    DCDC_AdjustTargetVoltage(DCDC, 0x13, 0x1);

    /* Disable FET ODRIVE */
    PMU->REG_CORE_SET &= ~PMU_REG_CORE_FET_ODRIVE_MASK;
    PMU->MISC0_CLR &= ~PMU_MISC0_DISCON_HIGH_SNVS_MASK;

    BandgapOn();
    EnableRegularLDO();
    DisableWeakLDO();

    /* Switch clock source to external OSC. */
    CLOCK_SwitchOsc(kCLOCK_XtalOsc);
    /* Set Oscillator ready counter value. */
    CCM->CCR = (CCM->CCR & (~CCM_CCR_OSCNT_MASK)) | CCM_CCR_OSCNT(127);
    /* Setting PeriphClk2Mux and PeriphMux to provide stable clock before PLLs are initialed */
    /* OSC_CLK (24M) */
    CLOCK_SetMux(kCLOCK_PeriphClk2Mux,
                 1); // CBCMR (13-12) 0 - pll3_sw_clk, 1 - osc_clk (pll1_ref_clk), 2 - pll2_bypass_clk, 3 - reserved
    /* PERIPH_CLK2_SEL */
    CLOCK_SetMux(kCLOCK_PeriphMux, 1); // CBCDR (25) 0 - pre_periph_clk_sel, 1 - periph_clk2_clk_divided

    /* Set AHB_PODF. */
    CLOCK_SetDiv(kCLOCK_AhbDiv, 0); // CBCDR

    /* Set IPG_PODF. */
    /* AHB_CLK/4 */
    CLOCK_SetDiv(kCLOCK_IpgDiv, 3); // CBCDR

    /* Set ARM_PODF. */
    /* PLL1/2 (864MHz / 2 = 432 MHz) */
    CLOCK_SetDiv(kCLOCK_ArmDiv, 1); // CACRR

    /* Set PERCLK_PODF. */
    /* IPG_CLK_ROOT/2 */
    CLOCK_SetDiv(kCLOCK_PerclkDiv, 0); // CSCMR1
    /* Set per clock source. */
    CLOCK_SetMux(kCLOCK_PerclkMux, 1); // CSCMR1  (6) 0 - ipg_clk_root, 1 - osc_clk - PIT, GPT

    clkPLL2setup(CLK_ENABLE);
    CLOCK_SetMux(kCLOCK_SemcMux, 1);

    /* Set preperiph clock source. */
    /* PLL1/2 = 432MHz */
    CLOCK_SetMux(kCLOCK_PrePeriphMux, 0); // CBCMR  (19-18) 0 - PLL2, 1 - PLL2_PFD2, 2 - PLL2_PFD0, 3 - PLL1
    /* Set periph clock source. */
    /* PRE_PERIPH_CLK <- PLL1/2 = 432MHz */
    CLOCK_SetMux(kCLOCK_PeriphMux, 0); // CBCDR  (25) 0 - pre_periph_clk_sel, 1 - periph_clk2_clk_divided

    /* Adjust SOC voltage to 1.15V */
    DCDC_AdjustTargetVoltage(DCDC, 0xe, 0x1);

    /* Set SystemCoreClock variable. */
    SystemCoreClockUpdate();
}

void LPM_EnterLowPowerIdle(void)
{
    /* Turn on FlexRAM0 */
    GPC->CNTR &= ~GPC_CNTR_PDRAM0_PGE_MASK;
    /* Turn on FlexRAM1 */
    PGC->MEGA_CTRL &= ~PGC_MEGA_CTRL_PCR_MASK;

    LPM_SetWaitModeConfig();
    // SetLowPowerClockGate();

    CLOCK_SetDiv(kCLOCK_PeriphClk2Div, 0);

    /* OSC_CLK (24M) */
    CLOCK_SetMux(kCLOCK_PeriphClk2Mux,
                 1); // CBCMR (13-12) 0 - pll3_sw_clk, 1 - osc_clk (pll1_ref_clk), 2 - pll2_bypass_clk, 3 - reserved
    /* PERIPH_CLK2_SEL */
    CLOCK_SetMux(kCLOCK_PeriphMux, 1); // CBCDR (25) 0 - pre_periph_clk_sel, 1 - periph_clk2_clk_divided

    /* SET AHB, IPG to 12MHz */
    CLOCK_SetDiv(kCLOCK_IpgDiv, 0);
    CLOCK_SetDiv(kCLOCK_AhbDiv, 1);

    /*Set PERCLK to 12Mhz*/
    CLOCK_SetMux(kCLOCK_PerclkMux, 0x0);
    CLOCK_SetDiv(kCLOCK_PerclkDiv, 0x0);

    /* Switch clock source to internal RC */
    CLOCK_SwitchOsc(kCLOCK_RcOsc);

    CLOCK_DeinitExternalClk();

    /* Power down USBPHY */
    PowerDownUSBPHY();

    /* Adjust SOC voltage to 0.90V */
    DCDC_AdjustTargetVoltage(DCDC, 0x4, 0x0);
    /* Switch DCDC to use DCDC internal OSC */
    DCDC_SetClockSource(DCDC, kDCDC_ClockInternalOsc);

    /* Disconnect internal the load resistor */
    DCDC->REG1 &= ~DCDC_REG1_REG_RLOAD_SW_MASK;
    /* Power Down output range comparator */
    DCDC->REG0 |= DCDC_REG0_PWD_CMP_OFFSET_MASK;

    /* Enable FET ODRIVE */
    PMU->REG_CORE_SET = PMU_REG_CORE_FET_ODRIVE_MASK;
    /* Connect vdd_high_in and connect vdd_snvs_in */
    PMU->MISC0_CLR = PMU_MISC0_DISCON_HIGH_SNVS_MASK;

    EnableWeakLDO();
    DisableRegularLDO();
    BandgapOff();

    CLOCK_SetMux(kCLOCK_SemcMux, 0);
    clkPLL2setup(CLK_DISABLE);
}
