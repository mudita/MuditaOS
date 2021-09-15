// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "widgets/UnityProgressBar.hpp"
#include <Application.hpp>
#include <InputEvent.hpp>
#include <Utils.hpp>
#include <Style.hpp>
#include <BoxLayout.hpp>
#include <Label.hpp>

#include "WithTimerWindow.hpp"

namespace gui
{
    namespace name::window
    {
        inline constexpr auto meditation_running       = "Meditation countdown";
        inline constexpr auto meditation_running_title = "Meditation timer";
    } // namespace name::window

    class MeditationRunningWindow : public WithTimerWindow
    {
      public:
        explicit MeditationRunningWindow(app::Application *app);

        // virtual methods
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        bool onInput(const InputEvent &inputEvent) override;
        void buildInterface() override;
        void destroyInterface() override;
        void onTimeout() override;

      private:
        gui::Label *datetime        = nullptr;
        gui::Label *title           = nullptr;
        gui::Label *timer           = nullptr;
        UnityProgressBar *progress  = nullptr;
        std::chrono::seconds passedTimer    = std::chrono::seconds::zero();
        std::chrono::seconds passedInterval = std::chrono::seconds::zero();

        void updateProgress();
        void updateDateTime();
        void updateTimer();
        void start();
        void pause();
        void resume();
        void intervalTimeout();
        void endSession();
        void playGong();
    };
} // namespace gui
