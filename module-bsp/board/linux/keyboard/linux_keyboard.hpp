/*
 * linux_keyboard.hpp
 *
 *  Created on: May 27, 2019
 *      Author: kuba
 */

#ifndef MODULE_BSP_BOARD_LINUX_KEYBOARD_LINUX_KEYBOARD_HPP_
#define MODULE_BSP_BOARD_LINUX_KEYBOARD_LINUX_KEYBOARD_HPP_


#include <functional>
#include <stdint.h>

#include "FreeRTOS.h"


#include "module-sys/Service/Service.hpp"
#include "key_codes_linux.hpp"
#include "common.hpp"



namespace bsp{

RetCode linux_keyboard_Init(std::function<void(KeyEvents event,KeyCodes code, sys::Service* s)> eventHandler , sys::Service* s);

		int32_t linux_keyboard_Deinit(void);

        BaseType_t linux_keyboard_IRQHandler(void);

        BaseType_t linux_keyboard_right_functional_IRQHandler(void);

        void linux_keyboard_event_callback(KeyEvents event,KeyCodes code, sys::Service* s);

}


#endif /* MODULE_BSP_BOARD_LINUX_KEYBOARD_LINUX_KEYBOARD_HPP_ */
