// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

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

      protected:
        void buildInterface() override;
        void buildLayout();
        void registerCallbacks();
        bool onInput(const InputEvent &inputEvent) override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;

      private:
        constexpr static auto alarmSummaryDisplayDuration = std::chrono::milliseconds{5000};
        std::shared_ptr<app::bell_alarm::BellAlarmSetContract::Presenter> presenter;
        Icon *icon{};
    };
} // namespace gui
