// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#include "vfs_internal_dirent.hpp"
#include <cstring>
#include <errno.h>
#include <new>
#include <dirent.h>

namespace vfsn::internal::dirent
{
    DIR_ITER *diropen(int &_errno_, const char *path)
    {
        return nullptr;
    }

    int dirreset(int &_errno_, DIR_ITER *state)
    {
        return -1;
    }
    int dirnext(int &_errno_, DIR_ITER *state)
    {
        return -1;
    }

    int dirclose(int &_errno_, DIR_ITER *state)
    {
        return -1;
    }

} // namespace vfsn::internal::dirent
