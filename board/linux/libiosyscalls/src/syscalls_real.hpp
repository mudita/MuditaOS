// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

/* Helpers for intercepting library calls */
#define __REAL_DECL(fun) decltype(::fun) *fun
#define __REAL_DLSYM(fun)                                                                                              \
    do {                                                                                                               \
        real::fun = reinterpret_cast<decltype(real::fun)>(dlsym(RTLD_NEXT, #fun));                                     \
        if (!real::fun)                                                                                                \
            fprintf(stderr, "Missing libc syscall: %s()\n", #fun);                                                     \
    } while (0);

#include <dlfcn.h> // for dlsym()
