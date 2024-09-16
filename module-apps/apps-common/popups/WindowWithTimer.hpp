// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "AppWindow.hpp"
#include <Timers/TimerHandle.hpp>

namespace gui
{
    class WindowWithTimer : public gui::AppWindow
    {
      public:
        static constexpr auto defaultTimeout  = std::chrono::seconds{3};
        static constexpr auto noTimeoutChange = std::chrono::seconds::zero();

        WindowWithTimer(app::ApplicationCommon *app,
                        const std::string &name,
                        std::chrono::milliseconds timeout = defaultTimeout);
        ~WindowWithTimer() override;

        void destroyInterface() override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void onClose(CloseReason reason) override;
        bool onInput(const gui::InputEvent &inputEvent) override;

        /// Called before automatic switch to previous window at window's timer timeout.
        void setWindowTimerActionCallback(std::function<void()> &&callback);

      protected:
        void detachTimerIfExists();
        void resetTimer(std::chrono::seconds newTimeout = noTimeoutChange);

      private:
        sys::TimerHandle popupTimer;
        std::chrono::milliseconds timeout;
        std::function<void()> windowTimerActionCallback;
    };
} // namespace gui
