// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <log/log.hpp>
#include <sys/stat.h>
#include <cerrno>
#include <cstdio>

namespace
{
    inline bool statFd(FILE *fd, char const *logMessage)
    {
        struct stat fdStats;
        constexpr int statFailed = -1;
        if (fstat(fileno(fd), &fdStats) != statFailed) {
            return true;
        }

        auto originalErrno = errno;
        LOG_WARN("%s", logMessage);
        errno = originalErrno;
        return false;
    }
} // namespace
