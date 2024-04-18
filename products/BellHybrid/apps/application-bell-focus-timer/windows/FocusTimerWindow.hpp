// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "data/FocusCommon.hpp"
#include "presenter/FocusTimerPresenter.hpp"

#include <Text.hpp>
#include <apps-common/widgets/BarGraph.hpp>
#include <apps-common/widgets/TimeMinuteSecondWidget.hpp>
#include <common/widgets/BellStatusClock.hpp>
#include <gui/widgets/Icon.hpp>
#include <apps-common/windows/AppWindow.hpp>

namespace app::focus
{
    class FocusTimerWindow : public gui::AppWindow, public FocusTimerContract::View
    {
      public:
        FocusTimerWindow(app::ApplicationCommon *app,
                         std::unique_ptr<FocusTimerContract::Presenter> &&windowPresenter,
                         const std::string &name = window::name::timer);

        void onBeforeShow(gui::ShowMode mode, gui::SwitchData *data) override;
        void buildInterface() override;
        bool onInput(const gui::InputEvent &inputEvent) override;
        void showFocusSessionCountdown() override;
        void showShortBreakCountdown() override;
        void showTimeForFocusInfo() override;
        void showTimeForBreakInfo() override;
        void showEndOfAllSessionsInfo() override;
        void pause() override;
        void resume() override;

      private:
        std::unique_ptr<FocusTimerContract::Presenter> presenter;
        gui::VBox *mainVBox{nullptr};
        gui::ArcProgressBar *progress{nullptr};
        gui::TimeMinuteSecondWidget *timer{nullptr};
        gui::TextFixedSize *bottomDescription{nullptr};
        gui::Icon *iconPause{nullptr};
        gui::Icon *iconRing{nullptr};
        gui::BellStatusClock *clock{nullptr};

        void setTime(std::time_t newTime) override;
        void setTimeFormat(utils::time::Locale::TimeFormat fmt) override;
        gui::RefreshModes updateTime() override;

        void buildLayout();
        void configureTimer();
    };
} // namespace app::focus
