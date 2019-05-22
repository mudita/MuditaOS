/*
 *  @file bsp_keyboard.h
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 25 oct 2018
 *  @brief Keyboard BSP implementation
 *  @copyright Copyright (C) 2018 mudita.com
 *  @details
 */

#ifndef BSP_KEYBOARD_BSP_KEYBOARD_H_
#define BSP_KEYBOARD_BSP_KEYBOARD_H_

#include <stdint.h>
#include "fsl_common.h"
#include "FreeRTOS.h"


status_t bsp_keyboard_Init(bsp_keyboard_event_callback_t eventHandler);

status_t bsp_keyboard_Deinit(void);


BaseType_t bsp_keyboard_IRQHandler(void);

BaseType_t bsp_keyboard_right_functional_IRQHandler(void);

#endif /* BSP_KEYBOARD_BSP_KEYBOARD_H_ */
