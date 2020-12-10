// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "utility.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int path_is_a_directory(const char *path)
{
    struct stat statbuf;
    int err = stat(path, &statbuf);
    if (err) {
        return err;
    }
    return statbuf.st_mode & S_IFDIR;
}
