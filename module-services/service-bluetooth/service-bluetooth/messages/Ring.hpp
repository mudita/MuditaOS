// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "service-bluetooth/BluetoothMessage.hpp"

namespace message::bluetooth
{
    /// @brief Message that indicates whether to enable or disable bluetooth ring
    class Ring : public BluetoothMessage
    {
      public:
        enum class State : bool
        {
            Enable,
            Disable
        };

        explicit Ring(State state) : state{state}
        {}

        /// @return True if bluetooth should ring, false otherwise
        [[nodiscard]] auto enabled() const noexcept -> bool
        {
            return state == State::Enable;
        }

      private:
        const State state;
    };
} // namespace message::bluetooth
