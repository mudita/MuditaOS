// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma GCC optimize("Og")

#include <CrashCatcher/CrashCatcher.h>

#include <log.hpp>
#include <date/date.h>
#include "crashdumpwriter.hpp"
#include "consoledump.hpp"

const CrashCatcherMemoryRegion *CrashCatcher_GetMemoryRegions(void)
{
    // board/rt1051/ldscripts/memory.ld
    static const CrashCatcherMemoryRegion regions[] = {
        // SRAM_OC
        {0x20200000, 0x20210000, CRASH_CATCHER_BYTE},
        // SRAM_DTC
        {0x20000000, 0x20070000, CRASH_CATCHER_BYTE},
    // intentionally skip text section
    // BOARD_SDRAM_HEAP
#if defined(HW_SDRAM_64_MB) && (HW_SDRAM_64_MB == 1)
        {0x80620000, 0x84000000, CRASH_CATCHER_BYTE},
#else
        {0x80620000, 0x81000000, CRASH_CATCHER_BYTE},
#endif
        // end tag
        {0xFFFFFFFF, 0xFFFFFFFF, CRASH_CATCHER_BYTE},
    };

    return regions;
}

void CrashCatcher_DumpStart(const CrashCatcherInfo *pInfo)
{
    crashdump::printCrashInfo(pInfo);
    crashdump::CrashDumpWriter::instance().openDump();
}

void CrashCatcher_DumpMemory(const void *pvMemory, CrashCatcherElementSizes elementSize, size_t elementCount)
{
    switch (elementSize) {
    case CRASH_CATCHER_BYTE:
        crashdump::CrashDumpWriter::instance().writeBytes(static_cast<const std::uint8_t *>(pvMemory), elementCount);
        break;
    case CRASH_CATCHER_HALFWORD:
        crashdump::CrashDumpWriter::instance().writeHalfWords(static_cast<const std::uint16_t *>(pvMemory),
                                                              elementCount);
        break;
    case CRASH_CATCHER_WORD:
        crashdump::CrashDumpWriter::instance().writeWords(static_cast<const std::uint32_t *>(pvMemory), elementCount);
        break;
    }
}

CrashCatcherReturnCodes CrashCatcher_DumpEnd(void)
{
    crashdump::CrashDumpWriter::instance().saveDump();
    abort();
    return CRASH_CATCHER_EXIT;
}
