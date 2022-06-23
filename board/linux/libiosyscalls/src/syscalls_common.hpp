// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

extern "C"
{
#if __GLIBC__ > 2 || ((__GLIBC__ == 2) && (__GLIBC_MINOR__ >= 33))
    int _iosys_stat(const char *file, struct stat *pstat);
#endif
}
