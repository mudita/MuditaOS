// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <sys/stat.h>
#include <cstdio>

namespace audio
{
    inline bool fileExists(FILE *fd)
    {
        struct stat fdStat
        {};
        constexpr auto statFailed = -1;
        return (fstat(fileno(fd), &fdStat) != statFailed);
    }
} // namespace audio
