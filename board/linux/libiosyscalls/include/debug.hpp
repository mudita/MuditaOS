// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <stdio.h>
#include <dlfcn.h>

namespace vfsn::debug::internal {
    static inline void trace_syscall( const char* fn)
    {
        int (*real_fprintf)(FILE *__restrict __stream, const char *__restrict __format, ...);
        real_fprintf = reinterpret_cast<decltype(real_fprintf)>(dlsym(RTLD_NEXT, "fprintf"));
        real_fprintf(stderr,">>>>>>> CALL FUNC [%s] <<<<<<<\n", fn );
    }
}
#ifdef DEBUG_SHARED_LIBRARY_FS_LIB
#define TRACE_SYSCALL() vfsn::debug::internal::trace_syscall(__PRETTY_FUNCTION__)
#else
#define TRACE_SYSCALL() do {} while(0)
#endif
