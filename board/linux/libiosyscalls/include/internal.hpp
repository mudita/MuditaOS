// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <stddef.h>

struct _FF_FILE;

namespace vfsn::linux::internal
{
    int  ff_file_to_handle(_FF_FILE *fil);
    _FF_FILE * handle_to_ff_file(int fd);
    const char* relative_to_root( char *out_path, size_t out_path_len, const char *inpath );
}
