// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "AbstractAlarmAction.hpp"

#include <Service/Service.hpp>

namespace alarms
{
    class NotifyGUIAction : public AbstractAlarmAction
    {
      public:
        explicit NotifyGUIAction(sys::Service &service);
        bool execute() override;
        bool turnOff() override;

      private:
        sys::Service &service;
    };

} // namespace alarms
