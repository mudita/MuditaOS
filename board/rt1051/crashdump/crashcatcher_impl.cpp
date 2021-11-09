// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma GCC optimize("Og")

#include <CrashCatcher/CrashCatcher.h>

#include <log/log.hpp>
#include <date/date.h>
#include "crashdumpwriter_vfs.hpp"
#include "consoledump.hpp"

namespace
{
    crashdump::CrashDumpWriterVFS cwrite;
}

const CrashCatcherMemoryRegion *CrashCatcher_GetMemoryRegions(void)
{
    /* board/rt1051/ldscripts/memory.ld
     * NOTE: Text section and stacks sections are intentionally ommited
     * because can cause throubles in the running system
     */
    static const CrashCatcherMemoryRegion regions[] = {
        // SRAM_OC
        {0x20200000, 0x20210000, CRASH_CATCHER_BYTE},
        // SRAM_DTC
        {0x20000000, 0x20070000, CRASH_CATCHER_BYTE},
        // end tag
        {0xFFFFFFFF, 0xFFFFFFFF, CRASH_CATCHER_BYTE},
    };

    return regions;
}

void CrashCatcher_DumpStart(const CrashCatcherInfo *pInfo)
{
    crashdump::printCrashInfo(pInfo);
    cwrite.openDump();
}

void CrashCatcher_DumpMemory(const void *pvMemory, CrashCatcherElementSizes elementSize, size_t elementCount)
{
    switch (elementSize) {
    case CRASH_CATCHER_BYTE:
        cwrite.writeBytes(static_cast<const std::uint8_t *>(pvMemory), elementCount);
        break;
    case CRASH_CATCHER_HALFWORD:
        cwrite.writeHalfWords(static_cast<const std::uint16_t *>(pvMemory), elementCount);
        break;
    case CRASH_CATCHER_WORD:
        cwrite.writeWords(static_cast<const std::uint32_t *>(pvMemory), elementCount);
        break;
    }
}

CrashCatcherReturnCodes CrashCatcher_DumpEnd(void)
{
    cwrite.saveDump();
    abort();
    return CRASH_CATCHER_EXIT;
}
