// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "key_codes.hpp"

#include <bsp/common.hpp>

#include <cstdint>
#include <vector>

namespace bsp::keyboard 
{
	struct KeyEvent
	{
		KeyCodes code;
		KeyEvents event;
	};

	std::vector<KeyEvent> getKeyEvents(std::uint8_t notification);

	std::int32_t init(xQueueHandle qHandle);

	std::int32_t deinit();

	BaseType_t IRQHandler();

	BaseType_t rightFunctionalIRQHandler();
} // namespace bsp::keyboard


