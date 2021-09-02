// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <CrashCatcher/CrashCatcher.h>

namespace crashdump
{
    void printCrashInfo(const CrashCatcherInfo *info);

} // namespace crashdump
