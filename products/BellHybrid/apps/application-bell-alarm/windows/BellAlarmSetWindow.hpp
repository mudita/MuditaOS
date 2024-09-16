// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "presenter/BellAlarmSetPresenter.hpp"
#include <apps-common/popups/WindowWithTimer.hpp>

namespace gui
{
    class Icon;

    class BellAlarmSetWindow : public WindowWithTimer, public app::bell_alarm::BellAlarmSetContract::View
    {
      public:
        explicit BellAlarmSetWindow(app::ApplicationCommon *app,
                                    std::shared_ptr<app::bell_alarm::BellAlarmSetContract::Presenter> presenter);
        void rebuild() override;

      protected:
        void buildInterface() override;
        void buildLayout();
        bool onInput(const InputEvent &inputEvent) override;
        void buildLowBatteryLayout();
        void buildAlarmInfoLayout();
        void buildAlarmInactiveLayout();
        void lowBatteryInfoHandled();

      private:
        constexpr static auto alarmSummaryDisplayDuration = std::chrono::seconds{5};
        std::shared_ptr<app::bell_alarm::BellAlarmSetContract::Presenter> presenter;
        Icon *icon{};
    };
} // namespace gui
