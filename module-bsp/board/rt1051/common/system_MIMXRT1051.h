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
 * @brief Device specific configuration file for MIMXRT1051 (header file)
 *
 * Provides a system configuration function and a global variable that contains
 * the system frequency. It configures the device and initializes the oscillator
 * (PLL) that is part of the microcontroller device.
 */

#ifndef _SYSTEM_MIMXRT1051_H_
#define _SYSTEM_MIMXRT1051_H_ /**< Symbol preventing repeated inclusion */

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

    /* Define clock source values */

#define CPU_XTAL_CLK_HZ 24000000UL /* Value of the external crystal or oscillator clock frequency in Hz */

#define CPU_CLK1_HZ 0UL /* Value of the CLK1 (select the CLK1_N/CLK1_P as source) frequency in Hz */
    /* If CLOCK1_P,CLOCK1_N is choose as the pll bypass clock source, please implement the CLKPN_FREQ define, otherwise
     * 0 will be returned. */

#define DEFAULT_SYSTEM_CLOCK 528000000UL /* Default System clock value */

    /**
     * @brief System clock frequency (core clock)
     *
     * The system clock frequency supplied to the SysTick timer and the processor
     * core clock. This variable can be used by the user application to setup the
     * SysTick timer or configure other parameters. It may also be used by debugger to
     * query the frequency of the debug timer or configure the trace clock speed
     * SystemCoreClock is initialized with a correct predefined value.
     */
    extern uint32_t SystemCoreClock;

    /**
     * @brief Setup the microcontroller system.
     *
     * Typically this function configures the oscillator (PLL) that is part of the
     * microcontroller device. For systems with variable clock speed it also updates
     * the variable SystemCoreClock. SystemInit is called from startup_device file.
     */
    void SystemInit(void);

    /**
     * @brief Updates the SystemCoreClock variable.
     *
     * It must be called whenever the core clock is changed during program
     * execution. SystemCoreClockUpdate() evaluates the clock register settings and calculates
     * the current core clock.
     */
    void SystemCoreClockUpdate(void);

    /**
     * @brief SystemInit function hook.
     *
     * This weak function allows to call specific initialization code during the
     * SystemInit() execution.This can be used when an application specific code needs
     * to be called as close to the reset entry as possible (for example the Multicore
     * Manager MCMGR_EarlyInit() function call).
     * NOTE: No global r/w variables can be used in this hook function because the
     * initialization of these variables happens after this function.
     */
    void SystemInitHook(void);

#ifdef __cplusplus
}
#endif

#endif /* _SYSTEM_MIMXRT1051_H_ */
