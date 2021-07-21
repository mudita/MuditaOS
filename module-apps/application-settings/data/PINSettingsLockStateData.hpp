// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <gui/SwitchData.hpp>

namespace gui
{
    class PINSettingsLockStateData : public SwitchData
    {
      public:
        explicit PINSettingsLockStateData(bool state) : state(state)
        {}
        [[nodiscard]] bool getSimCardPinLockState() const noexcept
        {
            return state;
        }

      private:
        const bool state;
    };
} // namespace gui
