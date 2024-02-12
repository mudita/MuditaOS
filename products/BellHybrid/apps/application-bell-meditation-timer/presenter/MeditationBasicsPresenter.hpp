// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/BasePresenter.hpp>
#include <common/models/AbstractAlarmModel.hpp>

namespace app
{
    class ApplicationCommon;
}

namespace app::meditation
{
    class MeditationBasicsContract
    {
      public:
        class View
        {
          public:
            virtual ~View() = default;
        };
        class Presenter : public BasePresenter<MeditationBasicsContract::View>
        {
          public:
            virtual bool handleIfPreWakeupIsToTurnOffFirst() = 0;
        };
    };

    class MeditationBasicsPresenter : public MeditationBasicsContract::Presenter
    {
        app::ApplicationCommon *app{};
        AbstractAlarmModel &alarmModel;
        bool handleIfPreWakeupIsToTurnOffFirst() override;

      public:
        explicit MeditationBasicsPresenter(app::ApplicationCommon *app, AbstractAlarmModel &alarm);
    };
} // namespace app::meditation
