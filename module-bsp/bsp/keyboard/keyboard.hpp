#ifndef PUREPHONE_KEYBOARD_HPP
#define PUREPHONE_KEYBOARD_HPP

#include <stdint.h>

#include "bsp/common.hpp"
#include "key_codes.hpp"

namespace bsp {

	void keyboard_get_data(const uint8_t& notification, uint8_t& event, uint8_t& code);

	int32_t keyboard_Init(xQueueHandle qHandle);

	int32_t keyboard_Deinit(void);

	BaseType_t keyboard_IRQHandler(void);

	BaseType_t keyboard_right_functional_IRQHandler(void);
}


#endif //PUREPHONE_KEYBOARD_HPP
