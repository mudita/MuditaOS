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
    class PowerNapSessionPausedContract
    {
      public:
        class View
        {
          public:
            virtual ~View() = default;
        };
        class Presenter : public BasePresenter<PowerNapSessionPausedContract::View>
        {
          public:
            virtual bool handleIfPreWakeupIsToTurnOffFirst() = 0;
        };
    };

    class PowerNapSessionPausedPresenter : public PowerNapSessionPausedContract::Presenter
    {
        app::ApplicationCommon *app{};
        AbstractAlarmModel &alarmModel;
        bool handleIfPreWakeupIsToTurnOffFirst() override;

      public:
        explicit PowerNapSessionPausedPresenter(app::ApplicationCommon *app, AbstractAlarmModel &alarm);
    };
} // namespace app::powernap
