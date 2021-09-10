// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "MeditationWindow.hpp"
#include <module-sys/Timers/TimerHandle.hpp>

namespace gui
{
    namespace window
    {
        constexpr auto timerName = "MeditationTimer";
    } // namespace window

    class WithTimerWindow : public MeditationWindow
    {
      private:
        sys::TimerHandle timer;

        void detachTimerIfExists();

      public:
        WithTimerWindow() = delete;
        explicit WithTimerWindow(app::Application *app, std::string name);
        ~WithTimerWindow();

        // virtual methods
        void destroyInterface() override;
        bool onInput(const InputEvent &inputEvent) override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;

        void startTimer(uint32_t secs, bool repeat = false);
        void stopTimer();

        virtual void onTimeout() = 0;
    };
} // namespace gui
