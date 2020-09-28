#pragma once

#include "BaseSettingsWindow.hpp"

namespace gui
{

    class FontSizeWindow : public BaseSettingsWindow
    {
      public:
        FontSizeWindow(app::Application *app);
        void buildInterface() override;
    };
} // namespace gui
