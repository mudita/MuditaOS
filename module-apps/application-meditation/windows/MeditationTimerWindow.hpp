#pragma once

#include "AppWindow.hpp"
#include "application-meditation/widgets/MeditationTimer.hpp"

namespace gui
{
    class MeditationTimerWindow : public AppWindow
    {
        MeditationTimer *timer      = nullptr;
        Text *meditationInfo        = nullptr;
        int meditationTimeInSeconds = 0;
        bool finished               = false;

        void setWidgetVisible(bool topBar, bool bottomBar, bool counter);
        void setVisibleRunning();
        void setVisiblePaused();
        void setVisiblePreparation();
        void setVisibleMeditationEnd();

      public:
        explicit MeditationTimerWindow(app::Application *app);

        auto onInput(const InputEvent &inputEvent) -> bool override;

        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
    };
} // namespace gui
