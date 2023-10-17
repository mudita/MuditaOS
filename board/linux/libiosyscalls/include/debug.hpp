// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#ifdef DEBUG_SHARED_LIBRARY_FS_LIB
namespace vfsn::linux::internal
{
    void debug_trace_syscall(const char *fn, const char *format, ...);
}
#define TRACE_SYSCALL()             vfsn::linux::internal::debug_trace_syscall(__PRETTY_FUNCTION__, "")
#define TRACE_SYSCALLN(format, ...) vfsn::linux::internal::debug_trace_syscall(__PRETTY_FUNCTION__, format, __VA_ARGS__)
#else
#define TRACE_SYSCALL()                                                                                                \
    do {                                                                                                               \
    } while (0)
#define TRACE_SYSCALLN(format, ...)                                                                                    \
    do {                                                                                                               \
    } while (0)
#endif
