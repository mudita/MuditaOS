// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "EnterSleepMode.h"
#include <macros.h>
#include <fsl_semc.h>

extern char __sdram_start; // Defined in linker script

/* This function needs to be compiled either as a C code, or as a C++ code with
 * exception handling disabled (-fno-exceptions), otherwise it is not possible to relocate
 * its code into OCRAM. Rationale:
 * https://stackoverflow.com/questions/52637962/arm-cortex-m7-long-branch-get-error-relocation-truncated-to-fit-r-arm-prel31
 *
 * As compiling single C++ file with exception handling disabled seems to be impossible
 * using CMake and selectively disabling exceptions using #pragmas looks ugly, I've decided
 * to just put it in a separate .c file. */
WFI_CODE_SECTION(void enterSleepMode(void))
{
    /* Wait until SDRAM operations done and switch to self-refresh mode */
    __DSB();
    while ((SEMC->STS0 & SEMC_STS0_IDLE_MASK) == 0) {} // Wait until SEMC idle
    __ISB();
    SEMC_SendIPCommand(SEMC, kSEMC_MemType_SDRAM, (uint32_t)&__sdram_start, kSEMC_SDRAMCM_SelfRefresh, 0, NULL);

    /* Go to sleep */
    __WFI();
    __ISB();
}
