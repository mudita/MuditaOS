// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include "CommandData.hpp"

namespace bluetooth
{

    class BatteryLevelData : public CommandData
    {
      public:
        explicit BatteryLevelData(const BatteryLevel &level);
        auto getData() -> DataVariant override;

      private:
        BatteryLevel level;
    };

} // namespace bluetooth
