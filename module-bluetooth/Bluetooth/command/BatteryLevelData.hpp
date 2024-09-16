// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once
#include "CommandData.hpp"

namespace bluetooth
{

    class BatteryLevelData : public Action
    {
      public:
        explicit BatteryLevelData(const BatteryLevel &level);
        auto getData() -> DataVariant override;

      private:
        BatteryLevel level;
    };

} // namespace bluetooth
