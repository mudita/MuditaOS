#pragma once

#include <AppWindow.hpp>
#include <Application.hpp>


namespace gui
{
    class IntervalBox;
    class CircularTimeSelector;

    class MeditationWindow : public AppWindow
    {
      public:
        explicit MeditationWindow(app::Application *app);

        auto onInput(const InputEvent &inputEvent) -> bool override;

        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;

      private:
        CircularTimeSelector *timeSelector = nullptr;
        IntervalBox *intervalBox           = nullptr;
    };
} // namespace gui
