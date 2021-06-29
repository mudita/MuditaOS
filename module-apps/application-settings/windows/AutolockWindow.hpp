// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ApplicationSettings.hpp"

#include <apps-common/windows/BaseSettingsWindow.hpp>

namespace gui
{

    class AutolockWindow : public BaseSettingsWindow
    {
      public:
        AutolockWindow(app::Application *app, app::settingsInterface::AutoLockSettings *autoLockSettings);
        void onBeforeShow(ShowMode mode, SwitchData *data) override;

      private:
        auto buildOptionsList() -> std::list<Option> override;
        std::chrono::seconds currentAutoLockTimeout{0};
        app::settingsInterface::AutoLockSettings *autoLockSettings;
    };
} // namespace gui
