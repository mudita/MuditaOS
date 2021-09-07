#pragma once

#include <cstdint>
#include <optional>

#include "../common.hpp"
#include <queue.hpp>

namespace bsp
{
    namespace rotary_encoder
    {
        enum class type
        {
            forward  = 0x01,
            backward = 0x02
        };
        int init(xQueueHandle qHandle);
        void deinit();
        std::optional<type> WorkerEventHandler();
    } // namespace rotary_encoder

} // namespace bsp
