/*
 * Copyright 2017 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _LPM_H_
#define _LPM_H_

#include "fsl_common.h"
#include "fsl_clock.h"
#include "fsl_device_registers.h"
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#ifndef FSL_RTOS_FREE_RTOS
#define FSL_RTOS_FREE_RTOS
#endif

extern void vPortGPTIsr(void);

#define vPortGptIsr GPT1_IRQHandler

#define CLOCK_SET_MUX(mux, value)                                                                                      \
                                                                                                                       \
    do {                                                                                                               \
        CCM_TUPLE_REG(CCM, mux) = (CCM_TUPLE_REG(CCM, mux) & (~CCM_TUPLE_MASK(mux))) |                                 \
                                  (((uint32_t)((value) << CCM_TUPLE_SHIFT(mux))) & CCM_TUPLE_MASK(mux));               \
        while (CCM->CDHIPR != 0) {}                                                                                    \
                                                                                                                       \
    } while (0)

#define CLOCK_SET_DIV(divider, value)                                                                                  \
                                                                                                                       \
    do {                                                                                                               \
        CCM_TUPLE_REG(CCM, divider) = (CCM_TUPLE_REG(CCM, divider) & (~CCM_TUPLE_MASK(divider))) |                     \
                                      (((uint32_t)((value) << CCM_TUPLE_SHIFT(divider))) & CCM_TUPLE_MASK(divider));   \
        while (CCM->CDHIPR != 0) {}                                                                                    \
                                                                                                                       \
    } while (0)

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

#if defined(XIP_EXTERNAL_FLASH) && (XIP_EXTERNAL_FLASH == 1)
#define LPM_EnterCritical()                                                                                            \
                                                                                                                       \
    do {                                                                                                               \
        __disable_irq();                                                                                               \
        SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;                                                                     \
                                                                                                                       \
    } while (0)

#define LPM_ExitCritical()                                                                                             \
                                                                                                                       \
    do {                                                                                                               \
        __enable_irq();                                                                                                \
        SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;                                                                      \
                                                                                                                       \
    } while (0)

#else
#define LPM_EnterCritical()
#define LPM_ExitCritical()
#endif

/* Power mode definition of low power management.
 * Waken up duration Off > Dsm > Idle > Wait > Run.
 */
typedef enum _lpm_power_mode
{
    LPM_PowerModeOverRun = 0, /* Over RUN mode, CPU won't stop running */

    LPM_PowerModeFullRun, /* Full RUN mode, CPU won't stop running */

    LPM_PowerModeLowSpeedRun,

    LPM_PowerModeLowPowerRun,

    LPM_PowerModeRunEnd = LPM_PowerModeLowPowerRun,
    /* In system wait mode, cpu clock is gated.
     * All peripheral can remain active, clock gating decided by CCGR setting.
     * DRAM enters auto-refresh mode when there is no access.
     */
    LPM_PowerModeSysIdle, /* System WAIT mode, also system low speed idle */

    /* In low power idle mode, all PLL/PFD is off, cpu power is off.
     * Analog modules running in low power mode.
     * All high-speed peripherals are power gated
     * Low speed peripherals can remain running at low frequency
     * DRAM in self-refresh.
     */
    LPM_PowerModeLPIdle, /* Low Power Idle mode */

    /* In deep sleep mode, all PLL/PFD is off, XTAL is off, cpu power is off.
     * All clocks are shut off except 32K RTC clock
     * All high-speed peripherals are power gated
     * Low speed peripherals are clock gated
     * DRAM in self-refresh.
     * If RTOS is used, systick will be disabled in DSM
     */
    LPM_PowerModeSuspend, /* Deep Sleep mode, suspend. */

    LPM_PowerModeSNVS, /* Power off mode, or shutdown mode */

    LPM_PowerModeEnd = LPM_PowerModeSNVS
} lpm_power_mode_t;

typedef bool (*lpm_power_mode_callback_t)(lpm_power_mode_t curMode, lpm_power_mode_t newMode, void *data);

/*! @name Time sensitive region */
/* @{ */
#if defined(XIP_EXTERNAL_FLASH) && (XIP_EXTERNAL_FLASH == 1)
#if (defined(__ICCARM__))
#define QUICKACCESS_SECTION_CODE(func) __ramfunc func
#elif (defined(__ARMCC_VERSION))
#define QUICKACCESS_SECTION_CODE(func) __attribute__((section("RamFunction"))) func
#elif defined(__MCUXPRESSO)
#define QUICKACCESS_SECTION_CODE(func) __attribute__((section(".ramfunc.$SRAM_ITC"))) func
#elif (defined(__GNUC__))
#define QUICKACCESS_SECTION_CODE(func) __attribute__((section("RamFunction"))) func
#else
#error Toolchain not supported.
#endif /* defined(__ICCARM__) */
#else
#if (defined(__ICCARM__))
#define QUICKACCESS_SECTION_CODE(func) func
#elif (defined(__ARMCC_VERSION))
#define QUICKACCESS_SECTION_CODE(func) func
#elif (defined(__MCUXPRESSO))
#define QUICKACCESS_SECTION_CODE(func) func
#elif (defined(__GNUC__))
#define QUICKACCESS_SECTION_CODE(func) func
#else
#error Toolchain not supported.
#endif
#endif /* __FSL_SDK_DRIVER_QUICK_ACCESS_ENABLE */

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C"
{
#endif /* __cplusplus*/

    QUICKACCESS_SECTION_CODE(void LPM_SwitchBandgap(void));
    QUICKACCESS_SECTION_CODE(void LPM_RestoreBandgap(void));
    QUICKACCESS_SECTION_CODE(void LPM_SwitchToXtalOSC(void));
    QUICKACCESS_SECTION_CODE(void LPM_SwitchToRcOSC(void));
    QUICKACCESS_SECTION_CODE(void LPM_SwitchFlexspiClock(lpm_power_mode_t power_mode));
    QUICKACCESS_SECTION_CODE(void LPM_RestoreFlexspiClock(void));

    /* Initialize the Low Power Management */
    bool LPM_Init(lpm_power_mode_t run_mode);

    /* Deinitialize the Low Power Management */
    void LPM_Deinit(void);

    /* Enable wakeup source in low power mode */
    void LPM_EnableWakeupSource(uint32_t irq);

    /* Disable wakeup source in low power mode */
    void LPM_DisableWakeupSource(uint32_t irq);

    /* Set power mode, all registered listeners will be notified.
     * Return true if all the registered listeners return true.
     */
    bool LPM_SetPowerMode(lpm_power_mode_t mode);

    /* LPM_SetPowerMode() won't switch system power status immediately,
     * instead, such operation is done by LPM_WaitForInterrupt().
     * It can be callled in idle task of FreeRTOS, or main loop in bare
     * metal application. The sleep depth of this API depends
     * on current power mode set by LPM_SetPowerMode().
     * The timeoutMilliSec means if no interrupt occurs before timeout, the
     * system will be waken up by systick. If timeout exceeds hardware timer
     * limitation, timeout will be reduced to maximum time of hardware.
     * timeoutMilliSec only works in FreeRTOS, in bare metal application,
     * it's just ignored.
     */
    void LPM_WaitForInterrupt(uint32_t timeoutMilliSec);

#ifdef FSL_RTOS_FREE_RTOS
    /* Register power mode switch listener. When LPM_SetPowerMode()
     * is called, all the registered listeners will be invoked. The
     * listener returns true if it allows the power mode switch,
     * otherwise returns FALSE.
     */
    void LPM_RegisterPowerListener(lpm_power_mode_callback_t callback, void *data);

    /* Unregister power mode switch listener */
    void LPM_UnregisterPowerListener(lpm_power_mode_callback_t callback, void *data);

    void LPM_SystemRestoreDsm(void);
    void LPM_SystemRestoreIdle(void);
    void LPM_SystemResumeDsm(void);

    void LPM_RestorePLLs(lpm_power_mode_t power_mode);
    void LPM_DisablePLLs(lpm_power_mode_t power_mode);

    void LPM_SystemFullRun(void);
    void LPM_SystemOverRun(void);
    void LPM_SystemLowSpeedRun(void);
    void LPM_SystemLowPowerRun(void);

#endif

    void APP_PowerPostSwitchHook(lpm_power_mode_t targetMode);

    void APP_SetLPMPowerMode(lpm_power_mode_t mode);

#if defined(__cplusplus)
}
#endif /* __cplusplus*/

#endif /* _LPM_H_ */
