// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "service-bluetooth/BluetoothMessage.hpp"

namespace message::bluetooth
{
    class InitializationResult : public BluetoothMessage
    {
      public:
        explicit InitializationResult(bool succeed) : succeed(succeed)
        {}
        [[nodiscard]] auto isSucceed() const noexcept -> bool
        {
            return succeed;
        }

      private:
        const bool succeed;
    };
} // namespace message::bluetooth
