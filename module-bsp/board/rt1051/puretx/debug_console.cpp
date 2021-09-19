// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <board/debug_console.hpp>
#include <stdint.h>

extern "C"
{
#include "fsl_common.h"
#include "fsl_clock.h"
#if LOG_LUART_ENABLED
#include "fsl_lpuart.h"
#endif
}

namespace board
{

#if LOG_LUART_ENABLED
    namespace
    {
        lpuart_handle_t g_lpuartHandle;

        /* Get debug console frequency. */
        [[maybe_unused]] uint32_t BOARD_DebugConsoleSrcFreq(void)
        {
            uint32_t freq;

            /* To make it simple, we assume default PLL and divider settings, and the only variable
               from application is use PLL3 source or OSC source */
            if (CLOCK_GetMux(kCLOCK_UartMux) == 0) /* PLL3 div6 80M */
            {
                freq = (CLOCK_GetPllFreq(kCLOCK_PllUsb1) / 6U) / (CLOCK_GetDiv(kCLOCK_UartDiv) + 1U);
            }
            else {
                freq = CLOCK_GetOscFreq() / (CLOCK_GetDiv(kCLOCK_UartDiv) + 1U);
            }

            return freq;
        }
    } // namespace
#endif

    /* Initialize debug console. */
    void initDebugConsole(void)
    {
#if LOG_LUART_ENABLED
        /* The user initialization should be placed here */
        lpuart_config_t lpuartConfig;

        LPUART_GetDefaultConfig(&lpuartConfig);
        lpuartConfig.baudRate_Bps = 115200;
        lpuartConfig.enableTx     = true;
        lpuartConfig.enableRx     = true;

        LPUART_TransferCreateHandle(LPUART3, &g_lpuartHandle, NULL, NULL);

        LPUART_Init(LPUART3, &lpuartConfig, BOARD_DebugConsoleSrcFreq());
        LPUART_EnableTx(LPUART3, true);
        LPUART_EnableRx(LPUART3, true);
#endif
    }
} // namespace board
