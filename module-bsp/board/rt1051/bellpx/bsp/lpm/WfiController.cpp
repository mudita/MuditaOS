// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "WfiController.hpp"
#include "EnterSleepMode.h"
#include <fsl_gpc.h>
#include <fsl_rtwdog.h>
#include <fsl_runtimestat_gpt.h>
#include <Utils.hpp>
#include <time/time_constants.hpp>
#include <ticks.hpp>
#include <timers.h>

namespace bsp
{
    namespace
    {
        /* RTC wakes up CPU every minute, so go to sleep only if next timer will
         * trigger after more than minute - this way no event will ever be missed */
        constexpr auto timersInactivityTimeMs{60 * utils::time::milisecondsInSecond};

        bool wfiModeAllowed = false;
        std::uint32_t timeSpentInWFI;

        bool isTimerTaskScheduledSoon()
        {
            const auto currentTick          = cpp_freertos::Ticks::GetTicks();
            const auto timersNextWakeUpTick = xTimerGetNextWakeUpTime();

            if (timersNextWakeUpTick > currentTick) {
                return (cpp_freertos::Ticks::TicksToMs(timersNextWakeUpTick - currentTick) < timersInactivityTimeMs);
            }
            return true;
        }

        bool isWfiModeAllowed()
        {
            return wfiModeAllowed;
        }

        void peripheralEnterDozeMode()
        {
            IOMUXC_GPR->GPR8 = IOMUXC_GPR_GPR8_LPI2C1_IPG_DOZE_MASK | IOMUXC_GPR_GPR8_LPI2C2_IPG_DOZE_MASK |
                               IOMUXC_GPR_GPR8_LPI2C3_IPG_DOZE_MASK | IOMUXC_GPR_GPR8_LPI2C4_IPG_DOZE_MASK |
                               IOMUXC_GPR_GPR8_LPSPI1_IPG_DOZE_MASK | IOMUXC_GPR_GPR8_LPSPI2_IPG_DOZE_MASK |
                               IOMUXC_GPR_GPR8_LPSPI3_IPG_DOZE_MASK | IOMUXC_GPR_GPR8_LPSPI4_IPG_DOZE_MASK |
                               IOMUXC_GPR_GPR8_LPUART1_IPG_DOZE_MASK | IOMUXC_GPR_GPR8_LPUART2_IPG_DOZE_MASK |
                               IOMUXC_GPR_GPR8_LPUART3_IPG_DOZE_MASK | IOMUXC_GPR_GPR8_LPUART4_IPG_DOZE_MASK |
                               IOMUXC_GPR_GPR8_LPUART5_IPG_DOZE_MASK | IOMUXC_GPR_GPR8_LPUART6_IPG_DOZE_MASK |
                               IOMUXC_GPR_GPR8_LPUART7_IPG_DOZE_MASK | IOMUXC_GPR_GPR8_LPUART8_IPG_DOZE_MASK;

            IOMUXC_GPR->GPR12 = IOMUXC_GPR_GPR12_FLEXIO1_IPG_DOZE_MASK | IOMUXC_GPR_GPR12_FLEXIO2_IPG_DOZE_MASK;
        }

        void peripheralExitDozeMode()
        {
            IOMUXC_GPR->GPR8  = 0x00000000;
            IOMUXC_GPR->GPR12 = 0x00000000;
        }

        void setRunModeConfig()
        {
            CCM->CLPCR &= ~(CCM_CLPCR_LPM_MASK | CCM_CLPCR_ARM_CLK_DIS_ON_LPM_MASK);
        }

        void setWaitModeConfig()
        {
            std::uint32_t clpcr;

            /*
             * ERR050143: CCM: When improper low-power sequence is used,
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
            CCM->CLPCR = clpcr | CCM_CLPCR_LPM(kCLOCK_ModeWait) | CCM_CLPCR_MASK_SCU_IDLE_MASK |
                         CCM_CLPCR_MASK_L2CC_IDLE_MASK | CCM_CLPCR_ARM_CLK_DIS_ON_LPM_MASK | CCM_CLPCR_STBY_COUNT_MASK |
                         CCM_CLPCR_BYPASS_LPM_HS0_MASK | CCM_CLPCR_BYPASS_LPM_HS1_MASK;
            GPC_DisableIRQ(GPC, GPR_IRQ_IRQn);
        }

        bool isBrownoutOn2P5Output()
        {
            return ((PMU->REG_2P5 & PMU_REG_2P5_BO_VDD2P5_MASK) != 0);
        }

        void disableSystick()
        {
            SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
            NVIC_ClearPendingIRQ(SysTick_IRQn);
        }

        void enableSystick()
        {
            SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
        }
    } // namespace

    void allowEnteringWfiMode()
    {
        wfiModeAllowed = true;
    }

    /* Block WFI mode so that OS wakes up fully and goes to sleep only after
     * frequency has dropped back to the lowest level */
    void blockEnteringWfiMode()
    {
        wfiModeAllowed = false;
    }

    std::uint32_t getLastTimeSpentInWfi()
    {
        return timeSpentInWFI;
    }

    std::uint32_t enterWfiModeIfAllowed()
    {
        if (!isWfiModeAllowed()) {
            return 0;
        }
        timeSpentInWFI = 0;
        if (isTimerTaskScheduledSoon()) {
            blockEnteringWfiMode();
            return 0;
        }
        if (isBrownoutOn2P5Output()) {
            LOG_WARN("WFI disabled - brownout detected on 2P5 VDD output");
            blockEnteringWfiMode();
            return 0;
        }

        RTWDOG_Refresh(RTWDOG);
        setWaitModeConfig();
        peripheralEnterDozeMode();

        disableSystick();
        const auto enterWfiTicks = ulHighFrequencyTimerTicks();

        const auto savedPrimask = DisableGlobalIRQ();
        __DSB();
        __ISB();

        /* Clear the SLEEPDEEP bit to go into sleep mode (WAIT) */
        SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;

        /* Switch SDRAM to self-refresh and execute WFI */
        enterSleepMode();

        __NOP();
        __NOP();
        __NOP();
        __NOP();

        const auto exitWfiTicks = ulHighFrequencyTimerTicks();
        enableSystick();

        peripheralExitDozeMode();
        RTWDOG_Refresh(RTWDOG);
        EnableGlobalIRQ(savedPrimask);

        blockEnteringWfiMode();
        setRunModeConfig();

        timeSpentInWFI = ulHighFrequencyTimerTicksToMs(utils::computeIncrease(exitWfiTicks, enterWfiTicks));
        return timeSpentInWFI;
    }
} // namespace bsp
