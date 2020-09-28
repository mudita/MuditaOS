#pragma once

#include "BaseSettingsWindow.hpp"

namespace gui
{

    class InputLanguageWindow : public BaseSettingsWindow
    {
      public:
        InputLanguageWindow(app::Application *app);
        void buildInterface() override;
    };
} // namespace gui
