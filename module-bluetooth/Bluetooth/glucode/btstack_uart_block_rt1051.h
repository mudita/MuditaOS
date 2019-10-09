#pragma once

#ifdef __cplusplus
extern "C"{
#endif
#include <btstack_uart_block.h>

const btstack_uart_block_t *btstack_uart_block_rt1051_instance(xQueueHandle qHandle);

#ifdef __cplusplus
}; // __cplusplus
#endif
