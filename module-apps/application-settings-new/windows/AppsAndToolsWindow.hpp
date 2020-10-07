#pragma once

#include <OptionWindow.hpp>

namespace gui
{
    class AppsAndToolsWindow : public OptionWindow
    {
      public:
        AppsAndToolsWindow(app::Application *app);
        ~AppsAndToolsWindow() override = default;

      private:
        std::list<Option> appsAndToolsOptionsList();
    };
} // namespace gui
