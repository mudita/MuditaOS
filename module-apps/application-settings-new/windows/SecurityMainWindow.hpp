// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "BaseSettingsWindow.hpp"

namespace gui
{
    class SecurityMainWindow : public BaseSettingsWindow
    {
      public:
        explicit SecurityMainWindow(app::Application *app, app::settingsInterface::SecuritySettings *settings);

      private:
        auto buildOptionsList() -> std::list<Option> override;

        bool lockScreenPasscodeIsOn;
        app::settingsInterface::SecuritySettings *securitySettings;
    };
} // namespace gui
