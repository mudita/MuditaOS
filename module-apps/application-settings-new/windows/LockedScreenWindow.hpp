#pragma once

#include "BaseSettingsWindow.hpp"

namespace gui
{
    class LockedScreenWindow : public BaseSettingsWindow
    {
      public:
        LockedScreenWindow(app::Application *app);
        void buildInterface() override;
    };
} // namespace gui
