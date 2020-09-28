#pragma once

#include "BaseSettingsWindow.hpp"

namespace gui
{

    class KeypadLightWindow : public BaseSettingsWindow
    {
      public:
        KeypadLightWindow(app::Application *app);
        void buildInterface() override;
    };
} // namespace gui
