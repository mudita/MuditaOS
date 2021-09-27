// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Application.hpp>
#include <InputEvent.hpp>
#include <Label.hpp>
#include <Text.hpp>

#include "UnityProgressBar.hpp"
#include "WithTimerWindow.hpp"
#include "MeditationProgressPresenter.hpp"

namespace gui
{
    namespace name::window
    {
        inline constexpr auto meditation_running = "Meditation progress";
    } // namespace name::window

    class MeditationRunningWindow : public MeditationWindow, public app::meditation::MeditationProgressContract::View
    {
      public:
        explicit MeditationRunningWindow(
            app::ApplicationCommon *app,
            std::unique_ptr<app::meditation::MeditationProgressContract::Presenter> &&windowPresenter);

        // virtual methods
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        bool onInput(const InputEvent &inputEvent) override;
        void buildInterface() override;
        void pregressFinished() override;
        void intervalReached() override;
        void baseTickReached() override;
        void buildMeditationItem(MeditationItem &item) override;
        void onMeditationItemAvailable(MeditationItem *item) override;

      private:
        std::unique_ptr<app::meditation::MeditationProgressContract::Presenter> presenter;
        gui::Label *datetime            = nullptr;
        gui::Text *timer                = nullptr;
        gui::UnityProgressBar *progress = nullptr;

        void configureTimer();
        void updateDateTime();
        void start();
        void pause();
        void resume();
        void endSession();
        void intervalTimeout();
        void playGong();
    };
} // namespace gui
