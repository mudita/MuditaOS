// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <service-appmgr/Actions.hpp>

namespace app
{
    class AlarmClockStatusParams : public app::manager::actions::ActionParams
    {
      public:
        explicit AlarmClockStatusParams(bool status) : status{status}
        {}

        [[nodiscard]] auto getAlarmClockStatus() const noexcept
        {
            return status;
        }

      private:
        bool status;
    };
} // namespace app
