// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "MeditationProgressPresenter.hpp"

#include <Application.hpp>
#include <AppWindow.hpp>

namespace gui
{
    class ArcProgressBar;
    class BellStatusClock;
    class TimeMinuteSecondWidget;

    class MeditationRunningWindow : public AppWindow, public app::meditation::MeditationProgressContract::View
    {
      public:
        MeditationRunningWindow(
            app::ApplicationCommon *app,
            std::unique_ptr<app::meditation::MeditationProgressContract::Presenter> &&windowPresenter);

        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        bool onInput(const InputEvent &inputEvent) override;
        void buildInterface() override;
        void progressFinished() override;
        void intervalReached() override;
        void pause() override;
        void resume() override;

      private:
        std::unique_ptr<app::meditation::MeditationProgressContract::Presenter> presenter;

        VBox *mainVBox{nullptr};
        BellStatusClock *clock{nullptr};
        ArcProgressBar *progress{nullptr};
        VBox *pauseBox{nullptr};
        TimeMinuteSecondWidget *timer{nullptr};
        TextFixedSize *bottomDescription{nullptr};

        void setTime(std::time_t newTime) override;
        void setTimeFormat(utils::time::Locale::TimeFormat fmt) override;
        RefreshModes updateTime() override;

        void buildLayout();
        void configureTimer();

        void intervalTimeout();
        void endSession();
    };
} // namespace gui
