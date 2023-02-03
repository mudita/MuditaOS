
#include "bsp.hpp"
#include "board.h"
#include "fsl_gpio.h"
#include <stdint.h>
extern "C"
{
#include "fsl_common.h"
#include "fsl_clock.h"
#include "fsl_dcdc.h"
#include "fsl_snvs_hp.h"
#include "fsl_snvs_lp.h"
#include "board/pin_mux.h"
#if LOG_LUART_ENABLED
#include "fsl_lpuart.h"
#endif
}
#include "chip.hpp"
#include "board/irq_gpio.hpp"
#include "reboot_codes.hpp"
#include "board/brownout.hpp"
#include <board/debug_console.hpp>

#include <cstdint>

extern std::uint32_t __sdram_cached_start[];

namespace bsp
{
    namespace
    {
        volatile rebootState rebootProgress{rebootState::none};
    } // namespace

    /* MPU configuration. */
    static void BOARD_ConfigMPU(void)
    {
        /* Disable I cache and D cache */
        if (SCB_CCR_IC_Msk == (SCB_CCR_IC_Msk & SCB->CCR)) {
            SCB_DisableICache();
        }
        if (SCB_CCR_DC_Msk == (SCB_CCR_DC_Msk & SCB->CCR)) {
            SCB_DisableDCache();
        }

        /* Disable MPU */
        ARM_MPU_Disable();

        /* MPU configure:
         * Use ARM_MPU_RASR(DisableExec, AccessPermission, TypeExtField, IsShareable, IsCacheable, IsBufferable,
         * SubRegionDisable, Size) API in core_cm7.h. param DisableExec       Instruction access (XN) disable
         * bit,0=instruction fetches enabled, 1=instruction fetches disabled. param AccessPermission  Data access
         * permissions, allows you to configure read/write access for User and Privileged mode. Use MACROS defined in
         * core_cm7.h: ARM_MPU_AP_NONE/ARM_MPU_AP_PRIV/ARM_MPU_AP_URO/ARM_MPU_AP_FULL/ARM_MPU_AP_PRO/ARM_MPU_AP_RO
         * Combine TypeExtField/IsShareable/IsCacheable/IsBufferable to configure MPU memory access attributes.
         *  TypeExtField  IsShareable  IsCacheable  IsBufferable   Memory Attribtue    Shareability        Cache
         *     0             x           0           0             Strongly Ordered    shareable
         *     0             x           0           1              Device             shareable
         *     0             0           1           0              Normal             not shareable   Outer and inner
         * write through no write allocate 0             0           1           1              Normal             not
         * shareable   Outer and inner write back no write allocate 0             1           1           0 Normal
         * shareable       Outer and inner write through no write allocate 0             1           1           1
         * Normal             shareable       Outer and inner write back no write allocate 1             0           0
         * 0              Normal             not shareable   outer and inner noncache 1             1           0 0
         * Normal             shareable       outer and inner noncache 1             0           1           1 Normal
         * not shareable   outer and inner write back write/read acllocate 1             1           1           1
         * Normal             shareable       outer and inner write back write/read acllocate 2             x 0 0 Device
         * not shareable Above are normal use settings, if your want to see more details or want to config different
         * inner/outter cache policy. please refer to Table 4-55 /4-56 in arm cortex-M7 generic user guide
         * <dui0646b_cortex_m7_dgug.pdf> param SubRegionDisable  Sub-region disable field. 0=sub-region is enabled,
         * 1=sub-region is disabled. param Size              Region size of the region to be configured. use
         * ARM_MPU_REGION_SIZE_xxx MACRO in core_cm7.h.
         */

        /* Region 0 setting: Memory with Device type, not shareable, non-cacheable. */
        MPU->RBAR = ARM_MPU_RBAR(0, 0xC0000000U);
        MPU->RASR = ARM_MPU_RASR(0, ARM_MPU_AP_FULL, 2, 0, 0, 0, 0, ARM_MPU_REGION_SIZE_512MB);

        /* Region 1 setting: Memory with Device type, not shareable,  non-cacheable. */
        MPU->RBAR = ARM_MPU_RBAR(1, 0x80000000U);
        MPU->RASR = ARM_MPU_RASR(0, ARM_MPU_AP_FULL, 2, 0, 0, 0, 0, ARM_MPU_REGION_SIZE_1GB);

        /* Region 2 setting */
#if defined(XIP_EXTERNAL_FLASH) && (XIP_EXTERNAL_FLASH == 1)
        /* Setting Memory with Normal type, not shareable, outer/inner write back. */
        MPU->RBAR = ARM_MPU_RBAR(2, 0x60000000U);
        MPU->RASR = ARM_MPU_RASR(0, ARM_MPU_AP_FULL, 0, 0, 1, 1, 0, ARM_MPU_REGION_SIZE_512MB);
#else
        /* Setting Memory with Device type, not shareable, non-cacheable. */
        // TODO: MPU->RBAR = ARM_MPU_RBAR(2, 0x60000000U);
        // TODO: MPU->RASR = ARM_MPU_RASR(0, ARM_MPU_AP_FULL, 2, 0, 0, 0, 0, ARM_MPU_REGION_SIZE_512MB);
#endif

        /* Region 3 setting: Memory with Device type, not shareable, non-cacheable. */
        MPU->RBAR = ARM_MPU_RBAR(3, 0x00000000U);
        MPU->RASR = ARM_MPU_RASR(0, ARM_MPU_AP_FULL, 2, 0, 0, 0, 0, ARM_MPU_REGION_SIZE_1GB);

        /* Region 4 setting: Memory with Normal type, not shareable, outer/inner write back */
        MPU->RBAR = ARM_MPU_RBAR(4, 0x00000000U);
        MPU->RASR = ARM_MPU_RASR(0, ARM_MPU_AP_FULL, 0, 0, 1, 1, 0, ARM_MPU_REGION_SIZE_128KB);

        // CPU doesn't use cache when accessing TCM memories
        /* Region 5 setting: Memory with Normal type, not shareable, outer/inner write back */
        MPU->RBAR = ARM_MPU_RBAR(5, 0x20000000U);
        MPU->RASR = ARM_MPU_RASR(0, ARM_MPU_AP_FULL, 0, 0, 1, 1, 0, ARM_MPU_REGION_SIZE_512KB);

        // OCRAM configured as non-cached segment
        /* Region 6 setting: Memory with Normal type, not shareable, outer/inner write back */
        MPU->RBAR = ARM_MPU_RBAR(6, 0x20200000U);
        MPU->RASR = ARM_MPU_RASR(0, ARM_MPU_AP_FULL, 1, 0, 0, 0, 0, ARM_MPU_REGION_SIZE_64KB);

        /* Region 7 setting: Memory with Normal type, not shareable, outer/inner write back
         * BOARD_SDRAM_TEXT
         */
        MPU->RBAR = ARM_MPU_RBAR(7, 0x80000000U);
#if defined(HW_SDRAM_64_MB) && (HW_SDRAM_64_MB == 1)
        MPU->RASR = ARM_MPU_RASR(0, ARM_MPU_AP_RO, 0, 0, 1, 1, 0, ARM_MPU_REGION_SIZE_64MB);
#else
        MPU->RASR = ARM_MPU_RASR(0, ARM_MPU_AP_RO, 0, 0, 1, 1, 0, ARM_MPU_REGION_SIZE_16MB);
#endif

        /* The define sets the cacheable memory to shareable,
         * this suggestion is referred from chapter 2.2.1 Memory regions,
         * types and attributes in Cortex-M7 Devices, Generic User Guide */
#if defined(SDRAM_IS_SHAREABLE)
        /* Region 7 setting: Memory with Normal type, not shareable, outer/inner write back */
        MPU->RBAR = ARM_MPU_RBAR(8, 0x80000000U);
        MPU->RASR = ARM_MPU_RASR(0, ARM_MPU_AP_FULL, 0, 1, 1, 1, 0, ARM_MPU_REGION_SIZE_32MB);
#else
        /* Region 9 setting: Memory with Normal type, not shareable, outer/inner write back
         * BOARD_SDRAM_HEAP
         */
        MPU->RBAR = ARM_MPU_RBAR(9, reinterpret_cast<std::uintptr_t>(__sdram_cached_start));
#if defined(HW_SDRAM_64_MB) && (HW_SDRAM_64_MB == 1)
        MPU->RASR = ARM_MPU_RASR(0, ARM_MPU_AP_FULL, 0, 0, 1, 1, 0, ARM_MPU_REGION_SIZE_64MB);
#else
        MPU->RASR = ARM_MPU_RASR(0, ARM_MPU_AP_FULL, 0, 0, 1, 1, 0, ARM_MPU_REGION_SIZE_16MB);
#endif // HW_SDRAM_64_MB
#endif // SDRAM_IS_SHAREABLE

        /* Enable MPU */
        ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk);

        SCB->SHCSR &= ~(SCB_SHCSR_MEMFAULTENA_Msk | SCB_SHCSR_USGFAULTENA_Msk | SCB_SHCSR_BUSFAULTENA_Msk);

        /* Enable I cache and D cache */
        SCB_EnableDCache();
        SCB_EnableICache();
    }

    void board_init()
    {
        PINMUX_InitBootPins();
        BOARD_InitBootClocks();
        BOARD_ConfigMPU();

        board::initDebugConsole();

        Brownout_init();
        irq_gpio_Init();

        // SNVS init. is required for proper operation of the RTC when Secure Boot is used
        SNVS_LP_Init(SNVS);
        SNVS_HP_Init(SNVS);
        SNVS_HP_ChangeSSMState(SNVS);

        // Default flag set on start in non-volatile memory to detect boot fault
        SNVS->LPGPR[0] = rebootCode::rebootFailedToBoot;

        // Set internal DCDC to DCM mode. Switching between DCM and CCM mode will be done automatically.
        DCDC_BootIntoDCM(DCDC);

        PrintSystemClocks();
        clearAndPrintBootReason();
    }

    //! Board PowerOff function by cutdown power
    void board_power_off()
    {
        rebootProgress = rebootState::poweroff;
    }

    //! Board reboot by the SVNC code
    void board_restart()
    {
        rebootProgress = rebootState::reboot;
    }

    struct PlatformExitObject
    {
        void (*func)();
    };
    static unsigned short iObject            = 0;
    constexpr auto MAX_PLATFORM_EXIT_OBJECTS = 16u;

    static PlatformExitObject objects[MAX_PLATFORM_EXIT_OBJECTS];

    int register_exit_functions(void (*func)())
    {
        if (iObject >= sizeof(objects))
            return -1;
        objects[iObject].func = func;
        ++iObject;
        return 0;
    }

    void CallPlatformExitFunctions()
    {
        while (iObject > 0) {
            objects[--iObject].func();
        }
    }

    extern "C" void _platform_exit(void)
    {
        CallPlatformExitFunctions();
        bsp::board_exit(rebootProgress);
    }

} // namespace bsp
