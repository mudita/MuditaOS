// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint>
#include "fsl_common.h"
#include "FreeRTOS.h"

namespace bsp::eink
{
    using BusyEventCallback = void (*)();

    inline constexpr auto busyTimeoutMs = 3000U;

    enum class SPICSConfig
    {
        Automatic,
        Manual
    };

    enum class SPICSState
    {
        Clear = 0,
        Set   = 1
    };

    status_t init();
    void deinit();

    void logicPowerOn();
    void logicPowerOff();

    void writeCS(SPICSState csState);
    bool waitUntilDisplayBusy(std::uint32_t timeout);
    void resetDisplayController();

    status_t changeSpiFrequency(std::uint32_t frequencyHz);
    status_t writeData(void *txBuffer, std::uint32_t len, SPICSConfig csMode);
    status_t readData(void *rxBuffer, std::uint32_t len, SPICSConfig csMode);

    BaseType_t busyPinStateChangeHandler();
} // namespace bsp::eink
