// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "BaseSettingsWindow.hpp"

namespace gui
{
    class KeypadLightWindow : public BaseSettingsWindow
    {
      public:
        KeypadLightWindow(app::Application *app);

      private:
        void switchHandler(bool &onOffSwitch);
        std::list<Option> buildOptionsList() override;

        bool isAlwaysOnSwitchOn = false;
        bool isActiveSwitchOn   = false;
        bool isOffSwitchOn      = false;
    };
} // namespace gui
