// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <bsp/rotary_encoder/rotary_encoder.hpp>

namespace bsp::rotary_encoder
{
    int init(xQueueHandle qHandle)
    {
        return 1;
    }
    void deinit()
    {}

    std::optional<type> WorkerEventHandler()
    {
        return std::nullopt;
    }

} // namespace bsp::rotary_encoder
