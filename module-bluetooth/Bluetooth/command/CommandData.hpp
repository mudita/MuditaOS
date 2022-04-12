// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <utility>
#include <variant>
#include <PhoneNumber.hpp>
#include "Device.hpp"
#include "EventStore.hpp"
#include "OperatorName.hpp"
#include "BatteryLevel.hpp"

namespace bluetooth
{
    using DataVariant =
        std::variant<BatteryLevel, OperatorName, Store::SignalStrength, Devicei, utils::PhoneNumber::View>;

    class CommandData
    {
      public:
        virtual auto getData() -> DataVariant = 0;
        virtual ~CommandData()                = default;
    };
} // namespace bluetooth
