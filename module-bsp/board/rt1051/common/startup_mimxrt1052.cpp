// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

//*****************************************************************************
// MIMXRT1052 startup code for use with MCUXpresso IDE
//
// Version : 090518
//*****************************************************************************
//
// The Clear BSD License
// Copyright 2016-2018 NXP
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted (subject to the limitations in the
// disclaimer below) provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the distribution.
//
// * Neither the name of the copyright holder nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
//
// NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
// GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
// HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
// BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
// OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
// IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//*****************************************************************************

#if defined(DEBUG)
#pragma GCC push_options
#pragma GCC optimize("Og")
#endif // (DEBUG)

#if defined(__cplusplus)
#ifdef __REDLIB__
#error Redlib does not support C++
#else
//*****************************************************************************
//
// The entry point for the C++ library startup
//
//*****************************************************************************
extern "C"
{
    extern void __libc_init_array(void);
    extern void __call_exitprocs(int code, void *d);
}
#endif
#endif

#define WEAK     __attribute__((weak))
#define WEAK_AV  __attribute__((weak, section(".after_vectors")))
#define ALIAS(f) __attribute__((weak, alias(#f)))

//*****************************************************************************
#if defined(__cplusplus)
extern "C"
{
#endif
    /* Platform exit function it is a last function called after system
     * deinitalization. It should be used to cutoff power
     */
    WEAK void _platform_exit(void)
    {}
    //*****************************************************************************
    // Variable to store CRP value in. Will be placed automatically
    // by the linker when "Enable Code Read Protect" selected.
    // See crp.h header for more information
    //*****************************************************************************
    //*****************************************************************************
    // Declaration of external SystemInit function
    //*****************************************************************************
    extern void SystemInit(void);

    //*****************************************************************************
    // Forward declaration of the core exception handlers.
    // When the application defines a handler (with the same name), this will
    // automatically take precedence over these weak definitions
    //*****************************************************************************
    void ResetISR(void);
    WEAK void NMI_Handler(void);
    WEAK void HardFault_Handler(void);
    WEAK void SVC_Handler(void);
    WEAK void PendSV_Handler(void);
    WEAK void SysTick_Handler(void);
    WEAK void IntDefaultHandler(void);
    WEAK void MemManage_Handler(void);
    WEAK void UsageFault_Handler(void);
    WEAK void BusFault_Handler(void);

    //*****************************************************************************
    // Forward declaration of the application IRQ handlers. When the application
    // defines a handler (with the same name), this will automatically take
    // precedence over weak definitions below
    //*****************************************************************************
    WEAK void DMA0_DMA16_IRQHandler(void);
    WEAK void DMA1_DMA17_IRQHandler(void);
    WEAK void DMA2_DMA18_IRQHandler(void);
    WEAK void DMA3_DMA19_IRQHandler(void);
    WEAK void DMA4_DMA20_IRQHandler(void);
    WEAK void DMA5_DMA21_IRQHandler(void);
    WEAK void DMA6_DMA22_IRQHandler(void);
    WEAK void DMA7_DMA23_IRQHandler(void);
    WEAK void DMA8_DMA24_IRQHandler(void);
    WEAK void DMA9_DMA25_IRQHandler(void);
    WEAK void DMA10_DMA26_IRQHandler(void);
    WEAK void DMA11_DMA27_IRQHandler(void);
    WEAK void DMA12_DMA28_IRQHandler(void);
    WEAK void DMA13_DMA29_IRQHandler(void);
    WEAK void DMA14_DMA30_IRQHandler(void);
    WEAK void DMA15_DMA31_IRQHandler(void);
    WEAK void DMA_ERROR_IRQHandler(void);
    WEAK void CTI0_ERROR_IRQHandler(void);
    WEAK void CTI1_ERROR_IRQHandler(void);
    WEAK void CORE_IRQHandler(void);
    WEAK void LPUART1_IRQHandler(void);
    WEAK void LPUART2_IRQHandler(void);
    WEAK void LPUART3_IRQHandler(void);
    WEAK void LPUART4_IRQHandler(void);
    WEAK void LPUART5_IRQHandler(void);
    WEAK void LPUART6_IRQHandler(void);
    WEAK void LPUART7_IRQHandler(void);
    WEAK void LPUART8_IRQHandler(void);
    WEAK void LPI2C1_IRQHandler(void);
    WEAK void LPI2C2_IRQHandler(void);
    WEAK void LPI2C3_IRQHandler(void);
    WEAK void LPI2C4_IRQHandler(void);
    WEAK void LPSPI1_IRQHandler(void);
    WEAK void LPSPI2_IRQHandler(void);
    WEAK void LPSPI3_IRQHandler(void);
    WEAK void LPSPI4_IRQHandler(void);
    WEAK void CAN1_IRQHandler(void);
    WEAK void CAN2_IRQHandler(void);
    WEAK void FLEXRAM_IRQHandler(void);
    WEAK void KPP_IRQHandler(void);
    WEAK void TSC_DIG_IRQHandler(void);
    WEAK void GPR_IRQ_IRQHandler(void);
    WEAK void LCDIF_IRQHandler(void);
    WEAK void CSI_IRQHandler(void);
    WEAK void PXP_IRQHandler(void);
    WEAK void WDOG2_IRQHandler(void);
    WEAK void SNVS_HP_WRAPPER_IRQHandler(void);
    WEAK void SNVS_HP_WRAPPER_TZ_IRQHandler(void);
    WEAK void SNVS_LP_WRAPPER_IRQHandler(void);
    WEAK void CSU_IRQHandler(void);
    WEAK void DCP_IRQHandler(void);
    WEAK void DCP_VMI_IRQHandler(void);
    WEAK void Reserved68_IRQHandler(void);
    WEAK void TRNG_IRQHandler(void);
    WEAK void SJC_IRQHandler(void);
    WEAK void BEE_IRQHandler(void);
    WEAK void SAI1_IRQHandler(void);
    WEAK void SAI2_IRQHandler(void);
    WEAK void SAI3_RX_IRQHandler(void);
    WEAK void SAI3_TX_IRQHandler(void);
    WEAK void SPDIF_IRQHandler(void);
    WEAK void ANATOP_EVENT0_IRQHandler(void);
    WEAK void ANATOP_EVENT1_IRQHandler(void);
    WEAK void ANATOP_TAMP_LOW_HIGH_IRQHandler(void);
    WEAK void ANATOP_TEMP_PANIC_IRQHandler(void);
    WEAK void USB_PHY1_IRQHandler(void);
    WEAK void USB_PHY2_IRQHandler(void);
    WEAK void ADC1_IRQHandler(void);
    WEAK void ADC2_IRQHandler(void);
    WEAK void DCDC_IRQHandler(void);
    WEAK void Reserved86_IRQHandler(void);
    WEAK void Reserved87_IRQHandler(void);
    WEAK void GPIO1_INT0_IRQHandler(void);
    WEAK void GPIO1_INT1_IRQHandler(void);
    WEAK void GPIO1_INT2_IRQHandler(void);
    WEAK void GPIO1_INT3_IRQHandler(void);
    WEAK void GPIO1_INT4_IRQHandler(void);
    WEAK void GPIO1_INT5_IRQHandler(void);
    WEAK void GPIO1_INT6_IRQHandler(void);
    WEAK void GPIO1_INT7_IRQHandler(void);
    WEAK void GPIO1_Combined_0_15_IRQHandler(void);
    WEAK void GPIO1_Combined_16_31_IRQHandler(void);
    WEAK void GPIO2_Combined_0_15_IRQHandler(void);
    WEAK void GPIO2_Combined_16_31_IRQHandler(void);
    WEAK void GPIO3_Combined_0_15_IRQHandler(void);
    WEAK void GPIO3_Combined_16_31_IRQHandler(void);
    WEAK void GPIO4_Combined_0_15_IRQHandler(void);
    WEAK void GPIO4_Combined_16_31_IRQHandler(void);
    WEAK void GPIO5_Combined_0_15_IRQHandler(void);
    WEAK void GPIO5_Combined_16_31_IRQHandler(void);
    WEAK void FLEXIO1_IRQHandler(void);
    WEAK void FLEXIO2_IRQHandler(void);
    WEAK void WDOG1_IRQHandler(void);
    WEAK void RTWDOG_IRQHandler(void);
    WEAK void EWM_IRQHandler(void);
    WEAK void CCM_1_IRQHandler(void);
    WEAK void CCM_2_IRQHandler(void);
    WEAK void GPC_IRQHandler(void);
    WEAK void SRC_IRQHandler(void);
    WEAK void Reserved115_IRQHandler(void);
    WEAK void GPT1_IRQHandler(void);
    WEAK void GPT2_IRQHandler(void);
    WEAK void PWM1_0_IRQHandler(void);
    WEAK void PWM1_1_IRQHandler(void);
    WEAK void PWM1_2_IRQHandler(void);
    WEAK void PWM1_3_IRQHandler(void);
    WEAK void PWM1_FAULT_IRQHandler(void);
    WEAK void Reserved123_IRQHandler(void);
    WEAK void FLEXSPI_IRQHandler(void);
    WEAK void SEMC_IRQHandler(void);
    WEAK void USDHC1_IRQHandler(void);
    WEAK void USDHC2_IRQHandler(void);
    WEAK void USB_OTG2_IRQHandler(void);
    WEAK void USB_OTG1_IRQHandler(void);
    WEAK void ENET_IRQHandler(void);
    WEAK void ENET_1588_Timer_IRQHandler(void);
    WEAK void XBAR1_IRQ_0_1_IRQHandler(void);
    WEAK void XBAR1_IRQ_2_3_IRQHandler(void);
    WEAK void ADC_ETC_IRQ0_IRQHandler(void);
    WEAK void ADC_ETC_IRQ1_IRQHandler(void);
    WEAK void ADC_ETC_IRQ2_IRQHandler(void);
    WEAK void ADC_ETC_ERROR_IRQ_IRQHandler(void);
    WEAK void PIT_IRQHandler(void);
    WEAK void ACMP1_IRQHandler(void);
    WEAK void ACMP2_IRQHandler(void);
    WEAK void ACMP3_IRQHandler(void);
    WEAK void ACMP4_IRQHandler(void);
    WEAK void Reserved143_IRQHandler(void);
    WEAK void Reserved144_IRQHandler(void);
    WEAK void ENC1_IRQHandler(void);
    WEAK void ENC2_IRQHandler(void);
    WEAK void ENC3_IRQHandler(void);
    WEAK void ENC4_IRQHandler(void);
    WEAK void TMR1_IRQHandler(void);
    WEAK void TMR2_IRQHandler(void);
    WEAK void TMR3_IRQHandler(void);
    WEAK void TMR4_IRQHandler(void);
    WEAK void PWM2_0_IRQHandler(void);
    WEAK void PWM2_1_IRQHandler(void);
    WEAK void PWM2_2_IRQHandler(void);
    WEAK void PWM2_3_IRQHandler(void);
    WEAK void PWM2_FAULT_IRQHandler(void);
    WEAK void PWM3_0_IRQHandler(void);
    WEAK void PWM3_1_IRQHandler(void);
    WEAK void PWM3_2_IRQHandler(void);
    WEAK void PWM3_3_IRQHandler(void);
    WEAK void PWM3_FAULT_IRQHandler(void);
    WEAK void PWM4_0_IRQHandler(void);
    WEAK void PWM4_1_IRQHandler(void);
    WEAK void PWM4_2_IRQHandler(void);
    WEAK void PWM4_3_IRQHandler(void);
    WEAK void PWM4_FAULT_IRQHandler(void);
    WEAK void Reserved168_IRQHandler(void);
    WEAK void Reserved169_IRQHandler(void);
    WEAK void Reserved170_IRQHandler(void);
    WEAK void Reserved171_IRQHandler(void);
    WEAK void Reserved172_IRQHandler(void);
    WEAK void Reserved173_IRQHandler(void);
    WEAK void SJC_ARM_DEBUG_IRQHandler(void);
    WEAK void NMI_WAKEUP_IRQHandler(void);

    //*****************************************************************************
    // Forward declaration of the driver IRQ handlers. These are aliased
    // to the IntDefaultHandler, which is a 'forever' loop. When the driver
    // defines a handler (with the same name), this will automatically take
    // precedence over these weak definitions
    //*****************************************************************************
    void DMA0_DMA16_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void DMA1_DMA17_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void DMA2_DMA18_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void DMA3_DMA19_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void DMA4_DMA20_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void DMA5_DMA21_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void DMA6_DMA22_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void DMA7_DMA23_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void DMA8_DMA24_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void DMA9_DMA25_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void DMA10_DMA26_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void DMA11_DMA27_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void DMA12_DMA28_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void DMA13_DMA29_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void DMA14_DMA30_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void DMA15_DMA31_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void DMA_ERROR_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void CTI0_ERROR_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void CTI1_ERROR_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void CORE_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void LPUART1_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void LPUART2_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void LPUART3_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void LPUART4_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void LPUART5_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void LPUART6_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void LPUART7_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void LPUART8_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void LPI2C1_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void LPI2C2_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void LPI2C3_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void LPI2C4_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void LPSPI1_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void LPSPI2_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void LPSPI3_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void LPSPI4_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void CAN1_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void CAN2_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void FLEXRAM_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void KPP_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void TSC_DIG_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void GPR_IRQ_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void LCDIF_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void CSI_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void PXP_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void WDOG2_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void SNVS_HP_WRAPPER_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void SNVS_HP_WRAPPER_TZ_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void SNVS_LP_WRAPPER_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void CSU_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void DCP_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void DCP_VMI_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void Reserved68_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void TRNG_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void SJC_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void BEE_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void SAI1_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void SAI2_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void SAI3_RX_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void SAI3_TX_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void SPDIF_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void ANATOP_EVENT0_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void ANATOP_EVENT1_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void ANATOP_TAMP_LOW_HIGH_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void ANATOP_TEMP_PANIC_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void USB_PHY1_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void USB_PHY2_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void ADC1_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void ADC2_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void DCDC_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void Reserved86_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void Reserved87_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void GPIO1_INT0_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void GPIO1_INT1_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void GPIO1_INT2_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void GPIO1_INT3_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void GPIO1_INT4_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void GPIO1_INT5_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void GPIO1_INT6_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void GPIO1_INT7_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void GPIO1_Combined_0_15_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void GPIO1_Combined_16_31_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void GPIO2_Combined_0_15_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void GPIO2_Combined_16_31_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void GPIO3_Combined_0_15_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void GPIO3_Combined_16_31_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void GPIO4_Combined_0_15_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void GPIO4_Combined_16_31_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void GPIO5_Combined_0_15_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void GPIO5_Combined_16_31_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void FLEXIO1_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void FLEXIO2_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void WDOG1_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void RTWDOG_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void EWM_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void CCM_1_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void CCM_2_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void GPC_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void SRC_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void Reserved115_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void GPT1_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void GPT2_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void PWM1_0_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void PWM1_1_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void PWM1_2_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void PWM1_3_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void PWM1_FAULT_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void Reserved123_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void FLEXSPI_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void SEMC_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void USDHC1_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void USDHC2_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void USB_OTG2_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void USB_OTG1_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void ENET_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void ENET_1588_Timer_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void XBAR1_IRQ_0_1_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void XBAR1_IRQ_2_3_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void ADC_ETC_IRQ0_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void ADC_ETC_IRQ1_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void ADC_ETC_IRQ2_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void ADC_ETC_ERROR_IRQ_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void PIT_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void ACMP1_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void ACMP2_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void ACMP3_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void ACMP4_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void Reserved143_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void Reserved144_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void ENC1_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void ENC2_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void ENC3_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void ENC4_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void TMR1_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void TMR2_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void TMR3_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void TMR4_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void PWM2_0_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void PWM2_1_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void PWM2_2_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void PWM2_3_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void PWM2_FAULT_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void PWM3_0_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void PWM3_1_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void PWM3_2_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void PWM3_3_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void PWM3_FAULT_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void PWM4_0_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void PWM4_1_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void PWM4_2_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void PWM4_3_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void PWM4_FAULT_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void Reserved168_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void Reserved169_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void Reserved170_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void Reserved171_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void Reserved172_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void Reserved173_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void SJC_ARM_DEBUG_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
    void NMI_WAKEUP_DriverIRQHandler(void) ALIAS(IntDefaultHandler);

//*****************************************************************************
// The entry point for the application.
// __main() is the entry point for Redlib based applications
// main() is the entry point for Newlib based applications
//*****************************************************************************
#if defined(__REDLIB__)
    extern void __main(void);
#endif
    extern int main(void);
    extern int unit_tests_main(void);

    //*****************************************************************************
    // External declaration for the pointer to the stack top from the Linker Script
    //*****************************************************************************
    extern void _vStackTop(void);
//*****************************************************************************
#if defined(__cplusplus)
} // extern "C"
#endif

#include "MIMXRT1051.h"

//*****************************************************************************
// The vector table.
// This relies on the linker script to place at correct location in memory.
//*****************************************************************************
extern void (*const g_pfnVectors[])(void);
extern void *__Vectors __attribute__((alias("g_pfnVectors")));

__attribute__((used, section(".isr_vector"))) void (*const g_pfnVectors[])(void) = {
    // Core Level - CM7
    &_vStackTop,        // The initial stack pointer
    ResetISR,           // The reset handler
    NMI_Handler,        // The NMI handler
    HardFault_Handler,  // The hard fault handler
    MemManage_Handler,  // The MPU fault handler
    BusFault_Handler,   // The bus fault handler
    UsageFault_Handler, // The usage fault handler
    0,                  // Reserved
    0,                  // Reserved
    0,                  // Reserved
    0,                  // Reserved
    SVC_Handler,        // SVCall handler
    0,                  // Reserved
    0,                  // Reserved
    PendSV_Handler,     // The PendSV handler
    SysTick_Handler,    // The SysTick handler

    // Chip Level - MIMXRT1052
    DMA0_DMA16_IRQHandler,           // 16 : DMA channel 0/16 transfer complete
    DMA1_DMA17_IRQHandler,           // 17 : DMA channel 1/17 transfer complete
    DMA2_DMA18_IRQHandler,           // 18 : DMA channel 2/18 transfer complete
    DMA3_DMA19_IRQHandler,           // 19 : DMA channel 3/19 transfer complete
    DMA4_DMA20_IRQHandler,           // 20 : DMA channel 4/20 transfer complete
    DMA5_DMA21_IRQHandler,           // 21 : DMA channel 5/21 transfer complete
    DMA6_DMA22_IRQHandler,           // 22 : DMA channel 6/22 transfer complete
    DMA7_DMA23_IRQHandler,           // 23 : DMA channel 7/23 transfer complete
    DMA8_DMA24_IRQHandler,           // 24 : DMA channel 8/24 transfer complete
    DMA9_DMA25_IRQHandler,           // 25 : DMA channel 9/25 transfer complete
    DMA10_DMA26_IRQHandler,          // 26 : DMA channel 10/26 transfer complete
    DMA11_DMA27_IRQHandler,          // 27 : DMA channel 11/27 transfer complete
    DMA12_DMA28_IRQHandler,          // 28 : DMA channel 12/28 transfer complete
    DMA13_DMA29_IRQHandler,          // 29 : DMA channel 13/29 transfer complete
    DMA14_DMA30_IRQHandler,          // 30 : DMA channel 14/30 transfer complete
    DMA15_DMA31_IRQHandler,          // 31 : DMA channel 15/31 transfer complete
    DMA_ERROR_IRQHandler,            // 32 : DMA error interrupt channels 0-15 / 16-31
    CTI0_ERROR_IRQHandler,           // 33 : CTI0_Error
    CTI1_ERROR_IRQHandler,           // 34 : CTI1_Error
    CORE_IRQHandler,                 // 35 : CorePlatform exception IRQ
    LPUART1_IRQHandler,              // 36 : LPUART1 TX interrupt and RX interrupt
    LPUART2_IRQHandler,              // 37 : LPUART2 TX interrupt and RX interrupt
    LPUART3_IRQHandler,              // 38 : LPUART3 TX interrupt and RX interrupt
    LPUART4_IRQHandler,              // 39 : LPUART4 TX interrupt and RX interrupt
    LPUART5_IRQHandler,              // 40 : LPUART5 TX interrupt and RX interrupt
    LPUART6_IRQHandler,              // 41 : LPUART6 TX interrupt and RX interrupt
    LPUART7_IRQHandler,              // 42 : LPUART7 TX interrupt and RX interrupt
    LPUART8_IRQHandler,              // 43 : LPUART8 TX interrupt and RX interrupt
    LPI2C1_IRQHandler,               // 44 : LPI2C1 interrupt
    LPI2C2_IRQHandler,               // 45 : LPI2C2 interrupt
    LPI2C3_IRQHandler,               // 46 : LPI2C3 interrupt
    LPI2C4_IRQHandler,               // 47 : LPI2C4 interrupt
    LPSPI1_IRQHandler,               // 48 : LPSPI1 single interrupt vector for all sources
    LPSPI2_IRQHandler,               // 49 : LPSPI2 single interrupt vector for all sources
    LPSPI3_IRQHandler,               // 50 : LPSPI3 single interrupt vector for all sources
    LPSPI4_IRQHandler,               // 51 : LPSPI4  single interrupt vector for all sources
    CAN1_IRQHandler,                 // 52 : CAN1 interrupt
    CAN2_IRQHandler,                 // 53 : CAN2 interrupt
    FLEXRAM_IRQHandler,              // 54 : FlexRAM address out of range Or access hit IRQ
    KPP_IRQHandler,                  // 55 : Keypad nterrupt
    TSC_DIG_IRQHandler,              // 56 : TSC interrupt
    GPR_IRQ_IRQHandler,              // 57 : GPR interrupt
    LCDIF_IRQHandler,                // 58 : LCDIF interrupt
    CSI_IRQHandler,                  // 59 : CSI interrupt
    PXP_IRQHandler,                  // 60 : PXP interrupt
    WDOG2_IRQHandler,                // 61 : WDOG2 interrupt
    SNVS_HP_WRAPPER_IRQHandler,      // 62 : SRTC Consolidated Interrupt. Non TZ
    SNVS_HP_WRAPPER_TZ_IRQHandler,   // 63 : SRTC Security Interrupt. TZ
    SNVS_LP_WRAPPER_IRQHandler,      // 64 : ON-OFF button press shorter than 5 secs (pulse event)
    CSU_IRQHandler,                  // 65 : CSU interrupt
    DCP_IRQHandler,                  // 66 : DCP_IRQ interrupt
    DCP_VMI_IRQHandler,              // 67 : DCP_VMI_IRQ interrupt
    Reserved68_IRQHandler,           // 68 : Reserved interrupt
    TRNG_IRQHandler,                 // 69 : TRNG interrupt
    SJC_IRQHandler,                  // 70 : SJC interrupt
    BEE_IRQHandler,                  // 71 : BEE interrupt
    SAI1_IRQHandler,                 // 72 : SAI1 interrupt
    SAI2_IRQHandler,                 // 73 : SAI1 interrupt
    SAI3_RX_IRQHandler,              // 74 : SAI3 interrupt
    SAI3_TX_IRQHandler,              // 75 : SAI3 interrupt
    SPDIF_IRQHandler,                // 76 : SPDIF interrupt
    ANATOP_EVENT0_IRQHandler,        // 77 : ANATOP interrupt
    ANATOP_EVENT1_IRQHandler,        // 78 : ANATOP interrupt
    ANATOP_TAMP_LOW_HIGH_IRQHandler, // 79 : ANATOP interrupt
    ANATOP_TEMP_PANIC_IRQHandler,    // 80 : ANATOP interrupt
    USB_PHY1_IRQHandler,             // 81 : USBPHY (UTMI0), Interrupt
    USB_PHY2_IRQHandler,             // 82 : USBPHY (UTMI0), Interrupt
    ADC1_IRQHandler,                 // 83 : ADC1 interrupt
    ADC2_IRQHandler,                 // 84 : ADC2 interrupt
    DCDC_IRQHandler,                 // 85 : DCDC interrupt
    Reserved86_IRQHandler,           // 86 : Reserved interrupt
    Reserved87_IRQHandler,           // 87 : Reserved interrupt
    GPIO1_INT0_IRQHandler,           // 88 : Active HIGH Interrupt from INT0 from GPIO
    GPIO1_INT1_IRQHandler,           // 89 : Active HIGH Interrupt from INT1 from GPIO
    GPIO1_INT2_IRQHandler,           // 90 : Active HIGH Interrupt from INT2 from GPIO
    GPIO1_INT3_IRQHandler,           // 91 : Active HIGH Interrupt from INT3 from GPIO
    GPIO1_INT4_IRQHandler,           // 92 : Active HIGH Interrupt from INT4 from GPIO
    GPIO1_INT5_IRQHandler,           // 93 : Active HIGH Interrupt from INT5 from GPIO
    GPIO1_INT6_IRQHandler,           // 94 : Active HIGH Interrupt from INT6 from GPIO
    GPIO1_INT7_IRQHandler,           // 95 : Active HIGH Interrupt from INT7 from GPIO
    GPIO1_Combined_0_15_IRQHandler,  // 96 : Combined interrupt indication for GPIO1 signal 0 throughout 15
    GPIO1_Combined_16_31_IRQHandler, // 97 : Combined interrupt indication for GPIO1 signal 16 throughout 31
    GPIO2_Combined_0_15_IRQHandler,  // 98 : Combined interrupt indication for GPIO2 signal 0 throughout 15
    GPIO2_Combined_16_31_IRQHandler, // 99 : Combined interrupt indication for GPIO2 signal 16 throughout 31
    GPIO3_Combined_0_15_IRQHandler,  // 100: Combined interrupt indication for GPIO3 signal 0 throughout 15
    GPIO3_Combined_16_31_IRQHandler, // 101: Combined interrupt indication for GPIO3 signal 16 throughout 31
    GPIO4_Combined_0_15_IRQHandler,  // 102: Combined interrupt indication for GPIO4 signal 0 throughout 15
    GPIO4_Combined_16_31_IRQHandler, // 103: Combined interrupt indication for GPIO4 signal 16 throughout 31
    GPIO5_Combined_0_15_IRQHandler,  // 104: Combined interrupt indication for GPIO5 signal 0 throughout 15
    GPIO5_Combined_16_31_IRQHandler, // 105: Combined interrupt indication for GPIO5 signal 16 throughout 31
    FLEXIO1_IRQHandler,              // 106: FLEXIO1 interrupt
    FLEXIO2_IRQHandler,              // 107: FLEXIO2 interrupt
    WDOG1_IRQHandler,                // 108: WDOG1 interrupt
    RTWDOG_IRQHandler,               // 109: RTWDOG interrupt
    EWM_IRQHandler,                  // 110: EWM interrupt
    CCM_1_IRQHandler,                // 111: CCM IRQ1 interrupt
    CCM_2_IRQHandler,                // 112: CCM IRQ2 interrupt
    GPC_IRQHandler,                  // 113: GPC interrupt
    SRC_IRQHandler,                  // 114: SRC interrupt
    Reserved115_IRQHandler,          // 115: Reserved interrupt
    GPT1_IRQHandler,                 // 116: GPT1 interrupt
    GPT2_IRQHandler,                 // 117: GPT2 interrupt
    PWM1_0_IRQHandler,               // 118: PWM1 capture 0, compare 0, or reload 0 interrupt
    PWM1_1_IRQHandler,               // 119: PWM1 capture 1, compare 1, or reload 0 interrupt
    PWM1_2_IRQHandler,               // 120: PWM1 capture 2, compare 2, or reload 0 interrupt
    PWM1_3_IRQHandler,               // 121: PWM1 capture 3, compare 3, or reload 0 interrupt
    PWM1_FAULT_IRQHandler,           // 122: PWM1 fault or reload error interrupt
    Reserved123_IRQHandler,          // 123: Reserved interrupt
    FLEXSPI_IRQHandler,              // 124: FlexSPI0 interrupt
    SEMC_IRQHandler,                 // 125: Reserved interrupt
    USDHC1_IRQHandler,               // 126: USDHC1 interrupt
    USDHC2_IRQHandler,               // 127: USDHC2 interrupt
    USB_OTG2_IRQHandler,             // 128: USBO2 USB OTG2
    USB_OTG1_IRQHandler,             // 129: USBO2 USB OTG1
    ENET_IRQHandler,                 // 130: ENET interrupt
    ENET_1588_Timer_IRQHandler,      // 131: ENET_1588_Timer interrupt
    XBAR1_IRQ_0_1_IRQHandler,        // 132: XBAR1 interrupt
    XBAR1_IRQ_2_3_IRQHandler,        // 133: XBAR1 interrupt
    ADC_ETC_IRQ0_IRQHandler,         // 134: ADCETC IRQ0 interrupt
    ADC_ETC_IRQ1_IRQHandler,         // 135: ADCETC IRQ1 interrupt
    ADC_ETC_IRQ2_IRQHandler,         // 136: ADCETC IRQ2 interrupt
    ADC_ETC_ERROR_IRQ_IRQHandler,    // 137: ADCETC Error IRQ interrupt
    PIT_IRQHandler,                  // 138: PIT interrupt
    ACMP1_IRQHandler,                // 139: ACMP interrupt
    ACMP2_IRQHandler,                // 140: ACMP interrupt
    ACMP3_IRQHandler,                // 141: ACMP interrupt
    ACMP4_IRQHandler,                // 142: ACMP interrupt
    Reserved143_IRQHandler,          // 143: Reserved interrupt
    Reserved144_IRQHandler,          // 144: Reserved interrupt
    ENC1_IRQHandler,                 // 145: ENC1 interrupt
    ENC2_IRQHandler,                 // 146: ENC2 interrupt
    ENC3_IRQHandler,                 // 147: ENC3 interrupt
    ENC4_IRQHandler,                 // 148: ENC4 interrupt
    TMR1_IRQHandler,                 // 149: TMR1 interrupt
    TMR2_IRQHandler,                 // 150: TMR2 interrupt
    TMR3_IRQHandler,                 // 151: TMR3 interrupt
    TMR4_IRQHandler,                 // 152: TMR4 interrupt
    PWM2_0_IRQHandler,               // 153: PWM2 capture 0, compare 0, or reload 0 interrupt
    PWM2_1_IRQHandler,               // 154: PWM2 capture 1, compare 1, or reload 0 interrupt
    PWM2_2_IRQHandler,               // 155: PWM2 capture 2, compare 2, or reload 0 interrupt
    PWM2_3_IRQHandler,               // 156: PWM2 capture 3, compare 3, or reload 0 interrupt
    PWM2_FAULT_IRQHandler,           // 157: PWM2 fault or reload error interrupt
    PWM3_0_IRQHandler,               // 158: PWM3 capture 0, compare 0, or reload 0 interrupt
    PWM3_1_IRQHandler,               // 159: PWM3 capture 1, compare 1, or reload 0 interrupt
    PWM3_2_IRQHandler,               // 160: PWM3 capture 2, compare 2, or reload 0 interrupt
    PWM3_3_IRQHandler,               // 161: PWM3 capture 3, compare 3, or reload 0 interrupt
    PWM3_FAULT_IRQHandler,           // 162: PWM3 fault or reload error interrupt
    PWM4_0_IRQHandler,               // 163: PWM4 capture 0, compare 0, or reload 0 interrupt
    PWM4_1_IRQHandler,               // 164: PWM4 capture 1, compare 1, or reload 0 interrupt
    PWM4_2_IRQHandler,               // 165: PWM4 capture 2, compare 2, or reload 0 interrupt
    PWM4_3_IRQHandler,               // 166: PWM4 capture 3, compare 3, or reload 0 interrupt
    PWM4_FAULT_IRQHandler,           // 167: PWM4 fault or reload error interrupt
    Reserved168_IRQHandler,          // 168: Reserved interrupt
    Reserved169_IRQHandler,          // 169: Reserved interrupt
    Reserved170_IRQHandler,          // 170: Reserved interrupt
    Reserved171_IRQHandler,          // 171: Reserved interrupt
    Reserved172_IRQHandler,          // 172: Reserved interrupt
    Reserved173_IRQHandler,          // 173: Reserved interrupt
    SJC_ARM_DEBUG_IRQHandler,        // 174: SJC ARM debug interrupt
    NMI_WAKEUP_IRQHandler,           // 175: NMI wake up
};                                   /* End of g_pfnVectors */

//*****************************************************************************
// Functions to carry out the initialization of RW and BSS data sections. These
// are written as separate functions rather than being inlined within the
// ResetISR() function in order to cope with MCUs with multiple banks of
// memory.
//*****************************************************************************
__attribute__((section(".after_vectors.init_data"))) void data_init(unsigned int romstart,
                                                                    unsigned int start,
                                                                    unsigned int len)
{
    unsigned int *pulDest = (unsigned int *)start;
    unsigned int *pulSrc  = (unsigned int *)romstart;
    unsigned int loop;
    for (loop = 0; loop < len; loop = loop + 4) {
        *pulDest++ = *pulSrc++;
    }
}

__attribute__((section(".after_vectors.init_bss"))) void bss_init(unsigned int start, unsigned int len)
{
    unsigned int *pulDest = (unsigned int *)start;
    unsigned int loop;
    for (loop = 0; loop < len; loop = loop + 4) {
        *pulDest++ = 0;
    }
}

//*****************************************************************************
// The following symbols are constructs generated by the linker, indicating
// the location of various points in the "Global Section Table". This table is
// created by the linker via the Code Red managed linker script mechanism. It
// contains the load address, execution address and length of each RW data
// section and the execution and length of each BSS (zero initialized) section.
//*****************************************************************************
extern unsigned int __textram_section_table;
extern unsigned int __textram_section_table_end;
extern unsigned int __data_section_table;
extern unsigned int __data_section_table_end;
extern unsigned int __bss_section_table;
extern unsigned int __bss_section_table_end;

//*****************************************************************************
// Reset entry point for your code.
// Sets up a simple runtime environment and initializes the C/C++
// library.
//*****************************************************************************
__attribute__((section(".after_vectors.reset"))) void ResetISR(void)
{
    // Disable interrupts
    __asm volatile("cpsid i");

    SystemInit();

    unsigned int LoadAddr, ExeAddr, SectionLen;
    unsigned int *SectionTableAddr;

    // Load base address of Global Section Table
    SectionTableAddr = &__data_section_table;

    // Copy the data sections from flash to SRAM.
    while (SectionTableAddr < &__data_section_table_end) {
        LoadAddr   = *SectionTableAddr++;
        ExeAddr    = *SectionTableAddr++;
        SectionLen = *SectionTableAddr++;
        if (LoadAddr != ExeAddr) {
            data_init(LoadAddr, ExeAddr, SectionLen);
        }
    }

    // Initialize BSS section
    SectionTableAddr = &__bss_section_table;
    // Zero fill the bss segment
    while (SectionTableAddr < &__bss_section_table_end) {
        ExeAddr    = *SectionTableAddr++;
        SectionLen = *SectionTableAddr++;
        bss_init(ExeAddr, SectionLen);
    }

#if defined(__cplusplus)
    //
    // Call C++ library initialisation
    //
    __libc_init_array();
#endif

    // Reenable interrupts
    __asm volatile("cpsie i");

#if defined(__REDLIB__)
    // Call the Redlib library, which in turn calls main()
    __main();
#elif PROJECT_CONFIG_UNIT_TESTS_BUILD == 1
    unit_tests_main();
#else
    main();
#endif
    // call global destructors and functions registered using atexit
    __call_exitprocs(0, 0);
    _platform_exit();
    while (true) {}
}

//*****************************************************************************
// Default core exception handlers. Override the ones here by defining your own
// handler routines in your application code.
//*****************************************************************************
WEAK_AV void NMI_Handler(void)
{
    while (1) {}
}

WEAK_AV void SVC_Handler(void)
{
    while (1) {}
}

WEAK_AV void PendSV_Handler(void)
{
    while (1) {}
}

WEAK_AV void SysTick_Handler(void)
{
    while (1) {}
}

//*****************************************************************************
// Processor ends up here if an unexpected interrupt occurs or a specific
// handler is not present in the application code.
//*****************************************************************************
WEAK_AV void IntDefaultHandler(void)
{
    while (1) {}
}

//*****************************************************************************
// Default application exception handlers. Override the ones here by defining
// your own handler routines in your application code. These routines call
// driver exception handlers or IntDefaultHandler() if no driver exception
// handler is included.
//*****************************************************************************
WEAK void DMA0_DMA16_IRQHandler(void)
{
    DMA0_DMA16_DriverIRQHandler();
}

WEAK void DMA1_DMA17_IRQHandler(void)
{
    DMA1_DMA17_DriverIRQHandler();
}

WEAK void DMA2_DMA18_IRQHandler(void)
{
    DMA2_DMA18_DriverIRQHandler();
}

WEAK void DMA3_DMA19_IRQHandler(void)
{
    DMA3_DMA19_DriverIRQHandler();
}

WEAK void DMA4_DMA20_IRQHandler(void)
{
    DMA4_DMA20_DriverIRQHandler();
}

WEAK void DMA5_DMA21_IRQHandler(void)
{
    DMA5_DMA21_DriverIRQHandler();
}

WEAK void DMA6_DMA22_IRQHandler(void)
{
    DMA6_DMA22_DriverIRQHandler();
}

WEAK void DMA7_DMA23_IRQHandler(void)
{
    DMA7_DMA23_DriverIRQHandler();
}

WEAK void DMA8_DMA24_IRQHandler(void)
{
    DMA8_DMA24_DriverIRQHandler();
}

WEAK void DMA9_DMA25_IRQHandler(void)
{
    DMA9_DMA25_DriverIRQHandler();
}

WEAK void DMA10_DMA26_IRQHandler(void)
{
    DMA10_DMA26_DriverIRQHandler();
}

WEAK void DMA11_DMA27_IRQHandler(void)
{
    DMA11_DMA27_DriverIRQHandler();
}

WEAK void DMA12_DMA28_IRQHandler(void)
{
    DMA12_DMA28_DriverIRQHandler();
}

WEAK void DMA13_DMA29_IRQHandler(void)
{
    DMA13_DMA29_DriverIRQHandler();
}

WEAK void DMA14_DMA30_IRQHandler(void)
{
    DMA14_DMA30_DriverIRQHandler();
}

WEAK void DMA15_DMA31_IRQHandler(void)
{
    DMA15_DMA31_DriverIRQHandler();
}

WEAK void DMA_ERROR_IRQHandler(void)
{
    DMA_ERROR_DriverIRQHandler();
}

WEAK void CTI0_ERROR_IRQHandler(void)
{
    CTI0_ERROR_DriverIRQHandler();
}

WEAK void CTI1_ERROR_IRQHandler(void)
{
    CTI1_ERROR_DriverIRQHandler();
}

WEAK void CORE_IRQHandler(void)
{
    CORE_DriverIRQHandler();
}

WEAK void LPUART1_IRQHandler(void)
{
    LPUART1_DriverIRQHandler();
}

WEAK void LPUART2_IRQHandler(void)
{
    LPUART2_DriverIRQHandler();
}

WEAK void LPUART3_IRQHandler(void)
{
    LPUART3_DriverIRQHandler();
}

WEAK void LPUART4_IRQHandler(void)
{
    LPUART4_DriverIRQHandler();
}

WEAK void LPUART5_IRQHandler(void)
{
    LPUART5_DriverIRQHandler();
}

WEAK void LPUART6_IRQHandler(void)
{
    LPUART6_DriverIRQHandler();
}

WEAK void LPUART7_IRQHandler(void)
{
    LPUART7_DriverIRQHandler();
}

WEAK void LPUART8_IRQHandler(void)
{
    LPUART8_DriverIRQHandler();
}

WEAK void LPI2C1_IRQHandler(void)
{
    LPI2C1_DriverIRQHandler();
}

WEAK void LPI2C2_IRQHandler(void)
{
    LPI2C2_DriverIRQHandler();
}

WEAK void LPI2C3_IRQHandler(void)
{
    LPI2C3_DriverIRQHandler();
}

WEAK void LPI2C4_IRQHandler(void)
{
    LPI2C4_DriverIRQHandler();
}

WEAK void LPSPI1_IRQHandler(void)
{
    LPSPI1_DriverIRQHandler();
}

WEAK void LPSPI2_IRQHandler(void)
{
    LPSPI2_DriverIRQHandler();
}

WEAK void LPSPI3_IRQHandler(void)
{
    LPSPI3_DriverIRQHandler();
}

WEAK void LPSPI4_IRQHandler(void)
{
    LPSPI4_DriverIRQHandler();
}

WEAK void CAN1_IRQHandler(void)
{
    CAN1_DriverIRQHandler();
}

WEAK void CAN2_IRQHandler(void)
{
    CAN2_DriverIRQHandler();
}

WEAK void FLEXRAM_IRQHandler(void)
{
    FLEXRAM_DriverIRQHandler();
}

WEAK void KPP_IRQHandler(void)
{
    KPP_DriverIRQHandler();
}

WEAK void TSC_DIG_IRQHandler(void)
{
    TSC_DIG_DriverIRQHandler();
}

WEAK void GPR_IRQ_IRQHandler(void)
{
    GPR_IRQ_DriverIRQHandler();
}

WEAK void LCDIF_IRQHandler(void)
{
    LCDIF_DriverIRQHandler();
}

WEAK void CSI_IRQHandler(void)
{
    CSI_DriverIRQHandler();
}

WEAK void PXP_IRQHandler(void)
{
    PXP_DriverIRQHandler();
}

WEAK void WDOG2_IRQHandler(void)
{
    WDOG2_DriverIRQHandler();
}

WEAK void SNVS_HP_WRAPPER_IRQHandler(void)
{
    SNVS_HP_WRAPPER_DriverIRQHandler();
}

WEAK void SNVS_HP_WRAPPER_TZ_IRQHandler(void)
{
    SNVS_HP_WRAPPER_TZ_DriverIRQHandler();
}

WEAK void SNVS_LP_WRAPPER_IRQHandler(void)
{
    SNVS_LP_WRAPPER_DriverIRQHandler();
}

WEAK void CSU_IRQHandler(void)
{
    CSU_DriverIRQHandler();
}

WEAK void DCP_IRQHandler(void)
{
    DCP_DriverIRQHandler();
}

WEAK void DCP_VMI_IRQHandler(void)
{
    DCP_VMI_DriverIRQHandler();
}

WEAK void Reserved68_IRQHandler(void)
{
    Reserved68_DriverIRQHandler();
}

WEAK void TRNG_IRQHandler(void)
{
    TRNG_DriverIRQHandler();
}

WEAK void SJC_IRQHandler(void)
{
    SJC_DriverIRQHandler();
}

WEAK void BEE_IRQHandler(void)
{
    BEE_DriverIRQHandler();
}

WEAK void SAI1_IRQHandler(void)
{
    SAI1_DriverIRQHandler();
}

WEAK void SAI2_IRQHandler(void)
{
    SAI2_DriverIRQHandler();
}

WEAK void SAI3_RX_IRQHandler(void)
{
    SAI3_RX_DriverIRQHandler();
}

WEAK void SAI3_TX_IRQHandler(void)
{
    SAI3_TX_DriverIRQHandler();
}

WEAK void SPDIF_IRQHandler(void)
{
    SPDIF_DriverIRQHandler();
}

WEAK void ANATOP_EVENT0_IRQHandler(void)
{
    ANATOP_EVENT0_DriverIRQHandler();
}

WEAK void ANATOP_EVENT1_IRQHandler(void)
{
    ANATOP_EVENT1_DriverIRQHandler();
}

WEAK void ANATOP_TAMP_LOW_HIGH_IRQHandler(void)
{
    ANATOP_TAMP_LOW_HIGH_DriverIRQHandler();
}

WEAK void ANATOP_TEMP_PANIC_IRQHandler(void)
{
    ANATOP_TEMP_PANIC_DriverIRQHandler();
}

WEAK void USB_PHY1_IRQHandler(void)
{
    USB_PHY1_DriverIRQHandler();
}

WEAK void USB_PHY2_IRQHandler(void)
{
    USB_PHY2_DriverIRQHandler();
}

WEAK void ADC1_IRQHandler(void)
{
    ADC1_DriverIRQHandler();
}

WEAK void ADC2_IRQHandler(void)
{
    ADC2_DriverIRQHandler();
}

WEAK void DCDC_IRQHandler(void)
{
    DCDC_DriverIRQHandler();
}

WEAK void Reserved86_IRQHandler(void)
{
    Reserved86_DriverIRQHandler();
}

WEAK void Reserved87_IRQHandler(void)
{
    Reserved87_DriverIRQHandler();
}

WEAK void GPIO1_INT0_IRQHandler(void)
{
    GPIO1_INT0_DriverIRQHandler();
}

WEAK void GPIO1_INT1_IRQHandler(void)
{
    GPIO1_INT1_DriverIRQHandler();
}

WEAK void GPIO1_INT2_IRQHandler(void)
{
    GPIO1_INT2_DriverIRQHandler();
}

WEAK void GPIO1_INT3_IRQHandler(void)
{
    GPIO1_INT3_DriverIRQHandler();
}

WEAK void GPIO1_INT4_IRQHandler(void)
{
    GPIO1_INT4_DriverIRQHandler();
}

WEAK void GPIO1_INT5_IRQHandler(void)
{
    GPIO1_INT5_DriverIRQHandler();
}

WEAK void GPIO1_INT6_IRQHandler(void)
{
    GPIO1_INT6_DriverIRQHandler();
}

WEAK void GPIO1_INT7_IRQHandler(void)
{
    GPIO1_INT7_DriverIRQHandler();
}

WEAK void GPIO1_Combined_0_15_IRQHandler(void)
{
    GPIO1_Combined_0_15_DriverIRQHandler();
}

WEAK void GPIO1_Combined_16_31_IRQHandler(void)
{
    GPIO1_Combined_16_31_DriverIRQHandler();
}

WEAK void GPIO2_Combined_0_15_IRQHandler(void)
{
    GPIO2_Combined_0_15_DriverIRQHandler();
}

WEAK void GPIO2_Combined_16_31_IRQHandler(void)
{
    GPIO2_Combined_16_31_DriverIRQHandler();
}

WEAK void GPIO3_Combined_0_15_IRQHandler(void)
{
    GPIO3_Combined_0_15_DriverIRQHandler();
}

WEAK void GPIO3_Combined_16_31_IRQHandler(void)
{
    GPIO3_Combined_16_31_DriverIRQHandler();
}

WEAK void GPIO4_Combined_0_15_IRQHandler(void)
{
    GPIO4_Combined_0_15_DriverIRQHandler();
}

WEAK void GPIO4_Combined_16_31_IRQHandler(void)
{
    GPIO4_Combined_16_31_DriverIRQHandler();
}

WEAK void GPIO5_Combined_0_15_IRQHandler(void)
{
    GPIO5_Combined_0_15_DriverIRQHandler();
}

WEAK void GPIO5_Combined_16_31_IRQHandler(void)
{
    GPIO5_Combined_16_31_DriverIRQHandler();
}

WEAK void FLEXIO1_IRQHandler(void)
{
    FLEXIO1_DriverIRQHandler();
}

WEAK void FLEXIO2_IRQHandler(void)
{
    FLEXIO2_DriverIRQHandler();
}

WEAK void WDOG1_IRQHandler(void)
{
    WDOG1_DriverIRQHandler();
}

WEAK void RTWDOG_IRQHandler(void)
{
    RTWDOG_DriverIRQHandler();
}

WEAK void EWM_IRQHandler(void)
{
    EWM_DriverIRQHandler();
}

WEAK void CCM_1_IRQHandler(void)
{
    CCM_1_DriverIRQHandler();
}

WEAK void CCM_2_IRQHandler(void)
{
    CCM_2_DriverIRQHandler();
}

WEAK void GPC_IRQHandler(void)
{
    GPC_DriverIRQHandler();
}

WEAK void SRC_IRQHandler(void)
{
    SRC_DriverIRQHandler();
}

WEAK void Reserved115_IRQHandler(void)
{
    Reserved115_DriverIRQHandler();
}

WEAK void GPT1_IRQHandler(void)
{
    GPT1_DriverIRQHandler();
}

WEAK void GPT2_IRQHandler(void)
{
    GPT2_DriverIRQHandler();
}

WEAK void PWM1_0_IRQHandler(void)
{
    PWM1_0_DriverIRQHandler();
}

WEAK void PWM1_1_IRQHandler(void)
{
    PWM1_1_DriverIRQHandler();
}

WEAK void PWM1_2_IRQHandler(void)
{
    PWM1_2_DriverIRQHandler();
}

WEAK void PWM1_3_IRQHandler(void)
{
    PWM1_3_DriverIRQHandler();
}

WEAK void PWM1_FAULT_IRQHandler(void)
{
    PWM1_FAULT_DriverIRQHandler();
}

WEAK void Reserved123_IRQHandler(void)
{
    Reserved123_DriverIRQHandler();
}

WEAK void FLEXSPI_IRQHandler(void)
{
    FLEXSPI_DriverIRQHandler();
}

WEAK void SEMC_IRQHandler(void)
{
    SEMC_DriverIRQHandler();
}

WEAK void USDHC1_IRQHandler(void)
{
    USDHC1_DriverIRQHandler();
}

WEAK void USDHC2_IRQHandler(void)
{
    USDHC2_DriverIRQHandler();
}

WEAK void USB_OTG2_IRQHandler(void)
{
    USB_OTG2_DriverIRQHandler();
}

WEAK void USB_OTG1_IRQHandler(void)
{
    USB_OTG1_DriverIRQHandler();
}

WEAK void ENET_IRQHandler(void)
{
    ENET_DriverIRQHandler();
}

WEAK void ENET_1588_Timer_IRQHandler(void)
{
    ENET_1588_Timer_DriverIRQHandler();
}

WEAK void XBAR1_IRQ_0_1_IRQHandler(void)
{
    XBAR1_IRQ_0_1_DriverIRQHandler();
}

WEAK void XBAR1_IRQ_2_3_IRQHandler(void)
{
    XBAR1_IRQ_2_3_DriverIRQHandler();
}

WEAK void ADC_ETC_IRQ0_IRQHandler(void)
{
    ADC_ETC_IRQ0_DriverIRQHandler();
}

WEAK void ADC_ETC_IRQ1_IRQHandler(void)
{
    ADC_ETC_IRQ1_DriverIRQHandler();
}

WEAK void ADC_ETC_IRQ2_IRQHandler(void)
{
    ADC_ETC_IRQ2_DriverIRQHandler();
}

WEAK void ADC_ETC_ERROR_IRQ_IRQHandler(void)
{
    ADC_ETC_ERROR_IRQ_DriverIRQHandler();
}

WEAK void PIT_IRQHandler(void)
{
    PIT_DriverIRQHandler();
}

WEAK void ACMP1_IRQHandler(void)
{
    ACMP1_DriverIRQHandler();
}

WEAK void ACMP2_IRQHandler(void)
{
    ACMP2_DriverIRQHandler();
}

WEAK void ACMP3_IRQHandler(void)
{
    ACMP3_DriverIRQHandler();
}

WEAK void ACMP4_IRQHandler(void)
{
    ACMP4_DriverIRQHandler();
}

WEAK void Reserved143_IRQHandler(void)
{
    Reserved143_DriverIRQHandler();
}

WEAK void Reserved144_IRQHandler(void)
{
    Reserved144_DriverIRQHandler();
}

WEAK void ENC1_IRQHandler(void)
{
    ENC1_DriverIRQHandler();
}

WEAK void ENC2_IRQHandler(void)
{
    ENC2_DriverIRQHandler();
}

WEAK void ENC3_IRQHandler(void)
{
    ENC3_DriverIRQHandler();
}

WEAK void ENC4_IRQHandler(void)
{
    ENC4_DriverIRQHandler();
}

WEAK void TMR1_IRQHandler(void)
{
    TMR1_DriverIRQHandler();
}

WEAK void TMR2_IRQHandler(void)
{
    TMR2_DriverIRQHandler();
}

WEAK void TMR3_IRQHandler(void)
{
    TMR3_DriverIRQHandler();
}

WEAK void TMR4_IRQHandler(void)
{
    TMR4_DriverIRQHandler();
}

WEAK void PWM2_0_IRQHandler(void)
{
    PWM2_0_DriverIRQHandler();
}

WEAK void PWM2_1_IRQHandler(void)
{
    PWM2_1_DriverIRQHandler();
}

WEAK void PWM2_2_IRQHandler(void)
{
    PWM2_2_DriverIRQHandler();
}

WEAK void PWM2_3_IRQHandler(void)
{
    PWM2_3_DriverIRQHandler();
}

WEAK void PWM2_FAULT_IRQHandler(void)
{
    PWM2_FAULT_DriverIRQHandler();
}

WEAK void PWM3_0_IRQHandler(void)
{
    PWM3_0_DriverIRQHandler();
}

WEAK void PWM3_1_IRQHandler(void)
{
    PWM3_1_DriverIRQHandler();
}

WEAK void PWM3_2_IRQHandler(void)
{
    PWM3_2_DriverIRQHandler();
}

WEAK void PWM3_3_IRQHandler(void)
{
    PWM3_3_DriverIRQHandler();
}

WEAK void PWM3_FAULT_IRQHandler(void)
{
    PWM3_FAULT_DriverIRQHandler();
}

WEAK void PWM4_0_IRQHandler(void)
{
    PWM4_0_DriverIRQHandler();
}

WEAK void PWM4_1_IRQHandler(void)
{
    PWM4_1_DriverIRQHandler();
}

WEAK void PWM4_2_IRQHandler(void)
{
    PWM4_2_DriverIRQHandler();
}

WEAK void PWM4_3_IRQHandler(void)
{
    PWM4_3_DriverIRQHandler();
}

WEAK void PWM4_FAULT_IRQHandler(void)
{
    PWM4_FAULT_DriverIRQHandler();
}

WEAK void Reserved168_IRQHandler(void)
{
    Reserved168_DriverIRQHandler();
}

WEAK void Reserved169_IRQHandler(void)
{
    Reserved169_DriverIRQHandler();
}

WEAK void Reserved170_IRQHandler(void)
{
    Reserved170_DriverIRQHandler();
}

WEAK void Reserved171_IRQHandler(void)
{
    Reserved171_DriverIRQHandler();
}

WEAK void Reserved172_IRQHandler(void)
{
    Reserved172_DriverIRQHandler();
}

WEAK void Reserved173_IRQHandler(void)
{
    Reserved173_DriverIRQHandler();
}

WEAK void SJC_ARM_DEBUG_IRQHandler(void)
{
    SJC_ARM_DEBUG_DriverIRQHandler();
}

WEAK void NMI_WAKEUP_IRQHandler(void)
{
    NMI_WAKEUP_DriverIRQHandler();
}

//*****************************************************************************

#if defined(DEBUG)
#pragma GCC pop_options
#endif // (DEBUG)
