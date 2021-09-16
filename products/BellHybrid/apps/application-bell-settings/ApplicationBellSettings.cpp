// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationBellSettings.hpp"
#include "FrontlightPresenter.hpp"
#include "PrewakeUpPresenter.hpp"
#include "TimeUnitsPresenter.hpp"
#include "models/FrontlightModel.hpp"
#include "models/PrewakeUpModel.hpp"
#include "models/TemperatureUnitModel.hpp"
#include "models/alarm_settings/SnoozeListItemProvider.hpp"
#include "presenter/alarm_settings/SnoozePresenter.hpp"
#include "windows/advanced/BellSettingsAdvancedWindow.hpp"
#include "windows/advanced/BellSettingsTimeUnitsWindow.hpp"
#include "windows/alarm_settings/BellSettingsAlarmSettingsWindow.hpp"
#include "windows/alarm_settings/BellSettingsAlarmSettingsSnoozeWindow.hpp"
#include "windows/BellSettingsBedtimeToneWindow.hpp"
#include "windows/BellSettingsFrontlight.hpp"
#include "windows/BellSettingsHomeViewWindow.hpp"
#include "windows/BellSettingsPrewakeUpWindow.hpp"
#include "windows/BellSettingsTurnOffWindow.hpp"
#include "windows/BellSettingsWindow.hpp"

#include <apps-common/windows/Dialog.hpp>
#include <common/models/SnoozeSettingsModel.hpp>
#include <common/BellFinishedWindow.hpp>
#include <service-evtmgr/Constants.hpp>
#include <service-evtmgr/EventManagerServiceAPI.hpp>
#include <service-evtmgr/ScreenLightControlMessage.hpp>

namespace app
{
    ApplicationBellSettings::ApplicationBellSettings(std::string name,
                                                     std::string parent,
                                                     sys::phone_modes::PhoneMode mode,
                                                     sys::bluetooth::BluetoothMode bluetoothMode,
                                                     StartInBackground startInBackground)
        : Application(std::move(name), std::move(parent), mode, bluetoothMode, startInBackground)
    {}

    sys::ReturnCodes ApplicationBellSettings::InitHandler()
    {
        auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success) {
            return ret;
        }
        createUserInterface();

        return sys::ReturnCodes::Success;
    }

    void ApplicationBellSettings::createUserInterface()
    {
        windowsFactory.attach(gui::name::window::main_window, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::BellSettingsWindow>(app);
        });

        // Advanced
        windowsFactory.attach(gui::window::name::bellSettingsAdvanced,
                              [](ApplicationCommon *app, const std::string &name) {
                                  return std::make_unique<gui::BellSettingsAdvancedWindow>(app);
                              });

        windowsFactory.attach(
            gui::window::name::bellSettingsTimeUnits, [](ApplicationCommon *app, const std::string &name) {
                auto temperatureUnitModel = std::make_unique<bell_settings::TemperatureUnitModel>(app);
                auto timeUnitsProvider    = std::make_shared<bell_settings::TimeUnitsModel>(app);
                auto presenter            = std::make_unique<bell_settings::TimeUnitsWindowPresenter>(
                    timeUnitsProvider, std::move(temperatureUnitModel));
                return std::make_unique<gui::BellSettingsTimeUnitsWindow>(app, std::move(presenter));
            });

        windowsFactory.attach(
            gui::window::name::bellSettingsFrontlight, [](ApplicationCommon *app, const std::string &name) {
                auto model =
                    std::make_shared<bell_settings::FrontlightModel>(app, static_cast<ApplicationBellSettings *>(app));
                auto presenter = std::make_unique<bell_settings::FrontlightWindowPresenter>(std::move(model));
                return std::make_unique<gui::BellSettingsFrontlightWindow>(app, std::move(presenter));
            });

        windowsFactory.attach(gui::BellFinishedWindow::name, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::BellFinishedWindow>(app);
        });

        windowsFactory.attach(gui::window::name::bellSettingsHomeView,
                              [](ApplicationCommon *app, const std::string &name) {
                                  return std::make_unique<gui::BellSettingsHomeViewWindow>(app);
                              });

        windowsFactory.attach(gui::window::name::bellSettingsBedtimeTone,
                              [](ApplicationCommon *app, const std::string &name) {
                                  return std::make_unique<gui::BellSettingsBedtimeToneWindow>(app);
                              });

        windowsFactory.attach(gui::window::name::bellSettingsTurnOff,
                              [](ApplicationCommon *app, const std::string &name) {
                                  return std::make_unique<gui::BellSettingsTurnOffWindow>(app);
                              });

        windowsFactory.attach(
            gui::window::name::bellSettingsAlarmSettingsPrewakeUp, [](ApplicationCommon *app, const std::string &name) {
                auto model     = std::make_shared<bell_settings::PrewakeUpModel>(app);
                auto presenter = std::make_unique<bell_settings::PrewakeUpWindowPresenter>(std::move(model));
                return std::make_unique<gui::BellSettingsPrewakeUpWindow>(app, std::move(presenter));
            });

        // Alarm setup
        windowsFactory.attach(gui::BellSettingsAlarmSettingsWindow::name,
                              [](ApplicationCommon *app, const std::string &name) {
                                  return std::make_unique<gui::BellSettingsAlarmSettingsWindow>(app);
                              });
        windowsFactory.attach(
            gui::BellSettingsAlarmSettingsSnoozeWindow::name, [this](ApplicationCommon *app, const std::string &) {
                auto model     = std::make_unique<bell_settings::SnoozeSettingsModel>(this);
                auto provider  = std::make_shared<bell_settings::SnoozeListItemProvider>(*model);
                auto presenter = std::make_unique<bell_settings::SnoozePresenter>(provider, std::move(model));
                return std::make_unique<gui::BellSettingsAlarmSettingsSnoozeWindow>(app, std::move(presenter));
            });

        attachPopups({gui::popup::ID::AlarmActivated, gui::popup::ID::AlarmDeactivated});
    }

    sys::MessagePointer ApplicationBellSettings::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
    {
        auto retMsg = Application::DataReceivedHandler(msgl);
        if (dynamic_cast<sys::ResponseMessage *>(retMsg.get())->retCode == sys::ReturnCodes::Success) {
            return retMsg;
        }

        // handle database response
        if (resp != nullptr) {
            if (auto command = callbackStorage->getCallback(resp); command->execute()) {
                refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
            }
            return sys::msgHandled();
        }
        return handleAsyncResponse(resp);
    }

    void ApplicationBellSettings::setBrightness(bsp::eink_frontlight::BrightnessPercentage value)
    {
        screen_light_control::ManualModeParameters parameters{value};
        bus.sendUnicast(std::make_shared<sevm::ScreenLightSetManualModeParams>(parameters), service::name::evt_manager);
    }

    void ApplicationBellSettings::setMode(bool isAutoLightSwitchOn)
    {
        bus.sendUnicast(std::make_shared<sevm::ScreenLightControlMessage>(
                            isAutoLightSwitchOn ? screen_light_control::Action::enableAutomaticMode
                                                : screen_light_control::Action::disableAutomaticMode),
                        service::name::evt_manager);
    }

    void ApplicationBellSettings::setStatus(bool isDisplayLightSwitchOn)
    {
        bus.sendUnicast(std::make_shared<sevm::ScreenLightControlMessage>(isDisplayLightSwitchOn
                                                                              ? screen_light_control::Action::turnOn
                                                                              : screen_light_control::Action::turnOff),
                        service::name::evt_manager);
    }
} // namespace app
