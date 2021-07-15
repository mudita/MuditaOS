#pragma once

#include <bsp/keyboard/key_codes.hpp>

#include <cstdint>
#include <optional>

extern "C"
{
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
}

#include "../common.hpp"

namespace bsp
{
    namespace rotary_encoder
    {
        int32_t init(xQueueHandle qHandle);
        void deinit();

        bool isPresent(void);

        bsp::Board GetBoard(void);

        std::optional<bsp::KeyCodes> WorkerEventHandler();
    } // namespace rotary-encoder

} // namespace bsp
