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

#include "clock_config.h"
#include "board.h"
#include "fsl_iomuxc.h"

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
 *  2   | PLL1 (ARM)         | core            | 516MHz
 *      |                    | ADC, XBAR       | 129MHz
 *      |                    | PIT, GPT        | 64,5MHz
 *      |                    | SEMC            | 129MHz
 * ---------------------------------------------------------
 *  3   | PLL2_PFD2 (SYS)    | USDHC1          | 198MHz
 *      |                    | USDHC2          | 132MHz
 *      |                    | LPSPI           | 49,5MHz
 *      |                    | TRACE           | 117,33MHz
 * ---------------------------------------------------------
 *  4   | PLL3_PFD0 (USB1)   | FlexSPI         | 261,81MHz
 * ---------------------------------------------------------
 *  5   | PLL3               | CAN             | 40MHz
 *      |                    | SPDIF0          | 30MHz
 *      |                    | FLEXIO1         | 30MHz
 *      |                    | FLEXIO2         | 30MHz
 *      |                    | SAI1_MCLK3      | 30MHz
 *      |                    | SAI2_MCLK3      | 30MHz
 *      |                    | SAI3_MCLK3      | 30MHz
 * ---------------------------------------------------------
 *  6   | PLL3_PFD1          | LCDIF           | 67,5MHz
 * ---------------------------------------------------------
 *  7   | PLL3_PFD2          | SAI3            | 63,52MHz
 *      |                    | SAI3_MCLK1      | 63,52MHz
 *      |                    | MQS_MCLK        | 63,52MHz
 * ---------------------------------------------------------
 *  8   | PLL4 (audio)       | SAI1            | 6,14MHz
 *      |                    | SAI2            | 6,14MHz
 *      |                    | SAI1_MCLK1      | 6,14MHz
 *      |                    | SAI1_MCLK2      | 6,14MHz
 *      |                    | SAI2_MCLK1      | 6,14MHz
 */

void BOARD_BootClockRUN(void)
{
	/* Init RTC OSC clock frequency. */
	CLOCK_SetRtcXtalFreq(32768U);
	/* Enable 1MHz clock output. */
	XTALOSC24M->OSC_CONFIG2 |= XTALOSC24M_OSC_CONFIG2_ENABLE_1M_MASK;
	/* Use free 1MHz clock output. */
	XTALOSC24M->OSC_CONFIG2 &= ~XTALOSC24M_OSC_CONFIG2_MUX_1M_MASK;
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
	CLOCK_SetMux(kCLOCK_PeriphClk2Mux, 1); //CBCMR (13-12) 0 - pll3_sw_clk, 1 - osc_clk (pll1_ref_clk), 2 - pll2_bypass_clk, 3 - reserved
	/* PERIPH_CLK2_SEL */
	CLOCK_SetMux(kCLOCK_PeriphMux, 1);     //CBCDR (25) 0 - pre_periph_clk_sel, 1 - periph_clk2_clk_divided
	/* Setting the VDD_SOC to 1.275V. It is necessary to config AHB to 600Mhz. */
	DCDC->REG3 = (DCDC->REG3 & (~DCDC_REG3_TRG_MASK)) | DCDC_REG3_TRG(0x13);
	/* Waiting for DCDC_STS_DC_OK bit is asserted */
	while (DCDC_REG0_STS_DC_OK_MASK != (DCDC_REG0_STS_DC_OK_MASK & DCDC->REG0))
	{
	}

	/* Set AHB_PODF. */
	CLOCK_SetDiv(kCLOCK_AhbDiv, 0);	//CBCDR

	/* Set IPG_PODF. */
	/* AHB_CLK/4 */
	CLOCK_SetDiv(kCLOCK_IpgDiv, 3);	//CBCDR

	/* Set ARM_PODF. */
	/* PLL1/2 (864MHz / 2 = 432 MHz) */
	CLOCK_SetDiv(kCLOCK_ArmDiv, 1);	//CACRR

	/* Set PERCLK_PODF. */
	/* IPG_CLK_ROOT/2 */
	CLOCK_SetDiv(kCLOCK_PerclkDiv, 1);	//CSCMR1
	/* Set per clock source. */
	CLOCK_SetMux(kCLOCK_PerclkMux, 0);	//CSCMR1  (6) 0 - ipg_clk_root, 1 - osc_clk - PIT, GPT




	/* Set USDHC1_PODF. */
	CLOCK_SetDiv(kCLOCK_Usdhc1Div, 1);	//CSCDR1
	/* Set Usdhc1 clock source. */
	/* PLL2_PFD2/2 = 396MHz/2 = 198MHz */
	CLOCK_SetMux(kCLOCK_Usdhc1Mux, 0);	//CSCMR1  (16) 0 - PLL2_PFD2, 1 - PLL2_PFD0

	/* Set USDHC2_PODF. */
	CLOCK_SetDiv(kCLOCK_Usdhc2Div, 2);	//CSCDR1
	/* Set Usdhc2 clock source. */
	/* PLL2_PFD2/3 = 396MHz/3 = 132MHz */
	CLOCK_SetMux(kCLOCK_Usdhc2Mux, 0);	//CSCMR1  (17) 0 - PLL2_PFD2, 1 - PLL2_PFD0

	/* Set FLEXSPI_PODF. */
	CLOCK_SetDiv(kCLOCK_FlexspiDiv, 0);	//CSCMR1
	/* Set Flexspi clock source. */
	CLOCK_SetMux(kCLOCK_FlexspiMux, 3);	//CSCMR1  (30-29) 0 - semc_clk_reet_pre, 1 - pll3_sw_clk, 2 - PLL2_PFD2, 3 - PLL3_PFD0

	/* Set CSI_PODF. */
	CLOCK_SetDiv(kCLOCK_CsiDiv, 1);	//CSCDR3
	/* Set Csi clock source. */
	CLOCK_SetMux(kCLOCK_CsiMux, 0);	//CSCDR3  (10-9) 0 - osc_clk, 1 - PLL2_PFD2, 2 - pll3_120M, 3 - PLL3_PFD1

	/* Set LPSPI_PODF. */
	CLOCK_SetDiv(kCLOCK_LpspiDiv, 7);	//CBCMR
	/* Set Lpspi clock source. */
	/* PLL2_PFD2/8 = 396MHz / 8 = 49,5MHz */
	CLOCK_SetMux(kCLOCK_LpspiMux, 3);	//CBCMR  (5-4) 0 - PLL3_PFD1, 1 - PLL3_PFD0, 2 - PLL2, 3 - PLL2_PFD2

	/* Set TRACE_PODF. */
	CLOCK_SetDiv(kCLOCK_TraceDiv, 2);	//CSCDR1
	/* Set Trace clock source. */
	/* PLL2_PFD0/4 disabled*/
	CLOCK_SetMux(kCLOCK_TraceMux, 2);	//CBCMR  (15-14) 0 - PLL2, 1 - PLL2_PFD2, 2 - PLL2_PFD0, 3 - PLL2_PFD1

	/* Set SAI1_CLK_PRED. */
	CLOCK_SetDiv(kCLOCK_Sai1PreDiv, 1);	//CS1CDR
	/* Set SAI1_CLK_PODF. */
	CLOCK_SetDiv(kCLOCK_Sai1Div, 63);	//CS1CDR
	/* Set Sai1 clock source. */
	CLOCK_SetMux(kCLOCK_Sai1Mux, 2);	//CSCMR1  (11-10) 0 - PLL3_PFD2, 1 - PLL5, 2 - PLL4, 3 - reserved

	/* Set SAI2_CLK_PRED. */
	CLOCK_SetDiv(kCLOCK_Sai2PreDiv, 1);	//CS2CDR
	/* Set SAI2_CLK_PODF. */
	CLOCK_SetDiv(kCLOCK_Sai2Div, 63);	//CS2CDR
	/* Set Sai2 clock source. */
	CLOCK_SetMux(kCLOCK_Sai2Mux, 2);	//CSCMR1  (13-12) 0 - PLL3_PFD2, 1 - PLL5, 2 - PLL4, 3 - reserved

	/* Set SAI3_CLK_PRED. */
	CLOCK_SetDiv(kCLOCK_Sai3PreDiv, 3);	//CS1CDR
	/* Set SAI3_CLK_PODF. */
	CLOCK_SetDiv(kCLOCK_Sai3Div, 1);	//CS1CDR
	/* Set Sai3 clock source. */
	CLOCK_SetMux(kCLOCK_Sai3Mux, 0);	//CSCMR1  (15-14) 0 - PLL3_PFD2, 1 - PLL5, 2 - PLL4, 3 - reserved

	/* Set LPI2C_CLK_PODF. */
	CLOCK_SetDiv(kCLOCK_Lpi2cDiv, 1);	//CSCDR2
	/* Set Lpi2c clock source. */
	/* OSC_CLK (24MHz/2 = 12MHz */
	CLOCK_SetMux(kCLOCK_Lpi2cMux, 1);	//CSCDR2  (18) 0 - pll3_60m, 1 - osc_clk

	/* Set CAN_CLK_PODF. */
	CLOCK_SetDiv(kCLOCK_CanDiv, 1);		//CSCMR2
	/* Set Can clock source. */
	CLOCK_SetMux(kCLOCK_CanMux, 2);		//CSCMR2  (9-8) 0 - pll3_sw_clk (divided clock 60M), 1 - osc_clk, 2 - pll3_sw_clk (divided clock 80M), 3 - disable

	/* Set UART_CLK_PODF. */
	CLOCK_SetDiv(kCLOCK_UartDiv, 0);	//CSCDR1
	/* Set Uart clock source. */
	/* OSC_CLK (24MHz)/1 */
	CLOCK_SetMux(kCLOCK_UartMux, 1);	//CSCDR1  (6) 0 - pll3_80m, 1 - osc_clk

	/* Set LCDIF_PRED. */
	CLOCK_SetDiv(kCLOCK_LcdifPreDiv, 1);	//CSCDR2
	/* Set LCDIF_CLK_PODF. */
	CLOCK_SetDiv(kCLOCK_LcdifDiv, 3);	//CBCMR
	/* Set Lcdif pre clock source. */
	/* PLL3_PFD1 (664.62MHz/4/2 = 83.08MHz disabled */
	CLOCK_SetMux(kCLOCK_LcdifPreMux, 5);	//CSCDR2  (17-15) 0 - Pll2, 1 - PLL3_PFD3, 2 - PLL5, 3 - PLL2_PFD0, 4 - PLL2_PFD1, 5 - PLL3_PFD1, 6,7 - reserved

	/* Set SPDIF0_CLK_PRED. */
	CLOCK_SetDiv(kCLOCK_Spdif0PreDiv, 1);	//CDCDR
	/* Set SPDIF0_CLK_PODF. */
	CLOCK_SetDiv(kCLOCK_Spdif0Div, 7);		//CDCDR
	/* Set Spdif clock source. */
	/* PLL3_main/8/2 = 480MHz / 8 / 2 = 30MHz disabled*/
	CLOCK_SetMux(kCLOCK_SpdifMux, 3);		//CDCDR  (21-20) 0 - PLL4, 1 - PLL3_PFD2, 2 - PLL5, 3 - pll3_sw_clk

	/* Set FLEXIO1_CLK_PRED. */
	CLOCK_SetDiv(kCLOCK_Flexio1PreDiv, 1);	//CDCDR
	/* Set FLEXIO1_CLK_PODF. */
	CLOCK_SetDiv(kCLOCK_Flexio1Div, 7);		//CDCDR
	/* Set Flexio1 clock source. */
	/* PLL3_main/8/2 = 480MHz / 8 / 2 = 30MHz disabled*/
	CLOCK_SetMux(kCLOCK_Flexio1Mux, 3);		//CDCDR (8-7) 0 - PLL4, 1 - PlLL3_PFD2, 2 - PLL5, 3 - pll3_sw_clk

	/* Set FLEXIO2_CLK_PRED. */
	CLOCK_SetDiv(kCLOCK_Flexio2PreDiv, 1);	//CS1CDR
	/* Set FLEXIO2_CLK_PODF. */
	CLOCK_SetDiv(kCLOCK_Flexio2Div, 7);		//CS1CDR
	/* Set Flexio2 clock source. */
	/* PLL3_main/8/2 = 480MHz / 8 / 2 = 30MHz disabled*/
	CLOCK_SetMux(kCLOCK_Flexio2Mux, 3);		//CSCMR2 (20-19) 0 - PLL4, 1 - PLL3_PFD2, 2 - PLL5, 3 - pll3_sw_clk

	clkFLEXSPIsetup( CLK_INSTANCE_ALL, CLK_DISABLE );

	/* ADC */
	clkADCsetup( CLK_INSTANCE_ALL, CLK_DISABLE );
	/* XBAR */
	clkXBARsetup( CLK_INSTANCE_ALL, CLK_DISABLE );
	/* GPT */
	clkGPTsetup( CLK_INSTANCE_ALL, CLK_DISABLE );
	/* PIT */
	clkPITsetup( CLK_INSTANCE_ALL, CLK_DISABLE );
	/* USDHC */
	clkUSDHCsetup( CLK_INSTANCE_ALL, CLK_DISABLE );

	/* CSI */
	clkCSIsetup( CLK_INSTANCE_ALL, CLK_DISABLE );
	/* LPSPI */
	clkLPSPICsetup( CLK_INSTANCE_ALL, CLK_DISABLE );
	/* TRACE */
	clkTRACEsetup( CLK_INSTANCE_ALL, CLK_DISABLE );
	/* SAI */
	clkSAIsetup( CLK_INSTANCE_ALL, CLK_DISABLE );
	/* LPI2C */
	clkLPI2Csetup( CLK_INSTANCE_ALL, CLK_DISABLE );
	/* CAN */
	clkCANsetup( CLK_INSTANCE_ALL, CLK_DISABLE );
	/* LPUART */
	clkLPUARTsetup( CLK_INSTANCE_ALL, CLK_DISABLE );
	/* LCDIF */
	clkLCDIFsetup( CLK_INSTANCE_ALL, CLK_DISABLE );
	/* SPDIF */
	clkSPDIFsetup( CLK_INSTANCE_ALL, CLK_DISABLE );
	/* FLEXIO */
	clkFLEXIOsetup( CLK_INSTANCE_ALL, CLK_DISABLE );

	/* Set Pll3 sw clock source. */
	/* PLL3_main = 480 MHz */
	CLOCK_SetMux(kCLOCK_Pll3SwMux, 0);		//CCSR  (0) 0 - pll3_main_clk, 1 - pll3_bypass_clock
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
	/* Init ARM PLL. */
	clkPLL1setup(CLK_DISABLE);

	/* Init System pfd0. */
	clkPLL2_PFD0setup(CLK_ENABLE);

	/* Disable pfd offset. */
	CCM_ANALOG->PLL_SYS &= ~CCM_ANALOG_PLL_SYS_PFD_OFFSET_EN_MASK;

	/* USB1 PLL */
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

	/* VideoPLL */
	/* DeInit Video PLL. */
	clkPLL5setup(CLK_DISABLE);
	/* Bypass Video PLL. */
	CCM_ANALOG->PLL_VIDEO |= CCM_ANALOG_PLL_VIDEO_BYPASS_MASK;
	/* Set divider for Video PLL. */
	CCM_ANALOG->MISC2 = (CCM_ANALOG->MISC2 & (~CCM_ANALOG_MISC2_VIDEO_DIV_MASK)) | CCM_ANALOG_MISC2_VIDEO_DIV(0);
	/* Disable Video PLL output. */
	CCM_ANALOG->PLL_VIDEO &= ~CCM_ANALOG_PLL_VIDEO_ENABLE_MASK;

	/* Ethernet PLL */
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

	/* USB2 PLL */
	/* DeInit Usb2 PLL. */
	CLOCK_DeinitUsb2Pll();
	clkPLL7setup(CLK_DISABLE);
	/* Bypass Usb2 PLL. */
	CLOCK_SetPllBypass(CCM_ANALOG, kCLOCK_PllUsb2, 1);
	/* Enable Usb2 PLL output. */
	CCM_ANALOG->PLL_USB2 |= CCM_ANALOG_PLL_USB2_ENABLE_MASK;

	/* Set preperiph clock source. */
	/* PLL1/2 = 432MHz */
	CLOCK_SetMux(kCLOCK_PrePeriphMux, 0);		//CBCMR  (19-18) 0 - PLL2, 1 - PLL2_PFD2, 2 - PLL2_PFD0, 3 - PLL1
	/* Set periph clock source. */
	/* PRE_PERIPH_CLK <- PLL1/2 = 432MHz */
	CLOCK_SetMux(kCLOCK_PeriphMux, 0);			//CBCDR  (25) 0 - pre_periph_clk_sel, 1 - periph_clk2_clk_divided
	/* Set PERIPH_CLK2_PODF. */
	/* PLL3_SW_CLK/1 = 480MHz */
	CLOCK_SetDiv(kCLOCK_PeriphClk2Div, 0);		//CBCDR
	/* Set periph clock2 clock source. */
	/* PLL3_SW_CLK = 480MHz */
	CLOCK_SetMux(kCLOCK_PeriphClk2Mux, 0);		//CBCMR  (13-12) 0 - pll3_sw_clk, 1 - osc_clk (pll1_ref_clk), 2 - pll2_bypass_clk, 3 - reserved
	/* Set SystemCoreClock variable. */
    SystemCoreClockUpdate();

	/* DeInit Audio PLL */
	clkPLL4setup(CLK_DISABLE);

}

/* ADC */
void clkADCsetup( uint8_t instance, uint8_t enabled ) {

	if (enabled) {
		if (instance == 1) CLOCK_EnableClock(kCLOCK_Adc1);
		if (instance == 2) CLOCK_EnableClock(kCLOCK_Adc2);
		if (instance == 0) {
			CLOCK_EnableClock(kCLOCK_Adc1);
			CLOCK_EnableClock(kCLOCK_Adc2);
		}
	}
	else {
		if (instance == 1) CLOCK_DisableClock(kCLOCK_Adc1);
		if (instance == 2) CLOCK_DisableClock(kCLOCK_Adc2);
		if (instance == 0) {
			CLOCK_DisableClock(kCLOCK_Adc1);
			CLOCK_DisableClock(kCLOCK_Adc2);
		}
	}
}
/* XBAR */
void clkXBARsetup( uint8_t instance, uint8_t enabled ) {

	if (enabled) {
		if (instance == 1) CLOCK_EnableClock(kCLOCK_Xbar1);
		if (instance == 2) CLOCK_EnableClock(kCLOCK_Xbar2);
		if (instance == 3) CLOCK_EnableClock(kCLOCK_Xbar3);
		if (instance == 0) {
			CLOCK_EnableClock(kCLOCK_Xbar1);
			CLOCK_EnableClock(kCLOCK_Xbar2);
			CLOCK_EnableClock(kCLOCK_Xbar3);
		}
	}
	else {
		if (instance == 1) CLOCK_DisableClock(kCLOCK_Xbar1);
		if (instance == 2) CLOCK_DisableClock(kCLOCK_Xbar2);
		if (instance == 3) CLOCK_DisableClock(kCLOCK_Xbar3);
		if (instance == 0) {
			CLOCK_DisableClock(kCLOCK_Xbar1);
			CLOCK_DisableClock(kCLOCK_Xbar2);
			CLOCK_DisableClock(kCLOCK_Xbar3);
		}
	}
}
/* GPT */
void clkGPTsetup( uint8_t instance, uint8_t enabled ) {

	if (enabled) {
		if (instance == 1) { CLOCK_EnableClock(kCLOCK_Gpt1); CLOCK_EnableClock(kCLOCK_Gpt1S); }
		if (instance == 2) { CLOCK_EnableClock(kCLOCK_Gpt2); CLOCK_EnableClock(kCLOCK_Gpt2S); }
		if (instance == 0) {
			CLOCK_EnableClock(kCLOCK_Gpt1);
			CLOCK_EnableClock(kCLOCK_Gpt1S);
			CLOCK_EnableClock(kCLOCK_Gpt2);
			CLOCK_EnableClock(kCLOCK_Gpt2S);
		}
	}
	else {
		if (instance == 1) { CLOCK_DisableClock(kCLOCK_Gpt1); CLOCK_DisableClock(kCLOCK_Gpt1S); }
		if (instance == 2) { CLOCK_DisableClock(kCLOCK_Gpt2); CLOCK_DisableClock(kCLOCK_Gpt2S); }
		if (instance == 0) {
			CLOCK_DisableClock(kCLOCK_Gpt1);
			CLOCK_DisableClock(kCLOCK_Gpt1S);
			CLOCK_DisableClock(kCLOCK_Gpt2);
			CLOCK_DisableClock(kCLOCK_Gpt2S);
		}
	}
}
/* PIT */
void clkPITsetup( uint8_t instance, uint8_t enabled ) {

	if (enabled) {
		CLOCK_EnableClock(kCLOCK_Pit);
	}
	else {
		/* PIT */
		CLOCK_DisableClock(kCLOCK_Pit);
	}
}
/* USDHC */
void clkUSDHCsetup( uint8_t instance, uint8_t enabled ) {

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
void clkFLEXSPIsetup( uint8_t instance, uint8_t enabled ) {

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
void clkCSIsetup( uint8_t instance, uint8_t enabled ) {

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
void clkLPSPICsetup( uint8_t instance, uint8_t enabled ) {

	if (enabled) {
		if ((instance == 1) || (instance == 0)) CLOCK_EnableClock(kCLOCK_Lpspi1);
		if ((instance == 2) || (instance == 0)) CLOCK_EnableClock(kCLOCK_Lpspi2);
		if ((instance == 3) || (instance == 0)) CLOCK_EnableClock(kCLOCK_Lpspi3);
		if ((instance == 4) || (instance == 0)) CLOCK_EnableClock(kCLOCK_Lpspi4);
	}
	else {
		if ((instance == 1) || (instance == 0)) CLOCK_DisableClock(kCLOCK_Lpspi1);
		if ((instance == 2) || (instance == 0)) CLOCK_DisableClock(kCLOCK_Lpspi2);
		if ((instance == 3) || (instance == 0)) CLOCK_DisableClock(kCLOCK_Lpspi3);
		if ((instance == 4) || (instance == 0)) CLOCK_DisableClock(kCLOCK_Lpspi4);
	}
}
/* TRACE */
void clkTRACEsetup( uint8_t instance, uint8_t enabled ) {

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
void clkSAIsetup( uint8_t instance, uint8_t enabled ) {

	if (enabled) {
		if ((instance == 1) || (instance == 0)) CLOCK_EnableClock(kCLOCK_Sai1);
		if ((instance == 2) || (instance == 0)) CLOCK_EnableClock(kCLOCK_Sai2);
		if ((instance == 3) || (instance == 0)) CLOCK_EnableClock(kCLOCK_Sai3);

	}
	else {
		/* Disable SAI1 clock gate. */
		if ((instance == 1) || (instance == 0)) CLOCK_DisableClock(kCLOCK_Sai1);

		/* Disable SAI2 clock gate. */
		if ((instance == 2) || (instance == 0)) CLOCK_DisableClock(kCLOCK_Sai2);

		/* Disable SAI3 clock gate. */
		if ((instance == 3) || (instance == 0)) CLOCK_DisableClock(kCLOCK_Sai3);

	}
}
/* LPI2C */
void clkLPI2Csetup( uint8_t instance, uint8_t enabled ) {

	if (enabled) {
		if ((instance == 1) || (instance == 0)) CLOCK_EnableClock(kCLOCK_Lpi2c1);
		if ((instance == 2) || (instance == 0)) CLOCK_EnableClock(kCLOCK_Lpi2c2);
		if ((instance == 3) || (instance == 0)) CLOCK_EnableClock(kCLOCK_Lpi2c3);
	}
	else {
		/* LPI2C */
		/* Disable Lpi2c clock gate. */
		if ((instance == 1) || (instance == 0)) CLOCK_DisableClock(kCLOCK_Lpi2c1);
		if ((instance == 2) || (instance == 0)) CLOCK_DisableClock(kCLOCK_Lpi2c2);
		if ((instance == 3) || (instance == 0)) CLOCK_DisableClock(kCLOCK_Lpi2c3);

	}
}
/* CAN */
void clkCANsetup( uint8_t instance, uint8_t enabled ) {

	if (enabled) {
		if ((instance == 1) || (instance == 0)) { CLOCK_EnableClock(kCLOCK_Can1); CLOCK_EnableClock(kCLOCK_Can1S); }
		if ((instance == 2) || (instance == 0)) { CLOCK_EnableClock(kCLOCK_Can2); CLOCK_EnableClock(kCLOCK_Can2S); }
	}
	else {
		CLOCK_SetMux(kCLOCK_CanMux, 3);		//CSCMR2  (9-8) 0 - pll3_sw_clk (divided clock 60M), 1 - osc_clk, 2 - pll3_sw_clk (divided clock 80M), 3 - disable
		if ((instance == 1) || (instance == 0)) { CLOCK_DisableClock(kCLOCK_Can1); CLOCK_DisableClock(kCLOCK_Can1S); }
		if ((instance == 2) || (instance == 0)) { CLOCK_DisableClock(kCLOCK_Can2); CLOCK_DisableClock(kCLOCK_Can2S); }

	}
}
/* LPUART */
void clkLPUARTsetup( uint8_t instance, uint8_t enabled ) {

	if (enabled) {
		if ((instance == 1) || (instance == 0)) CLOCK_EnableClock(kCLOCK_Lpuart1);
		if ((instance == 2) || (instance == 0)) CLOCK_EnableClock(kCLOCK_Lpuart2);
		if ((instance == 3) || (instance == 0)) CLOCK_EnableClock(kCLOCK_Lpuart3);
		if ((instance == 4) || (instance == 0)) CLOCK_EnableClock(kCLOCK_Lpuart4);
		if ((instance == 5) || (instance == 0)) CLOCK_EnableClock(kCLOCK_Lpuart5);
		if ((instance == 6) || (instance == 0)) CLOCK_EnableClock(kCLOCK_Lpuart6);
		if ((instance == 7) || (instance == 0)) CLOCK_EnableClock(kCLOCK_Lpuart7);
		if ((instance == 8) || (instance == 0)) CLOCK_EnableClock(kCLOCK_Lpuart8);
	}
	else {
		/* LPUART */
		if ((instance == 1) || (instance == 0)) CLOCK_DisableClock(kCLOCK_Lpuart1);
		if ((instance == 2) || (instance == 0)) CLOCK_DisableClock(kCLOCK_Lpuart2);
		if ((instance == 3) || (instance == 0)) CLOCK_DisableClock(kCLOCK_Lpuart3);
		if ((instance == 4) || (instance == 0)) CLOCK_DisableClock(kCLOCK_Lpuart4);
		if ((instance == 5) || (instance == 0)) CLOCK_DisableClock(kCLOCK_Lpuart5);
		if ((instance == 6) || (instance == 0)) CLOCK_DisableClock(kCLOCK_Lpuart6);
		if ((instance == 7) || (instance == 0)) CLOCK_DisableClock(kCLOCK_Lpuart7);
		if ((instance == 8) || (instance == 0)) CLOCK_DisableClock(kCLOCK_Lpuart8);

	}
}
/* LCDIF */
void clkLCDIFsetup( uint8_t instance, uint8_t enabled ) {

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
void clkSPDIFsetup( uint8_t instance, uint8_t enabled ) {

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
void clkFLEXIOsetup( uint8_t instance, uint8_t enabled ) {
	

	if (enabled) {
		if ((instance == 1) || (instance == 0)) CLOCK_EnableClock(kCLOCK_Flexio1);
		if ((instance == 2) || (instance == 0)) CLOCK_EnableClock(kCLOCK_Flexio2);
	}
	else {
		/* FLEXIO */
		/* Disable Flexio1 clock gate. */
		if ((instance == 1) || (instance == 0)) CLOCK_DisableClock(kCLOCK_Flexio1);

		/* Disable Flexio2 clock gate. */
		if ((instance == 2) || (instance == 0)) CLOCK_DisableClock(kCLOCK_Flexio2);
	}
}

void clkPLL1setup( uint8_t enabled ) {
	const clock_arm_pll_config_t armPllConfig_BOARD_BootClockRUN =
	{
			.loopDivider = 86,                       /* PLL loop divider, Fout = Fin * 43 */
			.src = 0,                                 /* Bypass clock source, 0 - OSC 24M, 1 - CLK1_P and CLK1_N */
	};

	if (enabled) {
		/* Init ARM PLL. */
		CLOCK_InitArmPll(&armPllConfig_BOARD_BootClockRUN);
	}
	else {
		CLOCK_DeinitArmPll();
	}
}

void clkPLL2setup( uint8_t enabled ) {
	const clock_sys_pll_config_t sysPllConfig_BOARD_BootClockRUN =
	{
			.loopDivider = 1,                         /* PLL loop divider, Fout = Fin * ( 20 + loopDivider*2 + numerator / denominator ) */
			.numerator = 0,                           /* 30 bit numerator of fractional loop divider */
			.denominator = 1,                         /* 30 bit denominator of fractional loop divider */
			.src = 0,                                 /* Bypass clock source, 0 - OSC 24M, 1 - CLK1_P and CLK1_N */
	};
	if (enabled) {
		CLOCK_InitSysPll(&sysPllConfig_BOARD_BootClockRUN);
	}
	else {
		CLOCK_DeinitSysPll();
	}
}

void clkPLL2_PFD0setup( uint8_t enabled ) {
	if (enabled) {
		CLOCK_InitSysPfd(kCLOCK_Pfd0, 27);
	}
	else {
		CLOCK_DeinitSysPfd(kCLOCK_Pfd0);
	}
}
void clkPLL2_PFD1setup( uint8_t enabled ) {
	if (enabled) {
		CLOCK_InitSysPfd(kCLOCK_Pfd1, 16);
	}
	else {
		CLOCK_DeinitSysPfd(kCLOCK_Pfd1);
	}
}
void clkPLL2_PFD2setup( uint8_t enabled ) {
	if (enabled) {
		CLOCK_InitSysPfd(kCLOCK_Pfd2, 24);
	}
	else {
		CLOCK_DeinitSysPfd(kCLOCK_Pfd2);
	}
}
void clkPLL2_PFD3setup( uint8_t enabled ) {
	if (enabled) {
		CLOCK_InitSysPfd(kCLOCK_Pfd3, 16);
	}
	else {
		CLOCK_DeinitSysPfd(kCLOCK_Pfd3);
	}
}

void clkPLL3setup( uint8_t enabled ) {
	const clock_usb_pll_config_t usb1PllConfig_BOARD_BootClockRUN =
	{
			.loopDivider = 0,                         /* PLL loop divider, Fout = Fin * 20 */
			.src = 0,                                 /* Bypass clock source, 0 - OSC 24M, 1 - CLK1_P and CLK1_N */
	};
	if (enabled) {
		/* Init Usb1 PLL. */
		CLOCK_InitUsb1Pll(&usb1PllConfig_BOARD_BootClockRUN);
	}
	else {
		CLOCK_DeinitUsb1Pll();
	}
}
void clkPLL3_PFD0setup( uint8_t enabled ) {
	if (enabled) {
		/* Init Usb1 pfd0. */
		CLOCK_InitUsb1Pfd(kCLOCK_Pfd0, 33);
	}
	else {
		CLOCK_DeinitUsb1Pfd(kCLOCK_Pfd0);
	}
}
void clkPLL3_PFD1setup( uint8_t enabled ) {
	if (enabled) {
		/* Init Usb1 pfd1. */
		CLOCK_InitUsb1Pfd(kCLOCK_Pfd1, 16);
	}
	else {
		CLOCK_DeinitUsb1Pfd(kCLOCK_Pfd1);
	}
}
void clkPLL3_PFD2setup( uint8_t enabled ) {
	if (enabled) {
		/* Init Usb1 pfd2. */
		CLOCK_InitUsb1Pfd(kCLOCK_Pfd2, 17);
	}
	else {
		CLOCK_DeinitUsb1Pfd(kCLOCK_Pfd2);
	}
}
void clkPLL3_PFD3setup( uint8_t enabled ) {
	if (enabled) {
		/* Init Usb1 pfd3. */
		CLOCK_InitUsb1Pfd(kCLOCK_Pfd3, 19);
	}
	else {
		CLOCK_DeinitUsb1Pfd(kCLOCK_Pfd3);
	}
}

void clkPLL4setup( uint8_t enabled ) {
	const clock_audio_pll_config_t audioPllConfig_BOARD_BootClockRUN =
	{
			.loopDivider = 32,  /* PLL loop divider. Valid range for DIV_SELECT divider value: 27~54. */
			.postDivider = 1,   /* Divider after the PLL, should only be 1, 2, 4, 8, 16. */
			.numerator = 77,    /* 30 bit numerator of fractional loop divider. */
			.denominator = 100, /* 30 bit denominator of fractional loop divider */
	};
	if (enabled) {
		CLOCK_InitAudioPll(&audioPllConfig_BOARD_BootClockRUN);
	}
	else {
		/* DeInit Audio PLL */
		CLOCK_DeinitAudioPll();
	}
}

void clkPLL5setup( uint8_t enabled ) {
	const clock_video_pll_config_t videoPllConfig_BOARD_BootClockRUN =
	{
			.loopDivider = 54,  /*!< PLL loop divider. Valid range for DIV_SELECT divider value: 27~54. */
			.postDivider = 16,  /*!< Divider after the PLL, should only be 1, 2, 4, 8, 16. */
			.numerator = 77,    /*!< 30 bit numerator of fractional loop divider.*/
			.denominator = 100, /*!< 30 bit denominator of fractional loop divider */
			.src = kCLOCK_PllClkSrc24M          /*!< Pll clock source, reference _clock_pll_clk_src */
	};
	if (enabled) {
		CLOCK_InitVideoPll(&videoPllConfig_BOARD_BootClockRUN);
	}
	else {
		/* DeInit Video PLL. */
		CLOCK_DeinitVideoPll();
	}
}

void clkPLL6setup( uint8_t enabled ) {
	const clock_enet_pll_config_t ethernetPllConfig_BOARD_BootClockRUN =
	{
			.enableClkOutput = false, /*!< Power on and enable PLL clock output for ENET0 (ref_enetpll0). */
			.enableClkOutput25M = false, /*!< Power on and enable PLL clock output for ENET1 (ref_enetpll1). */
			.loopDivider = 0,     /*!< Controls the frequency of the ENET0 reference clock.
	                                  b00 25MHz
	                                  b01 50MHz
	                                  b10 100MHz (not 50% duty cycle)
	                                  b11 125MHz */
			.src = kCLOCK_PllClkSrc24M             /*!< Pll clock source, reference _clock_pll_clk_src */

	};
	if (enabled) {
		CLOCK_InitEnetPll(&ethernetPllConfig_BOARD_BootClockRUN);
	}
	else {
		/* DeInit Enet PLL. */
		CLOCK_DeinitEnetPll();
	}
}

void clkPLL7setup( uint8_t enabled ) {
	const clock_usb_pll_config_t usb2PllConfig_BOARD_BootClockRUN =
	{
			.loopDivider = 0,                         /* PLL loop divider, Fout = Fin * 20 */
			.src = 0,                                 /* Bypass clock source, 0 - OSC 24M, 1 - CLK1_P and CLK1_N */
	};
	if (enabled) {
		CLOCK_InitUsb2Pll(&usb2PllConfig_BOARD_BootClockRUN);
	}
	else {
		/* DeInit Usb2 PLL. */
		CLOCK_DeinitUsb2Pll();
	}
}
