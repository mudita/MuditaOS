// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <bsp/common.hpp>
#include <hal/key_input/KeyEventDefinitions.hpp>
#include <queue.hpp>

#include <cstdint>
#include <vector>

namespace bsp
{
    namespace rotary_encoder
    {
        void init(xQueueHandle qHandle);
        void deinit();
        std::vector<KeyEvent> getKeyEvents();
        BaseType_t IRQHandler();
    } // namespace rotary_encoder

} // namespace bsp
