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


#include "module-sys/Service/Service.hpp"
#include "common.hpp"
#include "module-bsp/bsp/keyboard/key_codes.hpp"
#include "service-kbd/WorkerKbd.hpp"


namespace bsp{



RetCode linux_keyboard_Init(WorkerKbd* worker);

		int32_t linux_keyboard_Deinit(void);

        BaseType_t linux_keyboard_IRQHandler(void);

        BaseType_t linux_keyboard_right_functional_IRQHandler(void);

        void linux_keyboard_event_callback(KeyEvents event,KeyCodes code, sys::Service* S);
        void startKeyTimer(uint32_t time, xQueueHandle qhandle);

}


#endif /* MODULE_BSP_BOARD_LINUX_KEYBOARD_BSP_KEYBOARD_HPP_ */
