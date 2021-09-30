// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AbstractAlarmAction.hpp"

#include <Service/Service.hpp>

namespace alarms
{
    class AlarmRecordAction : public AbstractAlarmAction
    {
      public:
        explicit AlarmRecordAction(sys::Service &service);
        virtual bool execute(const AlarmEventRecord &record) override;
        bool turnOff(const AlarmEventRecord &record) override;

      private:
        sys::Service &service;
    };

} // namespace alarms
