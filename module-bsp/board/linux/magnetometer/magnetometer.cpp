// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "bsp/magnetometer/magnetometer.hpp"

namespace
{
    constexpr auto offlineSliderPosition = bsp::KeyCodes::SSwitchDown;
    xQueueHandle qHandleIrq              = nullptr;

    void requestReadout()
    {
        if (qHandleIrq == nullptr) {
            return;
        }

        std::uint8_t val = 0x01;
        xQueueSend(qHandleIrq, &val, 0);
    }
} // namespace

namespace bsp::magnetometer
{
    std::int32_t init(xQueueHandle qHandle)
    {
        qHandleIrq = qHandle;
        requestReadout();
        return 0;
    }

    void deinit()
    {}

    bool isPresent()
    {
        return false;
    }

    std::optional<bsp::KeyCodes> WorkerEventHandler()
    {
        return offlineSliderPosition;
    }

    void resetCurrentParsedValue()
    {}
} // namespace bsp::magnetometer
