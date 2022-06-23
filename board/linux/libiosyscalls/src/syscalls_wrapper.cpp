// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "syscalls_common.hpp"

extern "C"
{
#if __GLIBC__ > 2 || ((__GLIBC__ == 2) && (__GLIBC_MINOR__ >= 33))
    // Replace ASAN stat to our stat
    int stat(const char *file, struct stat *pstat)
    {
        return _iosys_stat(file, pstat);
    }
#endif
}
