#pragma once

#include <OptionWindow.hpp>

namespace gui
{
    class DisplayAndKeypadWindow : public OptionWindow
    {
      public:
        DisplayAndKeypadWindow(app::Application *app);
        ~DisplayAndKeypadWindow() override = default;

      private:
        std::list<Option> displayAndKeypadOptionsList();
    };
} // namespace gui
