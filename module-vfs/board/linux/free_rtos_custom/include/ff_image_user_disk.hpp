// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#pragma once

#include "vfs.hpp"

namespace freertos_fat::internals
{
    FF_Disk_t *diskInit(const char *pcName, const char img_path[]);
    BaseType_t diskDelete(FF_Disk_t *pxDisk);
    BaseType_t diskShowPartition(FF_Disk_t *pxDisk);
    void diskFlush(FF_Disk_t *pxDisk);
    uint8_t diskIsPresent(void);
} // namespace freertos_fat::internals
