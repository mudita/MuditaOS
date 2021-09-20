// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-settings/windows/BaseSettingsWindow.hpp>

namespace app::settingsInterface
{
    class AutoLockSettings;
}; // namespace app::settingsInterface

namespace gui
{
    class AutolockWindow : public BaseSettingsWindow
    {
      public:
        AutolockWindow(app::ApplicationCommon *app, app::settingsInterface::AutoLockSettings *autoLockSettings);
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        std::chrono::seconds getTime() const
        {
            return time;
        }
        void setTime(std::chrono::seconds t)
        {
            time = t;
        };

      private:
        auto buildOptionsList() -> std::list<Option> override;
        std::chrono::seconds currentAutoLockTimeout{0};
        app::settingsInterface::AutoLockSettings *autoLockSettings;
        std::chrono::seconds time;
    };
} // namespace gui
