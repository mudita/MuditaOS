// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#pragma once

#include <dirent.h>


namespace vfsn::linux::internal::diren
{
    struct DIR_ITER {
        void *dir_state;
        char name_state[NAME_MAX+1];
    };

    DIR_ITER *diropen(int &_errno_, const char *path);
    int dirreset(int &_errno_, DIR_ITER *state);
    int dirnext(int &_errno_, DIR_ITER *state);
    int dirclose(int &_errno_, DIR_ITER *state);
} // namespace vfsn::internal::dirent
