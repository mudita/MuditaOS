// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <AppWindow.hpp>
#include <Timers/TimerHandle.hpp>

#include <chrono>

namespace gui
{
    class MeditationTimer;
    class Text;

    class MeditationTimerWindow : public AppWindow
    {
        MeditationTimer *timer = nullptr;
        Text *meditationInfo   = nullptr;
        bool finished          = false;
        std::chrono::seconds meditationTime{0};
        std::chrono::seconds meditationIntervalPeriod{0};
        sys::TimerHandle endScreenTimeout;

        void setWidgetVisible(bool sBar, bool navBar, bool counter);
        void setVisibleRunning();
        void setVisiblePaused();
        void setVisiblePreparation();
        void setVisibleMeditationEnd();

        void invalidate() noexcept;

      public:
        explicit MeditationTimerWindow(app::ApplicationCommon *app);

        auto onInput(const InputEvent &inputEvent) -> bool override;

        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
    };
} // namespace gui
