
/*
 * @file keyboard.hpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 22.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#ifndef PUREPHONE_KEYBOARD_HPP
#define PUREPHONE_KEYBOARD_HPP

#include <stdint.h>

#include "common.hpp"
#include "service-kbd/WorkerEvent.hpp"
#include "key_codes.hpp"

namespace bsp {

	void keyboard_get_data(const uint8_t& notification, uint8_t& event, uint8_t& code);

	int32_t keyboard_Init(xQueueHandle qHandle);

	int32_t keyboard_Deinit(void);

	BaseType_t keyboard_IRQHandler(void);

	BaseType_t keyboard_right_functional_IRQHandler(void);
}


#endif //PUREPHONE_KEYBOARD_HPP
