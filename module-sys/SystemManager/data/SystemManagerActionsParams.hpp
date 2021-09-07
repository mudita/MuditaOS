// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-appmgr/Actions.hpp>

namespace app::manager::actions
{
    class LowBatteryNotificationParams : public ActionParams
    {
      public:
        explicit LowBatteryNotificationParams(bool isActive, bool isCharging)
            : active(isActive), charging(isCharging){};

        [[nodiscard]] bool isActive() const noexcept
        {
            return active;
        }

        [[nodiscard]] bool isCharging() const noexcept
        {
            return charging;
        }

      private:
        bool active;
        bool charging;
    };

} // namespace app::manager::actions
