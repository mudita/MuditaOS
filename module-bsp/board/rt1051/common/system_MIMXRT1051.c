/*
** ###################################################################
**     Processors:          MIMXRT1051CVJ5B
**                          MIMXRT1051CVL5B
**                          MIMXRT1051DVL6B
**
**     Compilers:           Keil ARM C/C++ Compiler
**                          Freescale C/C++ for Embedded ARM
**                          GNU C Compiler
**                          IAR ANSI C/C++ Compiler for ARM
**                          MCUXpresso Compiler
**
**     Reference manual:    IMXRT1050RM Rev.1, 03/2018
**     Version:             rev. 0.1, 2017-01-10
**     Build:               b180509
**
**     Abstract:
**         Provides a system configuration function and a global variable that
**         contains the system frequency. It configures the device and initializes
**         the oscillator (PLL) that is part of the microcontroller device.
**
**     The Clear BSD License
**     Copyright 2016 Freescale Semiconductor, Inc.
**     Copyright 2016-2018 NXP
**     All rights reserved.
**
**     Redistribution and use in source and binary forms, with or without
**     modification, are permitted (subject to the limitations in the
**     disclaimer below) provided that the following conditions are met:
**
**     * Redistributions of source code must retain the above copyright
**       notice, this list of conditions and the following disclaimer.
**
**     * Redistributions in binary form must reproduce the above copyright
**       notice, this list of conditions and the following disclaimer in the
**       documentation and/or other materials provided with the distribution.
**
**     * Neither the name of the copyright holder nor the names of its
**       contributors may be used to endorse or promote products derived from
**       this software without specific prior written permission.
**
**     NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
**     GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
**     HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
**     WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
**     MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
**     DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
**     LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
**     CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
**     SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
**     BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
**     WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
**     OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
**     IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
**     http:                 www.nxp.com
**     mail:                 support@nxp.com
**
**     Revisions:
**     - rev. 0.1 (2017-01-10)
**         Initial version.
**
** ###################################################################
*/

/*!
 * @file MIMXRT1051
 * @version 0.1
 * @date 2017-01-10
 * @brief Device specific configuration file for MIMXRT1051 (implementation file)
 *
 * Provides a system configuration function and a global variable that contains
 * the system frequency. It configures the device and initializes the oscillator
 * (PLL) that is part of the microcontroller device.
 */

#include <stdint.h>
#include "fsl_device_registers.h"

/* ----------------------------------------------------------------------------
   -- Core clock
   ---------------------------------------------------------------------------- */

uint32_t SystemCoreClock = DEFAULT_SYSTEM_CLOCK;

/* ----------------------------------------------------------------------------
   -- SystemInit()
   ---------------------------------------------------------------------------- */

void SystemInit(void)
{
#if ((__FPU_PRESENT == 1) && (__FPU_USED == 1))
    SCB->CPACR |= ((3UL << 10 * 2) | (3UL << 11 * 2)); /* set CP10, CP11 Full Access */
#endif                                                 /* ((__FPU_PRESENT == 1) && (__FPU_USED == 1)) */

    extern uint32_t g_pfnVectors[]; // Vector table defined in startup code
    SCB->VTOR = (uint32_t)g_pfnVectors;

    // Disable WDOGx Power Down Counter
    WDOG1->WMCR &= ~WDOG_WMCR_PDE_MASK;
    WDOG2->WMCR &= ~WDOG_WMCR_PDE_MASK;

    // Disable WDOGx watchdogs timers
    if (WDOG1->WCR & WDOG_WCR_WDE_MASK) {
        WDOG1->WCR &= ~WDOG_WCR_WDE_MASK;
    }
    if (WDOG2->WCR & WDOG_WCR_WDE_MASK) {
        WDOG2->WCR &= ~WDOG_WCR_WDE_MASK;
    }

#if defined(DISABLE_WATCHDOG)
    // Write watchdog update key to unlock
    RTWDOG->CNT = 0xD928C520U;
    // Disable RTWDOG and allow configuration updates
    RTWDOG->TOVAL = 0xFFFF;
    RTWDOG->CS    = (uint32_t)(((RTWDOG->CS) & ~RTWDOG_CS_EN_MASK) | RTWDOG_CS_UPDATE_MASK);
#else
    //
    // Perform preliminary RTWDOG configuration
    //
    // Write RTWDOG update key to unlock
    RTWDOG->CNT = 0xD928C520U;
    // Set timeout value (16s, assuming 128Hz clock - 32.768kHz after 256 prescaler)
    RTWDOG->TOVAL = 2048;
    // Enable RTWDOG, set 256 clock prescaler and allow configuration updates
    RTWDOG->CS = (uint32_t)((RTWDOG->CS) | RTWDOG_CS_EN_MASK | RTWDOG_CS_UPDATE_MASK | RTWDOG_CS_PRES_MASK);
#endif // (DISABLE_WATCHDOG)

    /* Disable Systick which might be enabled by bootrom */
    if (SysTick->CTRL & SysTick_CTRL_ENABLE_Msk) {
        SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    }

#if 0
/* Enable instruction and data caches */
#if defined(__ICACHE_PRESENT) && __ICACHE_PRESENT
    if (SCB_CCR_IC_Msk != (SCB_CCR_IC_Msk & SCB->CCR)) {
        SCB_EnableICache();
    }
#endif
#if defined(__DCACHE_PRESENT) && __DCACHE_PRESENT
    if (SCB_CCR_DC_Msk != (SCB_CCR_DC_Msk & SCB->CCR)) {
        SCB_EnableDCache();
    }
#endif

#endif

    SystemInitHook();
}

/* ----------------------------------------------------------------------------
   -- SystemCoreClockUpdate()
   ---------------------------------------------------------------------------- */

void SystemCoreClockUpdate(void)
{

    uint32_t freq;
    uint32_t PLL1MainClock;
    uint32_t PLL2MainClock;

    /* Periph_clk2_clk ---> Periph_clk */
    if (CCM->CBCDR & CCM_CBCDR_PERIPH_CLK_SEL_MASK) {
        switch (CCM->CBCMR & CCM_CBCMR_PERIPH_CLK2_SEL_MASK) {
        /* Pll3_sw_clk ---> Periph_clk2_clk ---> Periph_clk */
        case CCM_CBCMR_PERIPH_CLK2_SEL(0U):
            if (CCM_ANALOG->PLL_USB1 & CCM_ANALOG_PLL_USB1_BYPASS_MASK) {
                freq = (((CCM_ANALOG->PLL_USB1 & CCM_ANALOG_PLL_USB1_BYPASS_CLK_SRC_MASK) >>
                         CCM_ANALOG_PLL_USB1_BYPASS_CLK_SRC_SHIFT) == 0U)
                           ? CPU_XTAL_CLK_HZ
                           : CPU_CLK1_HZ;
            }
            else {
                freq = (CPU_XTAL_CLK_HZ * ((CCM_ANALOG->PLL_USB1 & CCM_ANALOG_PLL_USB1_DIV_SELECT_MASK) ? 22U : 20U));
            }
            break;

        /* Osc_clk ---> Periph_clk2_clk ---> Periph_clk */
        case CCM_CBCMR_PERIPH_CLK2_SEL(1U):
            freq = CPU_XTAL_CLK_HZ;
            break;

        case CCM_CBCMR_PERIPH_CLK2_SEL(2U):
            freq = (((CCM_ANALOG->PLL_SYS & CCM_ANALOG_PLL_SYS_BYPASS_CLK_SRC_MASK) >>
                     CCM_ANALOG_PLL_SYS_BYPASS_CLK_SRC_SHIFT) == 0U)
                       ? CPU_XTAL_CLK_HZ
                       : CPU_CLK1_HZ;
            break;

        case CCM_CBCMR_PERIPH_CLK2_SEL(3U):
            __attribute__((fallthrough));
        default:
            freq = 0U;
            break;
        }

        freq /= (((CCM->CBCDR & CCM_CBCDR_PERIPH_CLK2_PODF_MASK) >> CCM_CBCDR_PERIPH_CLK2_PODF_SHIFT) + 1U);
    }
    /* Pre_Periph_clk ---> Periph_clk */
    else {
        /* check if pll is bypassed */
        if (CCM_ANALOG->PLL_ARM & CCM_ANALOG_PLL_ARM_BYPASS_MASK) {
            PLL1MainClock = (((CCM_ANALOG->PLL_ARM & CCM_ANALOG_PLL_ARM_BYPASS_CLK_SRC_MASK) >>
                              CCM_ANALOG_PLL_ARM_BYPASS_CLK_SRC_SHIFT) == 0U)
                                ? CPU_XTAL_CLK_HZ
                                : CPU_CLK1_HZ;
        }
        else {
            PLL1MainClock = ((CPU_XTAL_CLK_HZ * ((CCM_ANALOG->PLL_ARM & CCM_ANALOG_PLL_ARM_DIV_SELECT_MASK) >>
                                                 CCM_ANALOG_PLL_ARM_DIV_SELECT_SHIFT)) >>
                             1U);
        }

        /* check if pll is bypassed */
        if (CCM_ANALOG->PLL_SYS & CCM_ANALOG_PLL_SYS_BYPASS_MASK) {
            PLL2MainClock = (((CCM_ANALOG->PLL_SYS & CCM_ANALOG_PLL_SYS_BYPASS_CLK_SRC_MASK) >>
                              CCM_ANALOG_PLL_SYS_BYPASS_CLK_SRC_SHIFT) == 0U)
                                ? CPU_XTAL_CLK_HZ
                                : CPU_CLK1_HZ;
        }
        else {
            PLL2MainClock =
                (CPU_XTAL_CLK_HZ * ((CCM_ANALOG->PLL_SYS & CCM_ANALOG_PLL_SYS_DIV_SELECT_MASK) ? 22U : 20U));
        }
        PLL2MainClock += ((uint64_t)CPU_XTAL_CLK_HZ * ((uint64_t)(CCM_ANALOG->PLL_SYS_NUM))) /
                         ((uint64_t)(CCM_ANALOG->PLL_SYS_DENOM));

        switch (CCM->CBCMR & CCM_CBCMR_PRE_PERIPH_CLK_SEL_MASK) {
        /* PLL2 ---> Pre_Periph_clk ---> Periph_clk */
        case CCM_CBCMR_PRE_PERIPH_CLK_SEL(0U):
            freq = PLL2MainClock;
            break;

        /* PLL2 PFD2 ---> Pre_Periph_clk ---> Periph_clk */
        case CCM_CBCMR_PRE_PERIPH_CLK_SEL(1U):
            freq = PLL2MainClock /
                   ((CCM_ANALOG->PFD_528 & CCM_ANALOG_PFD_528_PFD2_FRAC_MASK) >> CCM_ANALOG_PFD_528_PFD2_FRAC_SHIFT) *
                   18U;
            break;

        /* PLL2 PFD0 ---> Pre_Periph_clk ---> Periph_clk */
        case CCM_CBCMR_PRE_PERIPH_CLK_SEL(2U):
            freq = PLL2MainClock /
                   ((CCM_ANALOG->PFD_528 & CCM_ANALOG_PFD_528_PFD0_FRAC_MASK) >> CCM_ANALOG_PFD_528_PFD0_FRAC_SHIFT) *
                   18U;
            break;

        /* PLL1 divided(/2) ---> Pre_Periph_clk ---> Periph_clk */
        case CCM_CBCMR_PRE_PERIPH_CLK_SEL(3U):
            freq = PLL1MainClock / (((CCM->CACRR & CCM_CACRR_ARM_PODF_MASK) >> CCM_CACRR_ARM_PODF_SHIFT) + 1U);
            break;

        default:
            freq = 0U;
            break;
        }
    }

    SystemCoreClock = (freq / (((CCM->CBCDR & CCM_CBCDR_AHB_PODF_MASK) >> CCM_CBCDR_AHB_PODF_SHIFT) + 1U));
}

/* ----------------------------------------------------------------------------
   -- SystemInitHook()
   ---------------------------------------------------------------------------- */

__attribute__((weak)) void SystemInitHook(void)
{
    /* Void implementation of the weak function. */
}
