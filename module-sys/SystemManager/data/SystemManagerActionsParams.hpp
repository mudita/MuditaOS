// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-appmgr/service-appmgr/Actions.hpp>

namespace app::manager::actions
{
    class LowBatteryNotificationParams : public ActionParams
    {
      public:
        explicit LowBatteryNotificationParams(bool isActive) : isActive(isActive){};

        [[nodiscard]] bool getActiveState() const noexcept
        {
            return isActive;
        }

      private:
        bool isActive;
    };

} // namespace app::manager::actions
