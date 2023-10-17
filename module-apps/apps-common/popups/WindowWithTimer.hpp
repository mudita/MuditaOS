// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AppWindow.hpp"
#include <Timers/TimerHandle.hpp>

namespace gui
{
    inline constexpr auto defautTimeout   = std::chrono::seconds{3};
    inline constexpr auto noTimeoutChange = std::chrono::seconds::zero();
    class WindowWithTimer : public gui::AppWindow
    {
      private:
        sys::TimerHandle popupTimer;
        std::chrono::milliseconds timeout;

      public:
        explicit WindowWithTimer(app::ApplicationCommon *app,
                                 const std::string &name,
                                 const std::chrono::milliseconds timeout = defautTimeout);
        void destroyInterface() override;
        ~WindowWithTimer() override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void onClose(CloseReason reason) override;
        bool onInput(const gui::InputEvent &inputEvent) override;

      protected:
        void detachTimerIfExists();
        void resetTimer(const std::chrono::seconds newTimeout = noTimeoutChange);
    };
} // namespace gui
