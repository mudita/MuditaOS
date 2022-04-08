// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#ifndef MACROS_H_
#define MACROS_H_

#include <stdbool.h>
#include <assert.h>
#include <stdint.h>

#define _STRINGIFY(s) #s
#define STRINGIFY(s)  _STRINGIFY(s)

#define UNUSED(x) ((void)(x))

#define ALIGN_(n) __attribute__((aligned(n)))

#define CACHEABLE_SECTION_SDRAM(var) var

#define CACHEABLE_SECTION_SDRAM_ALIGN(var, alignbytes) var

#define NONCACHEABLE_SECTION_SDRAM(var) var

#define NONCACHEABLE_SECTION_SDRAM_ALIGN(var, alignbytes) var

#define NONCACHEABLE_SECTION_INIT(var) var

#define NONCACHEABLE_SECTION_ALIGN_INIT(var, alignbytes) var

#define NONCACHEABLE_SECTION(var) var

#define NONCACHEABLE_SECTION_ALIGN(var, alignbytes) var

static inline uint32_t IS_MEM_ADDR_CACHED(void *addr)
{
    extern uint32_t __sdram_non_cached_start[];
    extern uint32_t __sdram_non_cached_end[];
    extern uint32_t __ocram_cached_start[];
    extern uint32_t __ocram_cached_end[];
    extern uint32_t __dtcm_ram_start[];
    extern uint32_t __dtcm_ram_end[];
    extern uint32_t __sdram_cached_start[];
    extern uint32_t __sdram_cached_end[];

    if (((uint32_t *)addr >= (uint32_t *)__sdram_non_cached_start) &&
        ((uint32_t *)addr < (uint32_t *)__sdram_non_cached_end)) {
        return 0;
    }
    if (((uint32_t *)addr >= (uint32_t *)__ocram_cached_start) && ((uint32_t *)addr < (uint32_t *)__ocram_cached_end)) {
        return 1;
    }
    if (((uint32_t *)addr >= (uint32_t *)__dtcm_ram_start) && ((uint32_t *)addr < (uint32_t *)__dtcm_ram_end)) {
        return 0;
    }
    if (((uint32_t *)addr >= (uint32_t *)__sdram_cached_start) && ((uint32_t *)addr < (uint32_t *)__sdram_cached_end)) {
        return 1;
    }

    assert(0);
}

//! Test if in interrupt mode
static inline bool isIRQ()
{
    return false;
}

#endif /* MACROS_H_ */
