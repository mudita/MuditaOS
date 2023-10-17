// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <appmgr/ApplicationManager.hpp>
#include <application-special-input/ApplicationSpecialInput.hpp>

namespace app::manager
{
    void ApplicationManager::runAppsInBackground()
    {
        for (const auto &name : std::vector<ApplicationName>{app::name_special_input}) {
            if (auto app = getApplication(name); app != nullptr) {
                StatusIndicators statusIndicators;
                statusIndicators.phoneMode        = phoneModeObserver->getCurrentPhoneMode();
                statusIndicators.tetheringState   = phoneModeObserver->isTetheringOn() ? sys::phone_modes::Tethering::On
                                                                                       : sys::phone_modes::Tethering::Off;
                statusIndicators.bluetoothMode    = bluetoothMode;
                statusIndicators.alarmClockStatus = alarmClockStatus;
                app->runInBackground(statusIndicators, this);
            }
        }
    }
} // namespace app::manager
