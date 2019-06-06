/*
 * linux_keyboard.hpp
 *
 *  Created on: May 27, 2019
 *      Author: kuba
 */

#ifndef MODULE_BSP_BOARD_LINUX_KEYBOARD_BSP_KEYBOARD_HPP_
#define MODULE_BSP_BOARD_LINUX_KEYBOARD_BSP_KEYBOARD_HPP_


#include <functional>
#include <stdint.h>

#include "FreeRTOS.h"



#include "common.hpp"
#include "bsp/keyboard/key_codes.hpp"
#include "service-kbd/WorkerEvent.hpp"


namespace bsp{



	RetCode linux_keyboard_Init(WorkerEvent* worker);

	int32_t linux_keyboard_Deinit(void);

	BaseType_t linux_keyboard_IRQHandler(void);

	BaseType_t linux_keyboard_right_functional_IRQHandler(void);

	void startKeyTimer(uint32_t time, xQueueHandle qhandle);

}


#endif /* MODULE_BSP_BOARD_LINUX_KEYBOARD_BSP_KEYBOARD_HPP_ */
