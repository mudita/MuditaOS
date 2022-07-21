// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AbstractAlarmAction.hpp"

#include <Service/Service.hpp>

namespace alarms
{
    class NotifyGUIAction : public AbstractAlarmAction
    {
      public:
        explicit NotifyGUIAction(sys::Service &service);
        bool execute(Record record) override;
        bool turnOff() override;

      private:
        sys::Service &service;
    };

} // namespace alarms
