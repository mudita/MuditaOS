// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/BasePresenter.hpp>
#include <common/models/AbstractAlarmModel.hpp>

namespace app
{
    class ApplicationCommon;
}

namespace app::powernap
{
    class PowerNapSessionEndedContract
    {
      public:
        class View
        {
          public:
            virtual ~View() = default;
        };
        class Presenter : public BasePresenter<PowerNapSessionEndedContract::View>
        {
          public:
            virtual void activate()                          = 0;
            virtual bool handleIfPreWakeupIsToTurnOffFirst() = 0;
        };
    };

    class PowerNapSessionEndPresenter : public PowerNapSessionEndedContract::Presenter
    {
        app::ApplicationCommon *app{};
        AbstractAlarmModel &alarmModel;
        void activate() override;
        bool handleIfPreWakeupIsToTurnOffFirst() override;

      public:
        explicit PowerNapSessionEndPresenter(app::ApplicationCommon *app, AbstractAlarmModel &alarm);
    };
} // namespace app::powernap
