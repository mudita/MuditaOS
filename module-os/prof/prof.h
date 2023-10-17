// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

/// just store what's interesting
/// 1. do not store TaskHandle_t, id is better - we normally have up to 32 id's as we do not
///    as we tend to keep up to ~30 threads alive
/// 2. execution time, the better granulation, the better result
struct task_prof_data {
    uint32_t task_TCB_id; /// task TCB id
    uint32_t exec_time;   /// single task switch execution time summed up in TS
    uint32_t switches;    /// count how many times it was switched out
};

/// initialization structure
struct prof_pool_init_data
{
    size_t size;                /// size of the pool, use should have linear eficiency
};

#if PROF_ON

/// initialization of pool to store switch data
void prof_pool_init(struct prof_pool_init_data init);
void prof_pool_deinit();
struct prof_pool_init_data prof_pool_get_data();

/// get next available slot from the pool
/// struct task_prof_data* prof_pool_get_next();
/// set the element
void prof_pool_data_set(uint8_t ts, uint32_t id);

/// mark if overflow happened
unsigned int prof_pool_overflow();

/// to `mem` flush up to `cap` data - then clean
/// if passed:
/// - set used count: how much data was used
/// - returns overflow count
/// requires sched lock before running
unsigned int prof_pool_flush(struct task_prof_data *mem, size_t cap);

#else
#define prof_pool_data_set(...)
#endif

#ifdef __cplusplus
}
#endif
