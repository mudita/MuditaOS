#pragma once

#include <AppWindow.hpp>
#include <Application.hpp>

#include <application-meditation/widgets/MeditationTimer.hpp>

namespace gui
{
    class MeditationWindow : public AppWindow
    {
      public:
        explicit MeditationWindow(app::Application *app);

        auto onInput(const InputEvent &inputEvent) -> bool override;

        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;

      private:
        MeditationTimer *timer = nullptr;
    };
} // namespace gui
