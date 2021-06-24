// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AppWindow.hpp"
#include <module-sys/Timers/TimerHandle.hpp>

namespace gui
{
    inline constexpr auto defautTimeout = std::chrono::seconds{3};
    class WindowWithTimer : public gui::AppWindow
    {
      private:
        sys::TimerHandle popupTimer;

      public:
        explicit WindowWithTimer(app::Application *app,
                                 const std::string &name,
                                 const std::chrono::milliseconds timeout = defautTimeout);
        void destroyInterface() override;
        ~WindowWithTimer() override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        bool onInput(const gui::InputEvent &inputEvent) override;

      protected:
        void detachTimerIfExists();
        void resetTimer();
    };
} // namespace gui
