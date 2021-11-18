// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <Application.hpp>

namespace app
{
    void Application::updateStatuses(gui::AppWindow *window) const
    {
        window->updateBatteryStatus();
        window->updateBluetooth(statusIndicators.bluetoothMode);
        window->updateAlarmClock(statusIndicators.alarmClockStatus);
        window->updateSim();
        window->updateSignalStrength();
        window->updateNetworkAccessTechnology();
        window->updateTime();
        window->updatePhoneMode(statusIndicators.phoneMode);
    }
} // namespace app