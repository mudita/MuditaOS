// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include "CommandData.hpp"

namespace bluetooth
{

    class SignalStrengthData : public CommandData
    {
      public:
        explicit SignalStrengthData(const Store::SignalStrength &signalStrength);
        auto getData() -> DataVariant override;

      private:
        Store::SignalStrength signalStrength;
    };

} // namespace bluetooth
