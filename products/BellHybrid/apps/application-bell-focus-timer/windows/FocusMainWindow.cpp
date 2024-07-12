// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "FocusMainWindow.hpp"

#include "FocusSettingsWindow.hpp"
#include "FocusTimerWindow.hpp"

#include <common/options/OptionBellMenu.hpp>
#include <common/data/BatteryStatusSwitchData.hpp>

namespace app::focus
{
    using namespace gui;

    FocusMainWindow::FocusMainWindow(app::ApplicationCommon *app,
                                     std::unique_ptr<app::focus::FocusMainContract::Presenter> &&presenter,
                                     const std::string &name)
        : BellOptionWindow(app, name), presenter(std::move(presenter))
    {
        addOptions(settingsOptionsList());
        setListTitle(utils::translate("app_bellmain_focus_timer"));
    }

    void FocusMainWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        BellOptionWindow::onBeforeShow(mode, data);
        static_cast<app::Application *>(application)->resumeIdleTimer();
    }

    std::list<Option> FocusMainWindow::settingsOptionsList()
    {
        using ActivatedCallback = std::function<bool(Item &)>;
        using Callback          = std::function<ActivatedCallback(const std::string &window)>;

        auto defaultCallback = [this](const std::string &window) {
            return [window, this](Item &) {
                if (window.empty()) {
                    return false;
                }
                application->switchWindow(window);
                return true;
            };
        };

        auto startFocusCallback = [this](const std::string &window) {
            startFocus = [window, this]() {
                if (window.empty()) {
                    return;
                }
                application->switchWindow(window);
            };
            return [window, this](Item &) {
                const auto batteryState = presenter->getBatteryState();
                const auto soc          = batteryState.level;
                const auto isCharging   = presenter->isBatteryCharging(batteryState.state);
                if (!presenter->isLowBatteryWindowHandled() && !isCharging &&
                    presenter->isBatteryBelowLowLevelThreshold(soc)) {
                    auto lowBatterySwitchData =
                        std::make_unique<AppsBatteryStatusSwitchData>(soc, isCharging, startFocus);
                    application->switchWindow(focus::window::name::focusTimerLowBattery,
                                              std::move(lowBatterySwitchData));
                    presenter->handleLowBatteryWindow();
                }
                else {
                    startFocus();
                }
                return true;
            };
        };

        std::list<Option> settingsOptionList;
        auto addWinSettings = [&](const UTF8 &name, const std::string &window, Callback &&callback) {
            settingsOptionList.emplace_back(std::make_unique<option::OptionBellMenu>(
                name,
                callback(window),
                [=](Item &item) {
                    // put focus change callback here
                    return true;
                },
                this));
        };

        addWinSettings(utils::translate("app_bell_focus_start"), window::name::timer, startFocusCallback);
        addWinSettings(utils::translate("app_bell_focus_settings"), window::name::settings, defaultCallback);

        return settingsOptionList;
    }
} // namespace app::focus
