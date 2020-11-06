// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#pragma once

#include <dirent.h>

namespace vfsn::internal::dirent
{
    DIR_ITER *diropen(int &_errno_, const char *path);
    int dirreset(int &_errno_, DIR_ITER *state);
    int dirnext(int &_errno_, DIR_ITER *state);
    int dirclose(int &_errno_, DIR_ITER *state);
} // namespace vfsn::internal::dirent
