// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

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
