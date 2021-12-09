// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

/*
 * The Clear BSD License
 * Copyright 2017 NXP
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
#ifndef _CLOCK_CONFIG_H_
#define _CLOCK_CONFIG_H_

#include "fsl_common.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define BOARD_XTAL0_CLK_HZ 24000000U /*!< Board xtal0 frequency in Hz */

#define BOARD_XTAL32K_CLK_HZ 32768U /*!< Board xtal32k frequency in Hz */
/*******************************************************************************
 ************************ BOARD_InitBootClocks function ************************
 ******************************************************************************/

#if defined(__cplusplus)
extern "C"
{
#endif /* __cplusplus*/

    /*!
     * @brief This function executes default configuration of clocks.
     *
     */
    void BOARD_InitBootClocks(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus*/

/*******************************************************************************
 * API for BOARD_BootClockRUN configuration
 ******************************************************************************/
#if defined(__cplusplus)
extern "C"
{
#endif /* __cplusplus*/

    /*!
     * @brief This function executes configuration of clocks.
     *
     */
    void BOARD_BootClockRUN(void);

#define CLK_INSTANCE_ALL 0
#define CLK_ENABLE       1
#define CLK_DISABLE      0

    /* ADC */
    void clkADCsetup(uint8_t instance, uint8_t enabled);
    /* XBAR */
    void clkXBARsetup(uint8_t instance, uint8_t enabled);
    /* GPT */
    void clkGPTsetup(uint8_t instance, uint8_t enabled);
    /* PIT */
    void clkPITsetup(uint8_t instance, uint8_t enabled);
    /* USDHC */
    void clkUSDHCsetup(uint8_t instance, uint8_t enabled);
    /* FLEXSPI */
    void clkFLEXSPIsetup(uint8_t instance, uint8_t enabled);
    /* CSI */
    void clkCSIsetup(uint8_t instance, uint8_t enabled);
    /* LPSPI */
    void clkLPSPICsetup(uint8_t instance, uint8_t enabled);
    /* TRACE */
    void clkTRACEsetup(uint8_t instance, uint8_t enabled);
    /* SAI */
    void clkSAIsetup(uint8_t instance, uint8_t enabled);
    /* LPI2C */
    void clkLPI2Csetup(uint8_t instance, uint8_t enabled);
    /* CAN */
    void clkCANsetup(uint8_t instance, uint8_t enabled);
    /* LPUART */
    void clkLPUARTsetup(uint8_t instance, uint8_t enabled);
    /* LCDIF */
    void clkLCDIFsetup(uint8_t instance, uint8_t enabled);
    /* SPDIF */
    void clkSPDIFsetup(uint8_t instance, uint8_t enabled);
    /* FLEXIO */
    void clkFLEXIOsetup(uint8_t instance, uint8_t enabled);

    void clkPLL1setup(uint8_t enabled);

    void clkPLL2setup(uint8_t enabled);
    void clkPLL2_PFD0setup(uint8_t enabled);
    void clkPLL2_PFD1setup(uint8_t enabled);
    void clkPLL2_PFD2setup(uint8_t enabled);
    void clkPLL2_PFD3setup(uint8_t enabled);

    void clkPLL3setup(uint8_t enabled);
    void clkPLL3_PFD0setup(uint8_t enabled);
    void clkPLL3_PFD1setup(uint8_t enabled);
    void clkPLL3_PFD2setup(uint8_t enabled);
    void clkPLL3_PFD3setup(uint8_t enabled);

    void clkPLL4setup(uint8_t enabled, const clock_audio_pll_config_t config);

    void clkPLL5setup(uint8_t enabled);

    void clkPLL6setup(uint8_t enabled);

    void clkPLL7setup(uint8_t enabled);

    typedef enum
    {
        PerphClock_I2C,
        PerphClock_LPSPI,
        PerphClock_LPUART,
        PerphClock_SAI1,
        PerphClock_SAI2,
        PerphClock_USDHC2,

    } PerphClock_t;

    uint32_t GetPerphSourceClock(PerphClock_t clock);
    void PrintSystemClocks();

    void LPM_EnterFullSpeed(void);
    void LPM_EnterLowPowerRun();
    void LPM_EnterLowPowerIdle(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus*/

#endif /* _CLOCK_CONFIG_H_ */
