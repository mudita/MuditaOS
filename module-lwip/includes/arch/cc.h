#pragma once

#define LWIP_DECLARE_MEMORY_ALIGNED(variable_name, size) u8_t variable_name[LWIP_MEM_ALIGN_BUFFER(size)] \
__attribute__((section(".sdram"))) __attribute__((aligned(4)));

#define LWIP_PLATFORM_ASSERT(x) do {printf("Assertion \"%s\" failed at line %d in %s\n", \
                                     x, __LINE__, __FILE__); abort();} while(0)


#define TCPIP_MBOX_SIZE                 1028
#define TCPIP_THREAD_STACKSIZE          4096
