#pragma once

#include <functional>
#include <string>

#include "../BaseSettingsWindow.hpp"
#include "gui/widgets/Image.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Window.hpp"
#include <memory>
#include <BoxLayout.hpp>

namespace gui
{

    class InputLanguageWindow : public SettingsWindow
    {
      public:
        InputLanguageWindow(app::Application *app);
        void buildInterface() override;

        static constexpr const char *window_name = "InputLanguageWindow";
    };
} // namespace gui
