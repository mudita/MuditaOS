// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <stddef.h>

struct _FF_FILE;
struct _IO_FILE;

namespace vfsn::linux::internal
{
    int  ff_file_to_fd(_FF_FILE *fil);
    _FF_FILE * fd_to_ff_file(int fd);
    const char* relative_to_root( char *out_path, size_t out_path_len, const char *inpath );
    bool is_ff_handle(_IO_FILE* descriptor);
    bool vfs_is_initialized();
}
