#pragma once

#include <AppWindow.hpp>

namespace gui
{
    class IntervalBox;
    class TimerSetter;

    class MeditationWindow : public AppWindow
    {
      public:
        explicit MeditationWindow(app::Application *app);

        auto onInput(const InputEvent &inputEvent) -> bool override;

        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;

      private:
        void invalidate() noexcept;
        TimerSetter *timeSetter  = nullptr;
        IntervalBox *intervalBox = nullptr;
    };
} // namespace gui
