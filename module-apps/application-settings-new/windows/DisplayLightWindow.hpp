// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "BaseSettingsWindow.hpp"

namespace gui
{

    class DisplayLightWindow : public BaseSettingsWindow
    {
      public:
        DisplayLightWindow(app::Application *app);

      private:
        void switchHandler(bool &onOffSwitch);
        auto buildOptionsList() -> std::list<Option> override;

        bool isDisplayLightSwitchOn = false;
        bool isAutoLightSwitchOn    = false;
    };
} // namespace gui
