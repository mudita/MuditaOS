/*
 *  @file macros.h
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 31 lip 2018
 *  @brief Handy macros
 *  @copyright Copyright (C) 2018 mudita.com
 *  @details
 */

#ifndef MACROS_H_
#define MACROS_H_

#include "MIMXRT1051.h"
#include <stdbool.h>
#include <assert.h>

#define _STRINGIFY(s) #s
#define STRINGIFY(s) _STRINGIFY(s)

#define UNUSED(x) ((void)(x))

#define ALIGN_(n) 							__attribute__((aligned(n)))

#define CACHEABLE_SECTION_SDRAM(var)		__attribute__ ((section (".sdram"))) var

#define CACHEABLE_SECTION_SDRAM_ALIGN(var,alignbytes)	\
		__attribute__ ((section (".sdram")))  __attribute__((aligned(alignbytes))) var

#define NONCACHEABLE_SECTION_SDRAM(var)		__attribute__ ((section (".sdramnoncacheable"))) var

#define NONCACHEABLE_SECTION_SDRAM_ALIGN(var,alignbytes)	\
	__attribute__ ((section (".sdramnoncacheable"))) var __attribute__((aligned(alignbytes)))

#define NONCACHEABLE_SECTION_INIT(var) 		__attribute__((section("NonCacheable.init"))) var

#define NONCACHEABLE_SECTION_ALIGN_INIT(var, alignbytes) \
    __attribute__((section("NonCacheable.init"))) var __attribute__((aligned(alignbytes)))

#define NONCACHEABLE_SECTION(var) 			__attribute__((section("NonCacheable,\"aw\",%nobits @"))) var

#define NONCACHEABLE_SECTION_ALIGN(var, alignbytes) \
    __attribute__((section("NonCacheable,\"aw\",%nobits @"))) var __attribute__((aligned(alignbytes)))

static inline uint32_t IS_MEM_ADDR_CACHED(void* addr)
{
	extern uint32_t __sdram_non_cached_start[];
	extern uint32_t __sdram_non_cached_end[];
	extern uint32_t __ocram_cached_start[];
	extern uint32_t __ocram_cached_end[];
	extern uint32_t __dtcm_ram_start[];
	extern uint32_t __dtcm_ram_end[];
	extern uint32_t __sdram_cached_start[];
	extern uint32_t __sdram_cached_end[];


	if(((uint32_t*)addr >= (uint32_t*)__sdram_non_cached_start) && ((uint32_t*)addr < (uint32_t*)__sdram_non_cached_end)){
		return 0;
	}
	if(((uint32_t*)addr >= (uint32_t*)__ocram_cached_start) && ((uint32_t*)addr < (uint32_t*)__ocram_cached_end)){
		return 1;
	}
	if(((uint32_t*)addr >= (uint32_t*)__dtcm_ram_start) && ((uint32_t*)addr < (uint32_t*)__dtcm_ram_end)){
		return 0;
	}
	if(((uint32_t*)addr >= (uint32_t*)__sdram_cached_start) && ((uint32_t*)addr < (uint32_t*)__sdram_cached_end)){
		return 1;
	}

	assert(0);
}

//! Test if in interrupt mode
static inline bool isIRQ()
{
    return (SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) != 0 ;
}


#endif /* MACROS_H_ */
