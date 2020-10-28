// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <ff_stdio.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif
    /**
     * List all directories recursive
     * @param startPath Starting directory
     * @param callback  Callback handler (ctx, path, isDir)
     * @param context   private context
     * @return  Error code
     */
    int ff_stdio_listdir_recursive(const char *startPath, void (*callback)(void *, const char *, bool), void *context);

#ifdef __cplusplus
}
#endif
