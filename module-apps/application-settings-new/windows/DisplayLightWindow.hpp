#pragma once

#include "BaseSettingsWindow.hpp"

namespace gui
{

    class DisplayLightWindow : public BaseSettingsWindow
    {
      public:
        DisplayLightWindow(app::Application *app);
        void buildInterface() override;
    };
} // namespace gui
