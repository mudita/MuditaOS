#pragma once

#include "BaseSettingsWindow.hpp"

namespace gui
{
    class CheckBoxWithLabel;

    class KeypadLightWindow : public BaseSettingsWindow
    {
      public:
        KeypadLightWindow(app::Application *app);
        void buildInterface() override;

      private:
        std::vector<CheckBoxWithLabel *> boxes;
    };
} // namespace gui
