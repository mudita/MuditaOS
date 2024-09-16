// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include <windows/AppsBatteryStatusWindow.hpp>
#include <data/BatteryStatusSwitchData.hpp>

namespace
{
    using namespace std::chrono_literals;
    constexpr auto windowTimeout{5s};
} // namespace

namespace gui
{
    AppsBatteryStatusWindow::AppsBatteryStatusWindow(app::ApplicationCommon *app, const std::string &name)
        : BellBatteryStatusWindow{app, name}
    {}

    bool AppsBatteryStatusWindow::onInput(const InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease(KeyCode::KEY_ENTER) || inputEvent.isShortRelease(KeyCode::KEY_RF)) {
            if (returnCallback != nullptr) {
                detachTimerIfExists();
                returnCallback();
                return true;
            }
        }
        return false;
    }

    bool AppsBatteryStatusWindow::handleSwitchData(SwitchData *data)
    {
        if (data == nullptr) {
            LOG_ERROR("Received nullptr");
            return false;
        }

        const auto batteryData = dynamic_cast<AppsBatteryStatusSwitchData *>(data);
        if (batteryData == nullptr) {
            LOG_ERROR("Received data nullptr");
            return false;
        }

        returnCallback = batteryData->getReturnCallback();
        if (returnCallback != nullptr) {
            timerCallback = [this]([[maybe_unused]] Item &item, [[maybe_unused]] sys::Timer &timer) {
                returnCallback();
                return true;
            };
            resetTimer(windowTimeout);
        }

        return true;
    }
} // namespace gui
