// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

enum __atexit_types
{
    __et_atexit,
    __et_onexit,
    __et_cxa
};

#define MALLOC_PROVIDED

void __call_exitprocs(int, void *);
int __register_exitproc(int, void (*fn)(void), void *, void *);
