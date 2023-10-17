// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma GCC optimize("Og")

#include <CrashCatcher/CrashCatcher.h>

#include <log/log.hpp>
#include <date/date.h>
#include <exit_backtrace.h>
#include "crashdumpwriter_vfs.hpp"
#include "consoledump.hpp"

namespace
{
    crashdump::CrashDumpWriterVFS cwrite;
}

const CrashCatcherMemoryRegion *CrashCatcher_GetMemoryRegions()
{
    /* board/rt1051/ldscripts/memory.ld
     * NOTE: Text section and stacks sections are intentionally omitted
     * because can cause troubles in the running system
     */
    static const CrashCatcherMemoryRegion regions[] = {
        // SRAM_OC
        {0x20200000, 0x20200000 + 0x10000, CRASH_CATCHER_WORD},
        // SRAM_DTC
        {0x20000000, 0x20000000 + 0x70000, CRASH_CATCHER_WORD},
        // intentionally skip text section
        // intentionally skip the heap section
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

CrashCatcherReturnCodes CrashCatcher_DumpEnd()
{
    cwrite.saveDump();
    _exit_backtrace(-1, false);
    return CRASH_CATCHER_EXIT;
}
