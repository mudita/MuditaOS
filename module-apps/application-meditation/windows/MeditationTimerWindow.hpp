#pragma once

#include "AppWindow.hpp"

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

        void setWidgetVisible(bool topBar, bool bottomBar, bool counter);
        void setVisibleRunning();
        void setVisiblePaused();
        void setVisiblePreparation();
        void setVisibleMeditationEnd();

        void invalidate() noexcept;

      public:
        explicit MeditationTimerWindow(app::Application *app);

        auto onInput(const InputEvent &inputEvent) -> bool override;

        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
    };
} // namespace gui
