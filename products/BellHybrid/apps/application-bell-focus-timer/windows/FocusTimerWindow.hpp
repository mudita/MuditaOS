// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "data/FocusCommon.hpp"
#include "presenter/FocusTimerPresenter.hpp"

#include <apps-common/windows/AppWindow.hpp>

namespace gui
{
    class ArcProgressBar;
    class TimeMinuteSecondWidget;
    class Icon;
    class BellStatusClock;
} // namespace gui

namespace app::focus
{
    using namespace gui;

    class FocusTimerWindow : public AppWindow, public FocusTimerContract::View
    {
      public:
        FocusTimerWindow(app::ApplicationCommon *app,
                         std::unique_ptr<FocusTimerContract::Presenter> &&windowPresenter,
                         const std::string &name = window::name::timer);

        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void buildInterface() override;
        bool onInput(const InputEvent &inputEvent) override;
        void showFocusSessionCountdown() override;
        void showShortBreakCountdown() override;
        void showLongBreakCountdown() override;
        void showTimeForFocusInfo() override;
        void showTimeForBreakInfo() override;
        void showEndOfAllSessionsInfo() override;
        void pause() override;
        void resume() override;

      private:
        std::unique_ptr<FocusTimerContract::Presenter> presenter;

        VBox *mainVBox{nullptr};
        BellStatusClock *clock{nullptr};
        ArcProgressBar *progress{nullptr};
        VBox *pauseBox{nullptr};
        VBox *ringBox{nullptr};
        TimeMinuteSecondWidget *timer{nullptr};
        TextFixedSize *bottomDescription{nullptr};

        void setTime(std::time_t newTime) override;
        void setTimeFormat(utils::time::Locale::TimeFormat fmt) override;
        RefreshModes updateTime() override;

        void buildLayout();
        void configureTimer();
    };
} // namespace app::focus
