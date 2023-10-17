// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Application.hpp>
#include <AppWindow.hpp>
#include <InputEvent.hpp>
#include <Text.hpp>
#include <apps-common/widgets/BarGraph.hpp>
#include <apps-common/widgets/TimeFixedWidget.hpp>
#include <common/widgets/BellStatusClock.hpp>
#include <gui/widgets/Icon.hpp>

#include "MeditationProgressPresenter.hpp"

namespace gui
{
    class MeditationRunningWindow : public AppWindow, public app::meditation::MeditationProgressContract::View
    {
      public:
        MeditationRunningWindow(
            app::ApplicationCommon *app,
            std::unique_ptr<app::meditation::MeditationProgressContract::Presenter> &&windowPresenter);

        // virtual methods
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        bool onInput(const InputEvent &inputEvent) override;
        void buildInterface() override;
        void progressFinished() override;
        void intervalReached() override;
        void pause() override;
        void resume() override;

      private:
        std::unique_ptr<app::meditation::MeditationProgressContract::Presenter> presenter;
        gui::VBox *mainVBox           = nullptr;
        gui::ArcProgressBar *progress = nullptr;
        gui::TimeFixedWidget *timer   = nullptr;
        gui::Icon *icon               = nullptr;
        gui::BellStatusClock *clock   = nullptr;

        void setTime(std::time_t newTime) override;
        void setTimeFormat(utils::time::Locale::TimeFormat fmt) override;
        RefreshModes updateTime() override;

        void buildLayout();
        void configureTimer();

        void endSession();
        void intervalTimeout();
        void playGong();
    };
} // namespace gui
