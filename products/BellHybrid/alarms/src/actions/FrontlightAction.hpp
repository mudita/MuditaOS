// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AbstractAlarmAction.hpp"
#include <service-db/Settings.hpp>

#include <Service/Service.hpp>

namespace alarms
{
    class FrontlightAction : public AbstractAlarmAction
    {
      public:
        enum class Mode
        {
            Manual,
            LinearProgress
        };

        FrontlightAction(sys::Service &service, Mode mode);
        bool execute() override;
        bool turnOff() override;

      private:
        static constexpr auto frontlightOFF = "0";

        std::unique_ptr<AbstractAlarmAction> pimpl;
        settings::Settings settings;
    };
} // namespace alarms
