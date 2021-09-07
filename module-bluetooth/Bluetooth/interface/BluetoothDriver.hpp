// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <functional>
#include "Error.hpp"

namespace bluetooth
{
    class AbstractDriver
    {
      public:
        using ErrorCallback                = std::function<void(uint8_t)>;
        virtual ~AbstractDriver() noexcept = default;

        [[nodiscard]] virtual auto init() -> Error::Code                     = 0;
        [[nodiscard]] virtual auto run() -> Error::Code                      = 0;
        [[nodiscard]] virtual auto stop() -> Error::Code                     = 0;
        [[nodiscard]] virtual auto scan() -> Error                                               = 0;
        virtual void stopScan()                                                                  = 0;
        virtual void setVisibility(bool visibility)                                              = 0;
        [[nodiscard]] virtual auto pair(uint8_t *addr, std::uint8_t protectionLevel = 0) -> bool = 0;
        [[nodiscard]] virtual auto unpair(uint8_t *addr) -> bool                                 = 0;

        virtual void registerErrorCallback(const ErrorCallback &newCallback) = 0;
    };
} // namespace bluetooth
