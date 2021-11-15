// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif


/** This is a extension for standard function @see exit which stop the system
 * and optionaly takes a backtrace
 * @param[in] code Standard terminate exit code
 * @param[in] bt_dump If true backtrace will be created
 * @note Function never returns
 */
void __attribute__((noreturn, used)) _exit_backtrace(int code, bool bt_dump);


/** This is internal backtrce function
 * @note In never shouldn't to be called directly in the user code
 */
static inline void __attribute__((always_inline)) _StackTrace_Dump_stage_1(void)
{
    // Redirect to the save stacktrace syscall (1)
    __asm volatile("svc #1\n");
}

/** This is internal backtrce function
 * @note In never shouldn't to be called directly in the user code
 */
extern void _StackTrace_Dump_stage_2(void);


/** This function save a backtrace on the disk and stop the system by abort
 */
static inline void __attribute__((always_inline)) _StackTrace_Dump_And_Abort(void)
{
    _StackTrace_Dump_stage_1();
    _StackTrace_Dump_stage_2();
}

#ifdef __cplusplus
}
#endif


