#pragma once

#define LWIP_DECLARE_MEMORY_ALIGNED(variable_name, size) u8_t variable_name[LWIP_MEM_ALIGN_BUFFER(size)] \
__attribute__((section(".sdram"))) __attribute__((aligned(4)));
