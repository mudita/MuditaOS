// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <log/log.hpp>

#include "usermem.h"

struct UserMemStatsLogger
{
#if DEBUG_HEAP_ALLOCATIONS == 1
    UserMemStatsLogger()
    {
        usermemResetStatistics();
        freeHeapSize1 = usermemGetFreeHeapSize();
    }

    ~UserMemStatsLogger()
    {
        size_t freeHeapSize2      = usermemGetFreeHeapSize();
        size_t allocationsCount   = usermemGetAllocationsCount();
        size_t deallocationsCount = usermemGetDeallocationsCount();
        size_t allocatedMin       = usermemGetAllocatedMin();
        size_t allocatedMax       = usermemGetAllocatedMax();
        size_t allocatedSum       = usermemGetAllocatedSum();
        LOG_INFO("\nFree before: %zu\nFree after: %zu\n# allocations: %zu\n# deallocations: %zu\nSmallest block: %zu\nBiggest block: %zu\nAllocated: %zu",
                 freeHeapSize1, freeHeapSize2, allocationsCount, deallocationsCount, allocatedMin, allocatedMax, allocatedSum);
    }

private:
    size_t freeHeapSize1;
#endif
};
