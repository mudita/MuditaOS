// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AbstractAlarmAction.hpp"

#include <Service/Service.hpp>

namespace alarms
{
    class NotifyGUIBedtimeReminderAction : public AbstractAlarmAction
    {
      public:
        explicit NotifyGUIBedtimeReminderAction(sys::Service &service);
        bool execute() override;
        bool turnOff() override;

      private:
        sys::Service &service;
    };

} // namespace alarms
