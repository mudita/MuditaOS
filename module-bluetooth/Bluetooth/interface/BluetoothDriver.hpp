// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <functional>
#include "Device.hpp"
#include "Result.hpp"

namespace bluetooth
{
    using PowerOnCallback = std::function<void()>;

    class AbstractDriver
    {
      public:
        virtual ~AbstractDriver() noexcept = default;
        using ErrorCallback                = std::function<void(uint8_t)>;

        [[nodiscard]] virtual auto init() -> Result::Code                   = 0;
        [[nodiscard]] virtual auto run() -> Result::Code                    = 0;
        [[nodiscard]] virtual auto stop() -> Result::Code                   = 0;
        [[nodiscard]] virtual auto scan() -> Result                         = 0;
        virtual void stopScan()                                             = 0;
        virtual void setVisibility(bool visibility)                         = 0;
        virtual void pair(Devicei device, std::uint8_t protectionLevel = 0) = 0;
        virtual void unpair(Devicei device)                                 = 0;

        virtual void registerErrorCallback(const ErrorCallback &newCallback)     = 0;
        virtual void registerPowerOnCallback(const PowerOnCallback &newCallback) = 0;
    };
} // namespace bluetooth
