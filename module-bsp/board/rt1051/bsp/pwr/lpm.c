/*
 * Copyright 2017 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifdef FSL_RTOS_FREE_RTOS
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "fsl_tickless_generic.h"
#endif

#include "lpm.h"
#include "fsl_gpc.h"
#include "fsl_dcdc.h"
#include "fsl_gpt.h"
#include "fsl_iomuxc.h"
#include "fsl_lpuart.h"

#include "clock_config.h"
#include <board/board.h>

#include "FreeRTOS.h"
#include "semphr.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define LPM_GPC_IMR_NUM (sizeof(GPC->IMR) / sizeof(GPC->IMR[0]))

#ifdef FSL_RTOS_FREE_RTOS
/* Define the counter clock of the systick (GPT). For accuracy purpose,
 * please make LPM_SYSTICK_COUNTER_FREQ divisible by 32768, and times of
 * configTICK_RATE_HZ.
 */
#define LPM_SYSTICK_COUNTER_FREQ (32768)
#define LPM_COUNT_PER_TICK       (LPM_SYSTICK_COUNTER_FREQ / configTICK_RATE_HZ)

struct _lpm_power_mode_listener
{
    lpm_power_mode_callback_t callback;
    void *data;
    struct _lpm_power_mode_listener *next;
};

typedef struct _lpm_power_mode_listener lpm_power_mode_listener_t;
#endif

typedef struct _lpm_clock_context
{
    uint32_t armDiv;
    uint32_t ahbDiv;
    uint32_t ipgDiv;
    uint32_t perDiv;
    uint32_t perSel;
    uint32_t periphSel;
    uint32_t preperiphSel;
    uint32_t pfd480;
    uint32_t pfd528;
    uint32_t pllArm_loopdiv;
    uint32_t pllArm;
    uint32_t pllSys;
    uint32_t pllUsb1;
    uint32_t pllUsb2;
    uint32_t pllAudio;
    uint32_t pllVideo;
    uint32_t pllEnet;
    uint32_t is_valid;
} lpm_clock_context_t;

typedef void (*lpm_system_func_t)(uint32_t context);
typedef void (*freertos_tick_func_t)(void);

/*******************************************************************************
 * Variables
 ******************************************************************************/

static lpm_power_mode_t s_curMode;
static lpm_clock_context_t s_clockContext;
static uint32_t s_DllBackupValue = 0;
static int32_t s_SystemIdleFlag  = 0;
static int32_t s_SkipRestorePLLs = 0;

#ifdef FSL_RTOS_FREE_RTOS
static SemaphoreHandle_t s_mutex;
static lpm_power_mode_listener_t *s_listenerHead;
static lpm_power_mode_listener_t *s_listenerTail;
#if (configUSE_TICKLESS_IDLE == 1)
GPT_Type *vPortGetGptBase(void);
IRQn_Type vPortGetGptIrqn(void);
#endif
#endif

static lpm_power_mode_t s_targetPowerMode;
void APP_PowerPreSwitchHook(lpm_power_mode_t targetMode);
void APP_PowerPostSwitchHook(lpm_power_mode_t targetMode);
lpm_power_mode_t APP_GetLPMPowerMode(void);

/*******************************************************************************
 * Code
 ******************************************************************************/
static void BOARD_SetLPClockGate(void)
{
    CCM->CCGR0 = 0x014030C5U;
    CCM->CCGR1 = 0x541C0000U;
    CCM->CCGR2 = 0x00150010U;
    CCM->CCGR3 = 0x50040130U;

    CCM->CCGR4 = 0x00005514U;
    CCM->CCGR5 = 0x51001105U;
    /* We can enable DCDC when need to config it and close it after configuration */
    CCM->CCGR6 = 0x00540540U;
}

static void LPM_SetClockMode(clock_mode_t mode, uint32_t clpcr)
{
    switch (mode) {
    case kCLOCK_ModeRun:
        CCM->CLPCR = clpcr;
        break;
    default:
        /*
         * ERR007265: CCM: When improper low-power sequence is used,
         * the SoC enters low power mode before the ARM core executes WFI.
         *
         * Software workaround:
         * 1) Software should trigger IRQ #41 (GPR_IRQ) to be always pending
         *    by setting IOMUXC_GPR_GPR1_GINT.
         * 2) Software should then unmask IRQ #41 in GPC before setting CCM
         *    Low-Power mode.
         * 3) Software should mask IRQ #41 right after CCM Low-Power mode
         *    is set (set bits 0-1 of CCM_CLPCR).
         *
         */
        LPM_EnableWakeupSource(GPR_IRQ_IRQn);
        CCM->CLPCR = clpcr;
        LPM_DisableWakeupSource(GPR_IRQ_IRQn);
        break;
    }
}

void LPM_SwitchToXtalOSC(void)
{
    /* Restore XTAL-OSC and enable detector */
    CCM_ANALOG->MISC0_CLR = CCM_ANALOG_MISC0_XTAL_24M_PWD_MASK; /* Power up */
    while ((XTALOSC24M->LOWPWR_CTRL & XTALOSC24M_LOWPWR_CTRL_XTALOSC_PWRUP_STAT_MASK) == 0) {}
    CCM_ANALOG->MISC0_SET = CCM_ANALOG_MISC0_OSC_XTALOK_EN_MASK; /* detect freq */
    while ((CCM_ANALOG->MISC0 & CCM_ANALOG_MISC0_OSC_XTALOK_MASK) == 0) {}
    CCM_ANALOG->MISC0_CLR = CCM_ANALOG_MISC0_OSC_XTALOK_EN_MASK;

    /* Switch to XTAL-OSC */
    XTALOSC24M->LOWPWR_CTRL_CLR = XTALOSC24M_LOWPWR_CTRL_CLR_OSC_SEL_MASK;
    /* Turn off XTAL-OSC detector */
    CCM_ANALOG->MISC0_CLR = CCM_ANALOG_MISC0_OSC_XTALOK_EN_MASK;

    /* Wait CCM operation finishes */
    CLOCK_CCM_HANDSHAKE_WAIT();
    /* Take some delay */
    LPM_DELAY(40);
}

void LPM_SwitchToRcOSC(void)
{
    /* Switch to RC-OSC */
    XTALOSC24M->LOWPWR_CTRL_SET = XTALOSC24M_LOWPWR_CTRL_SET_OSC_SEL_MASK;
    /* Turn off XTAL-OSC */
    CCM_ANALOG->MISC0_SET = CCM_ANALOG_MISC0_XTAL_24M_PWD_MASK; /* Power down */

    /* Wait CCM operation finishes */
    CLOCK_CCM_HANDSHAKE_WAIT();
    /* Take some delay */
    LPM_DELAY(40);
}

void LPM_SwitchFlexspiClock(lpm_power_mode_t power_mode)
{
    while (!((FLEXSPI->STS0 & FLEXSPI_STS0_ARBIDLE_MASK) && (FLEXSPI->STS0 & FLEXSPI_STS0_SEQIDLE_MASK))) {
        ;
    }
    FLEXSPI->MCR0 |= FLEXSPI_MCR0_MDIS_MASK;
    /* Disable clock gate of flexspi. */
    CCM->CCGR6 &= (~CCM_CCGR6_CG5_MASK);

    /* Periph_clk output will be used as SEMC clock root */
    CLOCK_SET_MUX((uint32_t)kCLOCK_SemcMux, 0x0);
    /* Set post divider for SEMC clock as 0. */
    CLOCK_SET_DIV(kCLOCK_SemcDiv, 0x0);

    /* Semc_clk_root_pre will be used as flexspi clock. */
    CLOCK_SET_MUX((uint32_t)kCLOCK_FlexspiMux, 0x0);
    /* Set divider for flexspi clock root 0. */
    CLOCK_SET_DIV((uint32_t)kCLOCK_FlexspiDiv, 0x0);

    /* Enable clock gate of flexspi. */
    CCM->CCGR6 |= (CCM_CCGR6_CG5_MASK);

    if ((LPM_PowerModeLPIdle == power_mode) || (LPM_PowerModeLowPowerRun == power_mode)) {
        FLEXSPI->DLLCR[0] = FLEXSPI_DLLCR_OVRDEN(1) | FLEXSPI_DLLCR_OVRDVAL(19);
    }
    else {
        FLEXSPI->DLLCR[0] = 0x79;
    }
    FLEXSPI->MCR0 &= ~FLEXSPI_MCR0_MDIS_MASK;
    FLEXSPI->MCR0 |= FLEXSPI_MCR0_SWRESET_MASK;
    while (FLEXSPI->MCR0 & FLEXSPI_MCR0_SWRESET_MASK) {}
    while (!((FLEXSPI->STS0 & FLEXSPI_STS0_ARBIDLE_MASK) && (FLEXSPI->STS0 & FLEXSPI_STS0_SEQIDLE_MASK))) {
        ;
    }

    /* Take some delay */
    LPM_DELAY(40);
}

void LPM_RestoreFlexspiClock(void)
{
    while (!((FLEXSPI->STS0 & FLEXSPI_STS0_ARBIDLE_MASK) && (FLEXSPI->STS0 & FLEXSPI_STS0_SEQIDLE_MASK))) {
        ;
    }
    FLEXSPI->MCR0 |= FLEXSPI_MCR0_MDIS_MASK;
    /* Disable clock gate of flexspi. */
    CCM->CCGR6 &= (~CCM_CCGR6_CG5_MASK);

    /* PLL3 PFD0 will be used as flexspi clock. */
    CLOCK_SET_MUX((uint32_t)kCLOCK_FlexspiMux, 0x3);
    /* Set divider for flexspi clock root 0. */
    CLOCK_SET_DIV((uint32_t)kCLOCK_FlexspiDiv, 0x0);

    /* Enable clock gate of flexspi. */
    CCM->CCGR6 |= (CCM_CCGR6_CG5_MASK);
    FLEXSPI->DLLCR[0] = s_DllBackupValue;
    FLEXSPI->MCR0 &= ~FLEXSPI_MCR0_MDIS_MASK;
    FLEXSPI->MCR0 |= FLEXSPI_MCR0_SWRESET_MASK;
    while (FLEXSPI->MCR0 & FLEXSPI_MCR0_SWRESET_MASK) {}
    while (!((FLEXSPI->STS0 & FLEXSPI_STS0_ARBIDLE_MASK) && (FLEXSPI->STS0 & FLEXSPI_STS0_SEQIDLE_MASK))) {
        ;
    }
    /* Take some delay */
    LPM_DELAY(40);
}

void LPM_SwitchBandgap(void)
{
    /* Switch bandgap */
    PMU->MISC0_SET              = 0x00000004;
    XTALOSC24M->LOWPWR_CTRL_SET = XTALOSC24M_LOWPWR_CTRL_LPBG_SEL_MASK;
    PMU->MISC0_SET              = CCM_ANALOG_MISC0_REFTOP_PWD_MASK;

    /* Wait CCM operation finishes */
    CLOCK_CCM_HANDSHAKE_WAIT();
    /* Take some delay */
    LPM_DELAY(40);
}

void LPM_RestoreBandgap(void)
{
    /* Restore bandgap */
    /* Turn on regular bandgap and wait for stable */
    CCM_ANALOG->MISC0_CLR = CCM_ANALOG_MISC0_REFTOP_PWD_MASK;
    while ((CCM_ANALOG->MISC0 & CCM_ANALOG_MISC0_REFTOP_VBGUP_MASK) == 0) {}
    /* Low power band gap disable */
    XTALOSC24M->LOWPWR_CTRL_CLR = XTALOSC24M_LOWPWR_CTRL_LPBG_SEL_MASK;
    PMU->MISC0_CLR              = 0x00000004;

    /* Wait CCM operation finishes */
    CLOCK_CCM_HANDSHAKE_WAIT();
    /* Take some delay */
    LPM_DELAY(40);
}

void LPM_DisablePLLs(lpm_power_mode_t power_mode)
{
    /* Application shouldn't rely on PLL in low power mode,
     * gate all PLL and PFD now */
    if (LPM_PowerModeSuspend == power_mode) {
        return;
    }

    s_clockContext.pfd480   = CCM_ANALOG->PFD_480;
    s_clockContext.pfd528   = CCM_ANALOG->PFD_528;
    s_clockContext.pllSys   = CCM_ANALOG->PLL_SYS;
    s_clockContext.pllUsb1  = CCM_ANALOG->PLL_USB1;
    s_clockContext.pllUsb2  = CCM_ANALOG->PLL_USB2;
    s_clockContext.pllAudio = CCM_ANALOG->PLL_AUDIO;
    s_clockContext.pllVideo = CCM_ANALOG->PLL_VIDEO;
    s_clockContext.pllEnet  = CCM_ANALOG->PLL_ENET;
    s_clockContext.pllArm_loopdiv =
        (CCM_ANALOG->PLL_ARM & CCM_ANALOG_PLL_ARM_DIV_SELECT_MASK) >> CCM_ANALOG_PLL_ARM_DIV_SELECT_SHIFT;
    s_clockContext.pllArm       = CCM_ANALOG->PLL_ARM;
    s_clockContext.periphSel    = CLOCK_GetMux(kCLOCK_PeriphMux);
    s_clockContext.ipgDiv       = CLOCK_GetDiv(kCLOCK_IpgDiv);
    s_clockContext.ahbDiv       = CLOCK_GetDiv(kCLOCK_AhbDiv);
    s_clockContext.perSel       = CLOCK_GetMux(kCLOCK_PerclkMux);
    s_clockContext.perDiv       = CLOCK_GetDiv(kCLOCK_PerclkDiv);
    s_clockContext.preperiphSel = CLOCK_GetMux(kCLOCK_PrePeriphMux);
    s_clockContext.armDiv       = CLOCK_GetDiv(kCLOCK_ArmDiv);
    s_clockContext.is_valid     = 1;

    /* When need to close PLL, we need to use bypass clock first and then power it down. */
    /* Power off SYS PLL */
    CCM_ANALOG->PLL_SYS_SET = CCM_ANALOG_PLL_SYS_BYPASS_MASK;
    CCM_ANALOG->PLL_SYS_SET = CCM_ANALOG_PLL_SYS_POWERDOWN_MASK;
    // CCM_ANALOG->PFD_528_SET = CCM_ANALOG_PFD_528_PFD2_CLKGATE_MASK;

#if !(defined(XIP_EXTERNAL_FLASH) && (XIP_EXTERNAL_FLASH == 1))
    /* If XIP in hyper flash, should switch to ARM PLL before disble USB1 PLL */
    /* Power off USB1 PLL */
    CCM_ANALOG->PLL_USB1_SET = CCM_ANALOG_PLL_USB1_BYPASS_MASK;
    CCM_ANALOG->PLL_USB1_CLR = CCM_ANALOG_PLL_USB1_POWER_MASK;
    CCM_ANALOG->PFD_480_SET  = CCM_ANALOG_PFD_480_PFD2_CLKGATE_MASK;
#endif
    /* Power off USB2 PLL */
    CCM_ANALOG->PLL_USB2_SET = CCM_ANALOG_PLL_USB2_BYPASS_MASK;
    CCM_ANALOG->PLL_USB2_CLR = CCM_ANALOG_PLL_USB2_POWER_MASK;
    /* Power off AUDIO PLL */
    CCM_ANALOG->PLL_AUDIO_SET = CCM_ANALOG_PLL_AUDIO_BYPASS_MASK;
    CCM_ANALOG->PLL_AUDIO_SET = CCM_ANALOG_PLL_AUDIO_POWERDOWN_MASK;
    /* Power off VIDEO PLL */
    CCM_ANALOG->PLL_VIDEO_SET = CCM_ANALOG_PLL_VIDEO_BYPASS_MASK;
    CCM_ANALOG->PLL_VIDEO_SET = CCM_ANALOG_PLL_VIDEO_POWERDOWN_MASK;
    /* Power off ENET PLL */
    CCM_ANALOG->PLL_ENET_SET = CCM_ANALOG_PLL_ENET_BYPASS_MASK;
    CCM_ANALOG->PLL_ENET_SET = CCM_ANALOG_PLL_ENET_POWERDOWN_MASK;

    if ((LPM_PowerModeSysIdle == power_mode) || (LPM_PowerModeLowSpeedRun == power_mode)) {
        CLOCK_SetMux(kCLOCK_PeriphClk2Mux, 1);
        CLOCK_SetDiv(kCLOCK_PeriphClk2Div, 0);
        CLOCK_SetMux(kCLOCK_PeriphMux, 1);
        CLOCK_SetDiv(kCLOCK_IpgDiv, 0);
        CLOCK_SetDiv(kCLOCK_AhbDiv, 0);

        /*ARM PLL as clksource*/
        /* 24 * 88 / 2 / 8 = 132MHz */
        CCM_ANALOG->PLL_ARM_CLR = CCM_ANALOG_PLL_ARM_POWERDOWN_MASK;
        CCM_ANALOG->PLL_ARM_SET = CCM_ANALOG_PLL_ARM_ENABLE_MASK | CCM_ANALOG_PLL_ARM_BYPASS_MASK;
        CCM_ANALOG->PLL_ARM_CLR = CCM_ANALOG_PLL_ARM_DIV_SELECT_MASK;
        CCM_ANALOG->PLL_ARM_SET = CCM_ANALOG_PLL_ARM_DIV_SELECT(88);
        CLOCK_SetDiv(kCLOCK_ArmDiv, 7);
        CCM_ANALOG->PLL_ARM_CLR = CCM_ANALOG_PLL_ARM_BYPASS_MASK;

        /*Select ARM_PLL for pre_periph_clock */
        CLOCK_SetMux(kCLOCK_PrePeriphMux, 3);
        CLOCK_SetMux(kCLOCK_PeriphMux, 0);

        if (LPM_PowerModeLowSpeedRun == power_mode) {
            /* SET AHB to 132MHz, IPG to 33MHz */
            CLOCK_SetDiv(kCLOCK_IpgDiv, 3);
            CLOCK_SetDiv(kCLOCK_AhbDiv, 0);
        }
        else {
            /* SET AHB, IPG to 33MHz */
            CLOCK_SetDiv(kCLOCK_IpgDiv, 0);
            CLOCK_SetDiv(kCLOCK_AhbDiv, 3);
        }
        /*Set PERCLK to 33MHz*/
        CLOCK_SetMux(kCLOCK_PerclkMux, 0);
        CLOCK_SetDiv(kCLOCK_PerclkDiv, 0);
    }
    else if ((LPM_PowerModeLPIdle == power_mode) || (LPM_PowerModeLowPowerRun == power_mode)) {
        CLOCK_SetMux(kCLOCK_PeriphClk2Mux, 1);
        CLOCK_SetDiv(kCLOCK_PeriphClk2Div, 0);
        CLOCK_SetMux(kCLOCK_PeriphMux, 1);
        CLOCK_SetDiv(kCLOCK_IpgDiv, 0);
        CLOCK_SetDiv(kCLOCK_AhbDiv, 0);

        /*ARM PLL as clksource*/
        CCM_ANALOG->PLL_ARM |= CCM_ANALOG_PLL_ARM_ENABLE_MASK | CCM_ANALOG_PLL_ARM_BYPASS_MASK;
        /* Power off ARM PLL */
        CCM_ANALOG->PLL_ARM_SET = CCM_ANALOG_PLL_ARM_POWERDOWN_MASK;

        /*Select ARM_PLL for pre_periph_clock */
        CLOCK_SetDiv(kCLOCK_ArmDiv, 0x0);
        CLOCK_SetMux(kCLOCK_PrePeriphMux, 0x3);
        CLOCK_SetMux(kCLOCK_PeriphMux, 0x0);

        if (LPM_PowerModeLowPowerRun == power_mode) {
            /* SET AHB to 24MHz, IPG to 12MHz */
            CLOCK_SetDiv(kCLOCK_IpgDiv, 1);
            CLOCK_SetDiv(kCLOCK_AhbDiv, 0);
        }
        else {
            /* SET AHB, IPG to 12MHz */
            CLOCK_SetDiv(kCLOCK_IpgDiv, 0);
            CLOCK_SetDiv(kCLOCK_AhbDiv, 1);
        }
        /*Set PERCLK to 12Mhz*/
        CLOCK_SetMux(kCLOCK_PerclkMux, 0x0);
        CLOCK_SetDiv(kCLOCK_PerclkDiv, 0x0);
    }
    else {
        /* Direct return from RUN and Suspend */
        return;
    }

#if defined(XIP_EXTERNAL_FLASH) && (XIP_EXTERNAL_FLASH == 1)
    LPM_EnterCritical();
    LPM_SwitchFlexspiClock(power_mode);
    LPM_ExitCritical();

    /* Power off USB1 PLL */
    CCM_ANALOG->PLL_USB1_SET = CCM_ANALOG_PLL_USB1_BYPASS_MASK;
    CCM_ANALOG->PLL_USB1_CLR = CCM_ANALOG_PLL_USB1_POWER_MASK;
    CCM_ANALOG->PFD_480_SET  = CCM_ANALOG_PFD_480_PFD2_CLKGATE_MASK;
#endif

    CLOCK_DeinitUsb1Pfd(kCLOCK_Pfd0);
    CLOCK_DeinitUsb1Pfd(kCLOCK_Pfd1);
    CLOCK_DeinitUsb1Pfd(kCLOCK_Pfd2);
    CLOCK_DeinitUsb1Pfd(kCLOCK_Pfd3);

    CLOCK_DeinitSysPfd(kCLOCK_Pfd0);
    CLOCK_DeinitSysPfd(kCLOCK_Pfd1);
    //    CLOCK_DeinitSysPfd(kCLOCK_Pfd2);
    CLOCK_DeinitSysPfd(kCLOCK_Pfd3);
}

void LPM_RestorePLLs(lpm_power_mode_t power_mode)
{
    if (s_clockContext.is_valid) {
        /* Restore USB1 PLL */
        CCM_ANALOG->PLL_USB1_SET = CCM_ANALOG_PLL_USB1_BYPASS_MASK;
        CCM_ANALOG->PLL_USB1 =
            (s_clockContext.pllUsb1 & (~(CCM_ANALOG_PLL_USB1_POWER_MASK))) | CCM_ANALOG_PLL_USB1_BYPASS_MASK;
        CCM_ANALOG->PLL_USB1_CLR = CCM_ANALOG_PLL_USB1_BYPASS_MASK;
        CCM_ANALOG->PLL_USB1_SET = CCM_ANALOG_PLL_USB1_POWER_MASK;

        /* Restore USB1 PLL PFD */
        CCM_ANALOG->PFD_480     = s_clockContext.pfd480;
        CCM_ANALOG->PFD_480_CLR = CCM_ANALOG_PFD_480_PFD0_CLKGATE_MASK;

        if ((CCM_ANALOG->PLL_USB1 & CCM_ANALOG_PLL_USB1_POWER_MASK) != 0) {
            while ((CCM_ANALOG->PLL_USB1 & CCM_ANALOG_PLL_USB1_LOCK_MASK) == 0) {}
        }
        /* Wait CCM operation finishes */
        while (CCM->CDHIPR != 0) {}
#if defined(XIP_EXTERNAL_FLASH) && (XIP_EXTERNAL_FLASH == 1)
        LPM_EnterCritical();
        LPM_RestoreFlexspiClock();
        LPM_ExitCritical();
#endif
    }
#if defined(XIP_EXTERNAL_FLASH) && (XIP_EXTERNAL_FLASH == 1)
    /* If reset from suspend, need to switch back to USB1 PLL as the clock source */
    else if (is_suspend_reset) {
        const clock_usb_pll_config_t g_ccmConfigUsbPll = {.loopDivider = 0U};
        s_DllBackupValue                               = 0x79;
        is_suspend_reset                               = 0;
        CLOCK_InitUsb1Pll(&g_ccmConfigUsbPll); /* Configure USB1 PLL to 480M */
        CLOCK_InitUsb1Pfd(kCLOCK_Pfd0, 26);

        LPM_EnterCritical();
        LPM_RestoreFlexspiClock();
        LPM_ExitCritical();
        return;
    }
#endif
    else {
        return;
    }

    CLOCK_SetMux(kCLOCK_PeriphClk2Mux, 1);
    CLOCK_SetDiv(kCLOCK_PeriphClk2Div, 0);
    CLOCK_SetMux(kCLOCK_PeriphMux, 1);
    CLOCK_SetDiv(kCLOCK_IpgDiv, 0x1);
    CLOCK_SetDiv(kCLOCK_AhbDiv, 0x0);

    /* ARM PLL as clksource*/
    CCM_ANALOG->PLL_ARM_CLR = CCM_ANALOG_PLL_ARM_POWERDOWN_MASK;
    CCM_ANALOG->PLL_ARM_SET = CCM_ANALOG_PLL_ARM_ENABLE_MASK | CCM_ANALOG_PLL_ARM_BYPASS_MASK;

    CLOCK_SetDiv(kCLOCK_ArmDiv, s_clockContext.armDiv);
    CCM_ANALOG->PLL_ARM_CLR = CCM_ANALOG_PLL_ARM_DIV_SELECT_MASK;
    CCM_ANALOG->PLL_ARM_SET = CCM_ANALOG_PLL_ARM_DIV_SELECT(s_clockContext.pllArm_loopdiv);
    if ((s_clockContext.pllArm & CCM_ANALOG_PLL_ARM_BYPASS_MASK) == 0) {
        while ((CCM_ANALOG->PLL_ARM & CCM_ANALOG_PLL_ARM_LOCK_MASK) == 0) {}
    }
    if ((s_clockContext.pllArm & CCM_ANALOG_PLL_ARM_BYPASS_MASK) == 0) {
        CCM_ANALOG->PLL_ARM_CLR = CCM_ANALOG_PLL_ARM_BYPASS_MASK;
    }

    /* Restore AHB and IPG div */
    CCM->CBCDR = (CCM->CBCDR & ~(CCM_CBCDR_AHB_PODF_MASK | CCM_CBCDR_IPG_PODF_MASK | CCM_CBCDR_PERIPH_CLK_SEL_MASK)) |
                 CCM_CBCDR_AHB_PODF(s_clockContext.ahbDiv) | CCM_CBCDR_IPG_PODF(s_clockContext.ipgDiv) |
                 CCM_CBCDR_PERIPH_CLK_SEL(s_clockContext.periphSel);

    /* Restore Periphral clock */
    CCM->CSCMR1 = (CCM->CSCMR1 & ~CCM_CSCMR1_PERCLK_PODF_MASK) | CCM_CSCMR1_PERCLK_PODF(s_clockContext.perDiv) |
                  CCM_CSCMR1_PERCLK_CLK_SEL(s_clockContext.perSel);

    /* Switch clocks back */
    CCM->CBCMR =
        (CCM->CBCMR & ~CCM_CBCMR_PRE_PERIPH_CLK_SEL_MASK) | CCM_CBCMR_PRE_PERIPH_CLK_SEL(s_clockContext.preperiphSel);

    /* Wait CCM operation finishes */
    while (CCM->CDHIPR != 0) {}

    /* When need to enable PLL, we need to use bypass clock first and then switch pll back. */
    /* Power on SYS PLL and wait for locked */
    CCM_ANALOG->PLL_SYS_SET = CCM_ANALOG_PLL_SYS_BYPASS_MASK;
    CCM_ANALOG->PLL_SYS_CLR = CCM_ANALOG_PLL_SYS_POWERDOWN_MASK;
    CCM_ANALOG->PLL_SYS     = s_clockContext.pllSys;
    if ((CCM_ANALOG->PLL_SYS & CCM_ANALOG_PLL_SYS_POWERDOWN_MASK) == 0) {
        while ((CCM_ANALOG->PLL_SYS & CCM_ANALOG_PLL_SYS_LOCK_MASK) == 0) {}
    }
    // CCM_ANALOG->PFD_528_CLR = CCM_ANALOG_PFD_528_PFD2_CLKGATE_MASK;

    /* Restore USB2 PLL */
    CCM_ANALOG->PLL_USB2_SET = CCM_ANALOG_PLL_USB2_BYPASS_MASK;
    CCM_ANALOG->PLL_USB2_SET = CCM_ANALOG_PLL_USB2_POWER_MASK;
    CCM_ANALOG->PLL_USB2     = s_clockContext.pllUsb2;
    if ((CCM_ANALOG->PLL_USB2 & CCM_ANALOG_PLL_USB2_POWER_MASK) != 0) {
        while ((CCM_ANALOG->PLL_USB2 & CCM_ANALOG_PLL_USB2_LOCK_MASK) == 0) {}
    }

    /* Restore AUDIO PLL */
    CCM_ANALOG->PLL_AUDIO_SET = CCM_ANALOG_PLL_AUDIO_BYPASS_MASK;
    CCM_ANALOG->PLL_AUDIO_CLR = CCM_ANALOG_PLL_AUDIO_POWERDOWN_MASK;
    CCM_ANALOG->PLL_AUDIO     = s_clockContext.pllAudio;
    if ((CCM_ANALOG->PLL_AUDIO & CCM_ANALOG_PLL_AUDIO_POWERDOWN_MASK) == 0) {
        while ((CCM_ANALOG->PLL_AUDIO & CCM_ANALOG_PLL_AUDIO_LOCK_MASK) == 0) {}
    }

    /* Restore VIDEO PLL */
    CCM_ANALOG->PLL_VIDEO_SET = CCM_ANALOG_PLL_VIDEO_BYPASS_MASK;
    CCM_ANALOG->PLL_VIDEO_CLR = CCM_ANALOG_PLL_VIDEO_POWERDOWN_MASK;
    CCM_ANALOG->PLL_VIDEO     = s_clockContext.pllVideo;
    if ((CCM_ANALOG->PLL_VIDEO & CCM_ANALOG_PLL_VIDEO_POWERDOWN_MASK) == 0) {
        while ((CCM_ANALOG->PLL_VIDEO & CCM_ANALOG_PLL_VIDEO_LOCK_MASK) == 0) {}
    }

    /* Restore ENET PLL */
    CCM_ANALOG->PLL_ENET_SET = CCM_ANALOG_PLL_ENET_BYPASS_MASK;
    CCM_ANALOG->PLL_ENET_SET = CCM_ANALOG_PLL_ENET_POWERDOWN_MASK;
    CCM_ANALOG->PLL_ENET     = s_clockContext.pllEnet;
    if ((CCM_ANALOG->PLL_ENET & CCM_ANALOG_PLL_ENET_POWERDOWN_MASK) == 0) {
        while ((CCM_ANALOG->PLL_ENET & CCM_ANALOG_PLL_ENET_LOCK_MASK) == 0) {}
    }

    /* Restore SYS PLL PFD */
    CCM_ANALOG->PFD_528 = s_clockContext.pfd528;

    s_clockContext.is_valid = 0;
}

static void LPM_DisableRbcBypass(void)
{
    uint32_t gpcIMR[LPM_GPC_IMR_NUM];
    uint32_t i;

    /* Mask all GPC interrupts before disabling the RBC counters */
    for (i = 0; i < LPM_GPC_IMR_NUM; i++) {
        gpcIMR[i]   = GPC->IMR[i];
        GPC->IMR[i] = 0xFFFFFFFFU;
    }

    /* Disable the RBC bypass counter */
    CCM->CCR &= ~CCM_CCR_RBC_EN_MASK;
    CCM->CCR &= ~CCM_CCR_REG_BYPASS_COUNT_MASK;

    /* Now delay for 2 CKIL cycles (61usec). ARM is at 528MHz at this point
     * so a short loop should be enough.
     */
    for (i = 0; i < 528 * 22; i++) {
        __NOP();
    }

    /* Recover all the GPC interrupts. */
    for (i = 0; i < LPM_GPC_IMR_NUM; i++) {
        GPC->IMR[i] = gpcIMR[i];
    }
}

static void LPM_SystemWait(void)
{
    LPM_DisablePLLs(APP_GetLPMPowerMode());

    /* Adjust voltage to 1.15V */
    DCDC_AdjustTargetVoltage(DCDC, 0xE, 0x1);
}

static void LPM_SystemRestoreWait(void)
{
    /* Restore voltage to 1.25V */
    DCDC_AdjustTargetVoltage(DCDC, 0x12, 0x1);

    LPM_RestorePLLs(LPM_PowerModeSysIdle);
}

static void LPM_SystemIdle(void)
{
    /* Switch DCDC to use DCDC internal OSC */
    DCDC_SetClockSource(DCDC, kDCDC_ClockInternalOsc);

    /* Power down USBPHY */
    USBPHY1->CTRL = 0xFFFFFFFF;
    USBPHY2->CTRL = 0xFFFFFFFF;

    LPM_DisablePLLs(APP_GetLPMPowerMode());

    /* Enable weak 2P5 and turn off regular 2P5 */
    PMU->REG_2P5 |= PMU_REG_2P5_ENABLE_WEAK_LINREG_MASK;
    PMU->REG_2P5 &= ~PMU_REG_2P5_ENABLE_LINREG_MASK;
    /* Enable weak 1P1 and turn off regular 1P1 */
    PMU->REG_1P1 |= PMU_REG_1P1_ENABLE_WEAK_LINREG_MASK;
    PMU->REG_1P1 &= ~PMU_REG_1P1_ENABLE_LINREG_MASK;

    LPM_EnterCritical();
    LPM_SwitchToRcOSC();
    LPM_ExitCritical();

    /* Lower OSC current by 37.5% */
    CCM_ANALOG->MISC0_SET = CCM_ANALOG_MISC0_OSC_I_MASK;
    /* Enable FET ODRIVE */
    PMU->REG_CORE_SET = PMU_REG_CORE_FET_ODRIVE_MASK;
    /* Disconnect vdd_high_in and connect vdd_snvs_in */
    CCM_ANALOG->MISC0_SET = CCM_ANALOG_MISC0_DISCON_HIGH_SNVS_MASK;

    DCDC_AdjustTargetVoltage(DCDC, 0x6, 0x1);

    LPM_EnterCritical();
    LPM_SwitchBandgap();
    LPM_ExitCritical();

    /* RBC = 0; Enable COSC, OSC COUNT = 0xAF */
    CCM->CCR = (CCM_CCR_COSC_EN_MASK | CCM_CCR_OSCNT(0xAF));

    s_SystemIdleFlag = 1;
}

void LPM_SystemRestoreIdle(void)
{
    DCDC_AdjustTargetVoltage(DCDC, 0x12, 0x1);

    /* Switch DCDC to use DCDC internal OSC */
    DCDC_SetClockSource(DCDC, kDCDC_ClockExternalOsc);

    /* Disconnect vdd_snvs_in and connect vdd_high_in */
    CCM_ANALOG->MISC0_CLR = CCM_ANALOG_MISC0_DISCON_HIGH_SNVS_MASK;
    /* Increase OSC current to normal */
    CCM_ANALOG->MISC0_CLR = CCM_ANALOG_MISC0_OSC_I_MASK;
    /* Clear FET ODRIVE */
    PMU->REG_CORE_CLR = PMU_REG_CORE_FET_ODRIVE_MASK;

    LPM_EnterCritical();
    LPM_SwitchToXtalOSC();
    LPM_RestoreBandgap();
    LPM_ExitCritical();

    /* Enable regular 2P5 and wait for stable */
    PMU->REG_2P5_SET = PMU_REG_2P5_ENABLE_LINREG_MASK;
    while ((PMU->REG_2P5 & PMU_REG_2P5_OK_VDD2P5_MASK) == 0) {}
    /* Turn off weak 2P5 */
    PMU->REG_2P5_CLR = PMU_REG_2P5_ENABLE_WEAK_LINREG_MASK;

    /* Enable regular 1P1 and wait for stable */
    PMU->REG_1P1_SET = PMU_REG_1P1_ENABLE_LINREG_MASK;
    while ((PMU->REG_1P1 & PMU_REG_1P1_OK_VDD1P1_MASK) == 0) {}
    /* Turn off weak 1P1 */
    PMU->REG_1P1_CLR = PMU_REG_1P1_ENABLE_WEAK_LINREG_MASK;

    /* If s_SkipRestorePLLs is set, skip the process to restore the PLLs. */
    if (!s_SkipRestorePLLs) {
        LPM_RestorePLLs(LPM_PowerModeLPIdle);
    }
    s_SkipRestorePLLs = 0;
    s_SystemIdleFlag  = 0;
}

static void LPM_SystemDsm()
{
    uint32_t i;
    uint32_t gpcIMR[LPM_GPC_IMR_NUM];

    BOARD_SetLPClockGate();

    /* Turn off FlexRAM1 */
    PGC->MEGA_CTRL |= PGC_MEGA_CTRL_PCR_MASK;
    /* Turn off FlexRAM0 */
    GPC->CNTR |= GPC_CNTR_PDRAM0_PGE_MASK;

    /* Clean and disable data cache to make sure context is saved into DDR */
    SCB_CleanDCache();
    SCB_DisableDCache();

    /* Adjust LP voltage to 0.925V */
    DCDC_AdjustTargetVoltage(DCDC, 0x12, 0x1);
    /* Switch DCDC to use DCDC internal OSC */
    DCDC_SetClockSource(DCDC, kDCDC_ClockInternalOsc);

    /* Power down USBPHY */
    USBPHY1->CTRL = 0xFFFFFFFF;
    USBPHY2->CTRL = 0xFFFFFFFF;

    /* Power down CPU when requested */
    PGC->CPU_CTRL = PGC_CPU_CTRL_PCR_MASK;

    PMU->REG_CORE_SET = PMU_REG_CORE_FET_ODRIVE_MASK;
    /* Disconnect vdd_high_in and connect vdd_snvs_in */
    CCM_ANALOG->MISC0_SET = CCM_ANALOG_MISC0_DISCON_HIGH_SNVS_MASK;
    /* STOP_MODE config, turn off all analog except RTC in stop mode */
    CCM_ANALOG->MISC0_CLR = CCM_ANALOG_MISC0_STOP_MODE_CONFIG_MASK;

    /* Mask all GPC interrupts before enabling the RBC counters to
       avoid the counter starting too early if an interupt is already
       pending.
    */
    for (i = 0; i < LPM_GPC_IMR_NUM; i++) {
        gpcIMR[i]   = GPC->IMR[i];
        GPC->IMR[i] = 0xFFFFFFFFU;
    }

    /*
     * ERR006223: CCM: Failure to resuem from wait/stop mode with power gating
     *   Configure REG_BYPASS_COUNTER to 2
     *   Enable the RBC bypass counter here to hold off the interrupts. RBC counter
     *  needs to be no less than 2.
     */
    CCM->CCR = (CCM->CCR & ~CCM_CCR_REG_BYPASS_COUNT_MASK) | CCM_CCR_REG_BYPASS_COUNT(2);
    CCM->CCR |= (CCM_CCR_RBC_EN_MASK | CCM_CCR_COSC_EN_MASK | CCM_CCR_OSCNT(0xAF));

    /* Recover all the GPC interrupts. */
    for (i = 0; i < LPM_GPC_IMR_NUM; i++) {
        GPC->IMR[i] = gpcIMR[i];
    }

    /* Now delay for a short while (3usec) ARM is at 528MHz at this point
     * so a short loop should be enough. This delay is required to ensure that
     * the RBC counter can start counting in case an interrupt is already pending
     * or in case an interrupt arrives just as ARM is about to assert DSM_request.
     */
    for (i = 0; i < 22 * 24; i++) {
        __NOP();
    }
}

void LPM_SystemRestoreDsm(void)
{
    /* Enable clock of ARM platform memories when entering LPM */
    CCM->CGPR |= CCM_CGPR_INT_MEM_CLK_LPM_MASK;

    /* Clear ARM power gate setting */
    PGC->CPU_CTRL &= ~PGC_CPU_CTRL_PCR_MASK;
    /* Keep megamix power on when STOP */
    PGC->MEGA_CTRL &= ~PGC_MEGA_CTRL_PCR_MASK;
    /* Clear FET ODRIVE */
    PMU->REG_CORE_CLR = PMU_REG_CORE_FET_ODRIVE_MASK;
    /* Disconnect vdd_snvs_in and connect vdd_high_in */
    CCM_ANALOG->MISC0_CLR = CCM_ANALOG_MISC0_DISCON_HIGH_SNVS_MASK;
}

void LPM_SystemResumeDsm(void)
{
    uint32_t clpcr;

    clpcr = CCM->CLPCR & (~(CCM_CLPCR_LPM_MASK | CCM_CLPCR_ARM_CLK_DIS_ON_LPM_MASK));

    GPC->CNTR &= ~GPC_CNTR_PDRAM0_PGE_MASK;

    /* Clear resume entry */
    SRC->GPR[0] = 0U;
    SRC->GPR[1] = 0U;

    LPM_SystemRestoreDsm();

    /* RBC bypass enabled in LPM_SystemDsm */
    LPM_DisableRbcBypass();
    LPM_SetClockMode(kCLOCK_ModeRun, clpcr);

    // LPM_DisableWakeupSource(vPortGetGptIrqn());
}

void LPM_SystemOverRunRecovery(void)
{
    /* Switch DCDC to use DCDC internal OSC */
    DCDC_SetClockSource(DCDC, kDCDC_ClockExternalOsc);

    /* Disconnect vdd_snvs_in and connect vdd_high_in */
    CCM_ANALOG->MISC0_CLR = CCM_ANALOG_MISC0_DISCON_HIGH_SNVS_MASK;
    /* Increase OSC current to normal */
    CCM_ANALOG->MISC0_CLR = CCM_ANALOG_MISC0_OSC_I_MASK;
    /* Restore XTAL-OSC and enable detector */
    CLOCK_InitExternalClk(false);

    /* Switch to XTAL-OSC */
    LPM_EnterCritical();
    LPM_SwitchToXtalOSC();
    LPM_RestoreBandgap();
    LPM_ExitCritical();

    /* Enable regular 2P5 and wait for stable */
    PMU->REG_2P5_SET = PMU_REG_2P5_ENABLE_LINREG_MASK;
    while ((PMU->REG_2P5 & PMU_REG_2P5_OK_VDD2P5_MASK) == 0) {}
    /* Turn off weak 2P5 */
    PMU->REG_2P5_CLR = PMU_REG_2P5_ENABLE_WEAK_LINREG_MASK;

    /* Enable regular 1P1 and wait for stable */
    PMU->REG_1P1_SET = PMU_REG_1P1_ENABLE_LINREG_MASK;
    while ((PMU->REG_1P1 & PMU_REG_1P1_OK_VDD1P1_MASK) == 0) {}
    /* Turn off weak 1P1 */
    PMU->REG_1P1_CLR = PMU_REG_1P1_ENABLE_WEAK_LINREG_MASK;

    LPM_RestorePLLs(LPM_PowerModeOverRun);

    s_SystemIdleFlag = 0;
}

void LPM_SystemOverRun(void)
{
    // change the DCDC_LP to 1.25V first
    DCDC_AdjustTargetVoltage(DCDC, 0x12, 0x1);

    LPM_SystemOverRunRecovery();

    CLOCK_SetMux(kCLOCK_PeriphClk2Mux, 1);
    CLOCK_SetDiv(kCLOCK_PeriphClk2Div, 0);
    CLOCK_SetMux(kCLOCK_PeriphMux, 1);
    CLOCK_SetDiv(kCLOCK_IpgDiv, 0);
    CLOCK_SetDiv(kCLOCK_AhbDiv, 0);

    /*ARM PLL as clksource*/
    /* 24 * 100 / 2 / 2 = 600MHz */
    CCM_ANALOG->PLL_ARM_CLR = CCM_ANALOG_PLL_ARM_POWERDOWN_MASK;
    CCM_ANALOG->PLL_ARM_SET = CCM_ANALOG_PLL_ARM_ENABLE_MASK | CCM_ANALOG_PLL_ARM_BYPASS_MASK;
    CLOCK_SetDiv(kCLOCK_ArmDiv, 0x1);
    CCM_ANALOG->PLL_ARM_CLR = CCM_ANALOG_PLL_ARM_DIV_SELECT_MASK;
    CCM_ANALOG->PLL_ARM_SET = CCM_ANALOG_PLL_ARM_DIV_SELECT(100);
    CCM_ANALOG->PLL_ARM_CLR = CCM_ANALOG_PLL_ARM_BYPASS_MASK;

    CLOCK_SetMux(kCLOCK_PrePeriphMux, 0x3);
    CLOCK_SetDiv(kCLOCK_IpgDiv, 0x3);
    CLOCK_SetDiv(kCLOCK_AhbDiv, 0);
    CLOCK_SetMux(kCLOCK_PeriphMux, 0x0);

    CLOCK_SetMux(kCLOCK_PerclkMux, 1);
    CLOCK_SetDiv(kCLOCK_PerclkDiv, 0);
}

void LPM_SystemFullRun(void)
{
    /* Back to Over Run first */
    LPM_SystemOverRun();

    CLOCK_SetMux(kCLOCK_PeriphClk2Mux, 1);
    CLOCK_SetDiv(kCLOCK_PeriphClk2Div, 0);
    CLOCK_SetMux(kCLOCK_PeriphMux, 1);
    CLOCK_SetDiv(kCLOCK_IpgDiv, 0x1);
    CLOCK_SetDiv(kCLOCK_AhbDiv, 0x0);

    /* ARM PLL as clksource*/
    /* 24 * 86 / 2 / 2 = 516MHz */
    CCM_ANALOG->PLL_ARM_CLR = CCM_ANALOG_PLL_ARM_POWERDOWN_MASK;
    CCM_ANALOG->PLL_ARM_SET = CCM_ANALOG_PLL_ARM_ENABLE_MASK | CCM_ANALOG_PLL_ARM_BYPASS_MASK;
    CLOCK_SetDiv(kCLOCK_ArmDiv, 0x1);
    CCM_ANALOG->PLL_ARM_CLR = CCM_ANALOG_PLL_ARM_DIV_SELECT_MASK;
    CCM_ANALOG->PLL_ARM_SET = CCM_ANALOG_PLL_ARM_DIV_SELECT(86);
    CCM_ANALOG->PLL_ARM_CLR = CCM_ANALOG_PLL_ARM_BYPASS_MASK;

    /* Select ARM_PLL for pre_periph_clock */
    CLOCK_SetMux(kCLOCK_PrePeriphMux, 3);
    CLOCK_SetDiv(kCLOCK_IpgDiv, 0x3);
    CLOCK_SetDiv(kCLOCK_AhbDiv, 0);
    CLOCK_SetMux(kCLOCK_PeriphMux, 0x0);

    CLOCK_SetMux(kCLOCK_PerclkMux, 1);
    CLOCK_SetDiv(kCLOCK_PerclkDiv, 0);

    /* change the DCDC_LP to 1.15V */
    DCDC_AdjustTargetVoltage(DCDC, 0xE, 0x1);
}

void LPM_SystemLowSpeedRun(void)
{
    /* Back to Over Run first */
    LPM_SystemOverRun();

    LPM_SystemWait();
}

void LPM_SystemLowPowerRun(void)
{
    /* Back to Over Run first */
    LPM_SystemOverRun();

    LPM_SystemIdle();
}

#ifdef FSL_RTOS_FREE_RTOS
#if (configUSE_TICKLESS_IDLE == 1)
void LPM_InitTicklessTimer(void)
{
    gpt_config_t gptConfig;

    /* Init GPT for wakeup as FreeRTOS tell us */
    GPT_GetDefaultConfig(&gptConfig);
    gptConfig.clockSource = kGPT_ClockSource_LowFreq; /* 32K RTC OSC */
    // gptConfig.enableMode = false;                     /* Keep counter when stop */
    gptConfig.enableMode      = true; /* Don't keep counter when stop */
    gptConfig.enableRunInDoze = true;
    /* Initialize GPT module */
    GPT_Init(vPortGetGptBase(), &gptConfig);
    GPT_SetClockDivider(vPortGetGptBase(), 1);

    /* Enable GPT Output Compare1 interrupt */
    GPT_EnableInterrupts(vPortGetGptBase(), kGPT_OutputCompare1InterruptEnable);
    NVIC_SetPriority(vPortGetGptIrqn(), configMAX_SYSCALL_INTERRUPT_PRIORITY + 2);
    EnableIRQ(vPortGetGptIrqn());
}
#endif
#endif

bool LPM_Init(lpm_power_mode_t run_mode)
{
    uint32_t i;
    uint32_t tmp_reg = 0;

    /* Boot ROM did initialize the XTAL, here we only sets external XTAL OSC freq */
    CLOCK_SetXtalFreq(BOARD_XTAL0_CLK_HZ);
    CLOCK_SetRtcXtalFreq(BOARD_XTAL32K_CLK_HZ);

    /* Recover handshaking */
    IOMUXC_GPR->GPR4  = 0x00000000;
    IOMUXC_GPR->GPR7  = 0x00000000;
    IOMUXC_GPR->GPR8  = 0x00000000;
    IOMUXC_GPR->GPR12 = 0x00000000;

    CCM->CCR &= ~CCM_CCR_REG_BYPASS_COUNT_MASK;

    s_targetPowerMode = run_mode;

#ifdef FSL_RTOS_FREE_RTOS
    s_mutex = xSemaphoreCreateMutex();

    if (s_mutex == NULL) {
        return false;
    }

    s_listenerHead = s_listenerTail = NULL;

#if (configUSE_TICKLESS_IDLE == 1)
    LPM_InitTicklessTimer();
#endif
#endif

    if (run_mode > LPM_PowerModeRunEnd) {
        return false;
    }
    s_curMode = run_mode;
    CLOCK_SetMode(kCLOCK_ModeRun);

    CCM->CGPR |= CCM_CGPR_INT_MEM_CLK_LPM_MASK;

    /* Enable RC OSC. It needs at least 4ms to be stable, so self tuning need to be enabled. */
    XTALOSC24M->LOWPWR_CTRL |= XTALOSC24M_LOWPWR_CTRL_RC_OSC_EN_MASK;
    /* Configure RC OSC */
    XTALOSC24M->OSC_CONFIG0 = XTALOSC24M_OSC_CONFIG0_RC_OSC_PROG_CUR(0x4) | XTALOSC24M_OSC_CONFIG0_SET_HYST_MINUS(0x2) |
                              XTALOSC24M_OSC_CONFIG0_RC_OSC_PROG(0xA7) | XTALOSC24M_OSC_CONFIG0_START_MASK |
                              XTALOSC24M_OSC_CONFIG0_ENABLE_MASK;
    XTALOSC24M->OSC_CONFIG1 = XTALOSC24M_OSC_CONFIG1_COUNT_RC_CUR(0x40) | XTALOSC24M_OSC_CONFIG1_COUNT_RC_TRG(0x2DC);
    /* Take some delay */
    LPM_DELAY(40);
    /* Add some hysteresis */
    tmp_reg = XTALOSC24M->OSC_CONFIG0;
    tmp_reg &= ~(XTALOSC24M_OSC_CONFIG0_HYST_PLUS_MASK | XTALOSC24M_OSC_CONFIG0_HYST_MINUS_MASK);
    tmp_reg |= XTALOSC24M_OSC_CONFIG0_HYST_PLUS(3) | XTALOSC24M_OSC_CONFIG0_HYST_MINUS(3);
    XTALOSC24M->OSC_CONFIG0 = tmp_reg;
    /* Set COUNT_1M_TRG */
    tmp_reg = XTALOSC24M->OSC_CONFIG2;
    tmp_reg &= ~XTALOSC24M_OSC_CONFIG2_COUNT_1M_TRG_MASK;
    tmp_reg |= XTALOSC24M_OSC_CONFIG2_COUNT_1M_TRG(0x2d7);
    XTALOSC24M->OSC_CONFIG2 = tmp_reg;
    /* Hardware requires to read OSC_CONFIG0 or OSC_CONFIG1 to make OSC_CONFIG2 write work */
    tmp_reg                 = XTALOSC24M->OSC_CONFIG1;
    XTALOSC24M->OSC_CONFIG1 = tmp_reg;

    s_DllBackupValue = FLEXSPI->DLLCR[0];

    /* ERR007265 */
    IOMUXC_GPR->GPR1 |= IOMUXC_GPR_GPR1_GINT_MASK;

    /* Initialize GPC to mask all IRQs */
    for (i = 0; i < LPM_GPC_IMR_NUM; i++) {
        GPC->IMR[i] = 0xFFFFFFFFU;
    }

    return true;
}

void LPM_Deinit(void)
{
#ifdef FSL_RTOS_FREE_RTOS
    if (s_mutex != NULL) {
        vSemaphoreDelete(s_mutex);
        s_mutex = NULL;
    }
#endif

    /* ERR007265 */
    IOMUXC_GPR->GPR1 &= ~IOMUXC_GPR_GPR1_GINT_MASK;
}

void LPM_EnableWakeupSource(uint32_t irq)
{
    GPC_EnableIRQ(GPC, irq);
}

void LPM_DisableWakeupSource(uint32_t irq)
{
    GPC_DisableIRQ(GPC, irq);
}

bool LPM_SetPowerMode(lpm_power_mode_t mode)
{
#ifdef FSL_RTOS_FREE_RTOS /* Only FreeRTOS supports listener notification */
    lpm_power_mode_listener_t *l1, *l2;
#endif
    bool ret = true;

    if (mode == s_curMode) {
        return ret;
    }

#ifdef FSL_RTOS_FREE_RTOS /* Only FreeRTOS supports listener notification */
                          /* Need to make sure the list of listeners is not changed
                           * when traversing the list.
                           */
    xSemaphoreTake(s_mutex, portMAX_DELAY);

    for (l1 = s_listenerHead; l1 != NULL; l1 = l1->next) {
        if (l1->callback == NULL) {
            continue;
        }

        if (!l1->callback(s_curMode, mode, l1->data)) {
            /* One stakeholder doesn't allow new mode */
            ret = false;
            break;
        }
    }

    if (ret) {
        s_curMode = mode;
    }
    else {
        /* roll back the state change of previous listeners */
        for (l2 = s_listenerHead; l2 != l1; l2 = l2->next) {
            if (l2->callback == NULL) {
                continue;
            }

            l2->callback(mode, s_curMode, l2->data);
        }
    }

    xSemaphoreGive(s_mutex);
#else
    s_curMode = mode;
#endif /* FSL_RTOS_FREE_RTOS */

    return ret;
}

#ifdef FSL_RTOS_FREE_RTOS /* Only FreeRTOS supports listener notification */
void LPM_RegisterPowerListener(lpm_power_mode_callback_t callback, void *data)
{
    lpm_power_mode_listener_t *l = (lpm_power_mode_listener_t *)pvPortMalloc(sizeof(lpm_power_mode_listener_t));
    assert(l);

    l->callback = callback;
    l->data     = data;
    l->next     = NULL;

    xSemaphoreTake(s_mutex, portMAX_DELAY);

    if (s_listenerHead) {
        s_listenerTail->next = l;
        s_listenerTail       = l;
    }
    else {
        s_listenerHead = s_listenerTail = l;
    }

    xSemaphoreGive(s_mutex);
}

void LPM_UnregisterPowerListener(lpm_power_mode_callback_t callback, void *data)
{
    lpm_power_mode_listener_t *l, *p = NULL;

    xSemaphoreTake(s_mutex, portMAX_DELAY);

    for (l = s_listenerHead; l != NULL; l = l->next) {
        if (l->callback == callback && l->data == data) {
            if (p) {
                p->next = l->next;
            }
            else {
                s_listenerHead = l->next;
            }

            if (l->next == NULL) {
                s_listenerTail = p;
            }

            vPortFree(l);
            break;
        }
        p = l;
    }

    xSemaphoreGive(s_mutex);
}
#endif

/************ Internal public API start **************/
#ifdef FSL_RTOS_FREE_RTOS
#if (configUSE_TICKLESS_IDLE == 1)

GPT_Type *vPortGetGptBase(void)
{
    return GPT1;
}

IRQn_Type vPortGetGptIrqn(void)
{
    return GPT1_IRQn;
}

void vPortPRE_SLEEP_PROCESSING(TickType_t timeoutMilliSec)
{
    uint32_t clpcr;

    APP_PowerPreSwitchHook(APP_GetLPMPowerMode());
    LPM_EnableWakeupSource(vPortGetGptIrqn());

    clpcr = CCM->CLPCR & (~(CCM_CLPCR_LPM_MASK | CCM_CLPCR_ARM_CLK_DIS_ON_LPM_MASK));

    switch (APP_GetLPMPowerMode()) {
    case LPM_PowerModeOverRun:
    case LPM_PowerModeFullRun:
    case LPM_PowerModeLowSpeedRun:
    case LPM_PowerModeLowPowerRun:
        break;
    case LPM_PowerModeSysIdle:
        LPM_SetClockMode(kCLOCK_ModeWait,
                         clpcr | CCM_CLPCR_LPM(kCLOCK_ModeWait) | CCM_CLPCR_ARM_CLK_DIS_ON_LPM_MASK |
                             CCM_CLPCR_STBY_COUNT_MASK | 0x1C | 0x08280000);
        BOARD_SetLPClockGate();
        /* If last mode is idle, need to restore idle states. */
        if (s_SystemIdleFlag) {
            s_SkipRestorePLLs = 1;
            LPM_SystemRestoreIdle();
        }

        LPM_SystemWait();
        IOMUXC_GPR->GPR8  = 0xaaaaaaaa;
        IOMUXC_GPR->GPR12 = 0x0000000a;
        break;
    case LPM_PowerModeLPIdle:
        LPM_SetClockMode(kCLOCK_ModeWait,
                         clpcr | CCM_CLPCR_LPM(kCLOCK_ModeWait) | CCM_CLPCR_ARM_CLK_DIS_ON_LPM_MASK |
                             CCM_CLPCR_STBY_COUNT_MASK | 0x1C | 0x08280000);
        BOARD_SetLPClockGate();
        LPM_SystemIdle();
        IOMUXC_GPR->GPR8  = 0xaaaaaaaa;
        IOMUXC_GPR->GPR12 = 0x0000000a;

        /* put SDRAM to self-refresh mode */
        IOMUXC_GPR->GPR4 = (1 << 9U);
        while ((IOMUXC_GPR->GPR4 & (1 << 25U)) != (1 << 25U)) {};
        break;
    case LPM_PowerModeSuspend:
        LPM_SetClockMode(kCLOCK_ModeStop,
                         clpcr | CCM_CLPCR_LPM(kCLOCK_ModeStop) | CCM_CLPCR_VSTBY_MASK | CCM_CLPCR_STBY_COUNT_MASK |
                             CCM_CLPCR_SBYOS_MASK | CCM_CLPCR_ARM_CLK_DIS_ON_LPM_MASK | 0x08280000);
        LPM_SystemDsm();
        // LPM_CoreStateSave();
        break;
    default:
        assert(false);
        break;
    }
}

void vPortPOST_SLEEP_PROCESSING(TickType_t timeoutMilliSec)
{
    uint32_t clpcr;

    clpcr = CCM->CLPCR & (~(CCM_CLPCR_LPM_MASK | CCM_CLPCR_ARM_CLK_DIS_ON_LPM_MASK));

    switch (APP_GetLPMPowerMode()) {
    case LPM_PowerModeOverRun:
    case LPM_PowerModeFullRun:
    case LPM_PowerModeLowSpeedRun:
    case LPM_PowerModeLowPowerRun:
        break;
    case LPM_PowerModeSysIdle:
        IOMUXC_GPR->GPR8  = 0x00000000;
        IOMUXC_GPR->GPR12 = 0x00000000;
        LPM_SystemRestoreWait();
        LPM_SetClockMode(kCLOCK_ModeRun, clpcr);
        break;
    case LPM_PowerModeLPIdle:
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        /* restore SDRAM from self-refresh mode */
        IOMUXC_GPR->GPR4 = (0 << 9U);
        while ((IOMUXC_GPR->GPR4 & (1 << 25U)) != (1 << 25U)) {};

        IOMUXC_GPR->GPR8  = 0x00000000;
        IOMUXC_GPR->GPR12 = 0x00000000;
        /* Interrupt occurs before system idle */
        LPM_SystemRestoreIdle();
        LPM_SetClockMode(kCLOCK_ModeRun, clpcr);

        break;
    case LPM_PowerModeSuspend:
        /* Restore when wakeup from suspend reset */
        LPM_SystemResumeDsm();

        /* recover handshaking */
        IOMUXC_GPR->GPR4  = 0x00000000;
        IOMUXC_GPR->GPR7  = 0x00000000;
        IOMUXC_GPR->GPR8  = 0x00000000;
        IOMUXC_GPR->GPR12 = 0x00000000;

        CCM->CCR &= ~CCM_CCR_REG_BYPASS_COUNT_MASK;
        break;
    default:
        assert(false);
        break;
    }

    LPM_DisableWakeupSource(vPortGetGptIrqn());
    APP_PowerPostSwitchHook(APP_GetLPMPowerMode());
}
#endif /* configUSE_TICKLESS_IDLE */
#endif

void APP_PowerPreSwitchHook(lpm_power_mode_t targetMode)
{
    if (targetMode == LPM_PowerModeSuspend || targetMode == LPM_PowerModeSNVS) {
        /* Wait for debug console output finished. */
        while (!(kLPUART_TransmissionCompleteFlag & LPUART_GetStatusFlags((LPUART_Type *)BOARD_DEBUG_UART_BASEADDR))) {}
        // DbgConsole_Deinit();

        /*
         * Set pin for current leakage.
         * Debug console RX pin: Set to pinmux to GPIO input.
         * Debug console TX pin: Don't need to change.
         */
        IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_13_GPIO1_IO13, 0);
        IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_13_GPIO1_IO13,
                            IOMUXC_SW_PAD_CTL_PAD_PKE_MASK | IOMUXC_SW_PAD_CTL_PAD_PUS(2) |
                                IOMUXC_SW_PAD_CTL_PAD_PUE_MASK);
    }
}

void APP_PowerPostSwitchHook(lpm_power_mode_t targetMode)
{
    if (targetMode == LPM_PowerModeSuspend) {
        /*
         * Debug console RX pin is set to GPIO input, nee to re-configure pinmux.
         * Debug console TX pin: Don't need to change.
         */
        IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_13_LPUART1_RX, 0);
        IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_13_LPUART1_RX, IOMUXC_SW_PAD_CTL_PAD_SPEED(2));

        // BOARD_InitDebugConsole();
    }
}

lpm_power_mode_t APP_GetLPMPowerMode(void)
{
    return s_targetPowerMode;
}

void APP_SetLPMPowerMode(lpm_power_mode_t mode)
{
    s_targetPowerMode = mode;
}
