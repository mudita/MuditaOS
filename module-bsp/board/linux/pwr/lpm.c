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

//#include "clock_config.h"
#include "board.h"

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
#define LPM_COUNT_PER_TICK (LPM_SYSTICK_COUNTER_FREQ / configTICK_RATE_HZ)

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
static lpm_power_mode_t s_targetPowerMode;
void APP_PowerPreSwitchHook(lpm_power_mode_t targetMode);
void APP_PowerPostSwitchHook(lpm_power_mode_t targetMode);
lpm_power_mode_t APP_GetLPMPowerMode(void);

/*******************************************************************************
 * Code
 ******************************************************************************/
static void BOARD_SetLPClockGate(void) {
}

static void LPM_SetClockMode(clock_mode_t mode, uint32_t clpcr) {
}

void LPM_SwitchToXtalOSC(void) {
}

void LPM_SwitchToRcOSC(void) {
}


void LPM_SwitchFlexspiClock(lpm_power_mode_t power_mode) {
}

void LPM_RestoreFlexspiClock(void) {
}

void LPM_SwitchBandgap(void) {
}

void LPM_RestoreBandgap(void) {
}

void LPM_DisablePLLs(lpm_power_mode_t power_mode) {
}

void LPM_RestorePLLs(lpm_power_mode_t power_mode) {
}

static void LPM_DisableRbcBypass(void) {
}

static void LPM_SystemWait(void) {
}

static void LPM_SystemRestoreWait(void) {
}

static void LPM_SystemIdle(void) {
}

void LPM_SystemRestoreIdle(void) {
}

static void LPM_SystemDsm() {
}

void LPM_SystemRestoreDsm(void) {
}

void LPM_SystemResumeDsm(void) {
}

void LPM_SystemOverRunRecovery(void) {
}

void LPM_SystemOverRun(void) {
}

void LPM_SystemFullRun(void) {
}

void LPM_SystemLowSpeedRun(void) {
}

void LPM_SystemLowPowerRun(void) {
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
    gptConfig.enableMode = true; /* Don't keep counter when stop */
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

bool LPM_Init(lpm_power_mode_t run_mode) {
	return true;
}

void LPM_Deinit(void) {
}

void LPM_EnableWakeupSource(uint32_t irq) {
}

void LPM_DisableWakeupSource(uint32_t irq) {
}

bool LPM_SetPowerMode(lpm_power_mode_t mode) {
}

#ifdef FSL_RTOS_FREE_RTOS /* Only FreeRTOS supports listener notification */
void LPM_RegisterPowerListener(lpm_power_mode_callback_t callback, void *data) {

}

void LPM_UnregisterPowerListener(lpm_power_mode_callback_t callback, void *data) {
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

    switch (APP_GetLPMPowerMode())
    {
        case LPM_PowerModeOverRun:
        case LPM_PowerModeFullRun:
        case LPM_PowerModeLowSpeedRun:
        case LPM_PowerModeLowPowerRun:
            break;
        case LPM_PowerModeSysIdle:
            LPM_SetClockMode(kCLOCK_ModeWait, clpcr | CCM_CLPCR_LPM(kCLOCK_ModeWait) |
                                                  CCM_CLPCR_ARM_CLK_DIS_ON_LPM_MASK | CCM_CLPCR_STBY_COUNT_MASK | 0x1C |
                                                  0x08280000);
            BOARD_SetLPClockGate();
            /* If last mode is idle, need to restore idle states. */
            if (s_SystemIdleFlag)
            {
                s_SkipRestorePLLs = 1;
                LPM_SystemRestoreIdle();
            }

            LPM_SystemWait();
            IOMUXC_GPR->GPR8 = 0xaaaaaaaa;
            IOMUXC_GPR->GPR12 = 0x0000000a;
            break;
        case LPM_PowerModeLPIdle:
        	LPM_SetClockMode(kCLOCK_ModeWait, clpcr | CCM_CLPCR_LPM(kCLOCK_ModeWait) |
                                                  CCM_CLPCR_ARM_CLK_DIS_ON_LPM_MASK | CCM_CLPCR_STBY_COUNT_MASK | 0x1C |
                                                  0x08280000);
            BOARD_SetLPClockGate();
            LPM_SystemIdle();
            IOMUXC_GPR->GPR8 = 0xaaaaaaaa;
            IOMUXC_GPR->GPR12 = 0x0000000a;

        	/* put SDRAM to self-refresh mode */
        	IOMUXC_GPR->GPR4 = (1<<9U);
        	while((IOMUXC_GPR->GPR4 & (1<<25U)) != (1<<25U)) { };
            break;
        case LPM_PowerModeSuspend:
            LPM_SetClockMode(kCLOCK_ModeStop, clpcr | CCM_CLPCR_LPM(kCLOCK_ModeStop) | CCM_CLPCR_VSTBY_MASK |
                                                  CCM_CLPCR_STBY_COUNT_MASK | CCM_CLPCR_SBYOS_MASK |
                                                  CCM_CLPCR_ARM_CLK_DIS_ON_LPM_MASK | 0x08280000);
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

    switch (APP_GetLPMPowerMode())
    {
        case LPM_PowerModeOverRun:
        case LPM_PowerModeFullRun:
        case LPM_PowerModeLowSpeedRun:
        case LPM_PowerModeLowPowerRun:
            break;
        case LPM_PowerModeSysIdle:
            IOMUXC_GPR->GPR8 = 0x00000000;
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
            IOMUXC_GPR->GPR4 = (0<<9U);
            while((IOMUXC_GPR->GPR4 & (1<<25U)) != (1<<25U)) { };

            IOMUXC_GPR->GPR8 = 0x00000000;
            IOMUXC_GPR->GPR12 = 0x00000000;
            /* Interrupt occurs before system idle */
            LPM_SystemRestoreIdle();
            LPM_SetClockMode(kCLOCK_ModeRun, clpcr);

            break;
        case LPM_PowerModeSuspend:
            /* Restore when wakeup from suspend reset */
            LPM_SystemResumeDsm();

            /* recover handshaking */
            IOMUXC_GPR->GPR4 = 0x00000000;
            IOMUXC_GPR->GPR7 = 0x00000000;
            IOMUXC_GPR->GPR8 = 0x00000000;
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

void APP_PowerPreSwitchHook(lpm_power_mode_t targetMode) {
}

void APP_PowerPostSwitchHook(lpm_power_mode_t targetMode) {
}

lpm_power_mode_t APP_GetLPMPowerMode(void) {
    return s_targetPowerMode;
}

void APP_SetLPMPowerMode(lpm_power_mode_t mode){
}
