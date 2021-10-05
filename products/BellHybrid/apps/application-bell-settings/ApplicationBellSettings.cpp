// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationBellSettings.hpp"
#include "FrontlightPresenter.hpp"
#include "presenter/TimeUnitsPresenter.hpp"
#include "models/FrontlightModel.hpp"
#include "models/TemperatureUnitModel.hpp"
#include "models/advanced/AboutYourBellModel.hpp"
#include "models/alarm_settings/AlarmSettingsListItemProvider.hpp"
#include "models/alarm_settings/AlarmSettingsModel.hpp"
#include "models/alarm_settings/PrewakeUpListItemProvider.hpp"
#include "models/alarm_settings/PrewakeUpSettingsModel.hpp"
#include "models/alarm_settings/SnoozeListItemProvider.hpp"
#include "models/alarm_settings/SnoozeSettingsModel.hpp"
#include "presenter/advanced/AboutYourBellWindowPresenter.hpp"
#include "presenter/alarm_settings/SnoozePresenter.hpp"
#include "windows/advanced/AboutYourBellWindow.hpp"
#include "windows/advanced/BellSettingsAdvancedWindow.hpp"
#include "windows/advanced/BellSettingsTimeUnitsWindow.hpp"
#include "windows/advanced/BellSettingsLanguageWindow.hpp"
#include "windows/alarm_settings/BellSettingsAlarmSettingsMenuWindow.hpp"
#include "windows/alarm_settings/BellSettingsAlarmSettingsSnoozeWindow.hpp"
#include "windows/alarm_settings/BellSettingsAlarmSettingsWindow.hpp"
#include "windows/alarm_settings/BellSettingsPrewakeUpWindow.hpp"
#include "windows/BellSettingsBedtimeToneWindow.hpp"
#include "windows/BellSettingsFrontlight.hpp"
#include "windows/BellSettingsHomeViewWindow.hpp"
#include "windows/BellSettingsWindow.hpp"

#include <apps-common/windows/Dialog.hpp>
#include <common/windows/BellFinishedWindow.hpp>
#include <common/windows/BellTurnOffWindow.hpp>
#include <common/popups/BellTurnOffOptionWindow.hpp>
#include <service-evtmgr/Constants.hpp>
#include <service-evtmgr/EventManagerServiceAPI.hpp>
#include <service-evtmgr/ScreenLightControlMessage.hpp>
#include <service-appmgr/messages/GetCurrentDisplayLanguageResponse.hpp>

namespace app
{
    ApplicationBellSettings::ApplicationBellSettings(std::string name,
                                                     std::string parent,
                                                     StatusIndicators statusIndicators,
                                                     StartInBackground startInBackground)
        : Application(std::move(name), std::move(parent), statusIndicators, startInBackground)
    {}

    sys::ReturnCodes ApplicationBellSettings::InitHandler()
    {
        auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success) {
            return ret;
        }
        createUserInterface();

        connect(typeid(manager::GetCurrentDisplayLanguageResponse), [&](sys::Message *msg) {
            if (gui::window::name::bellSettingsLanguage == getCurrentWindow()->getName()) {

                switchWindow(gui::window::bell_finished::defaultName,
                             gui::BellFinishedWindowData::Factory::create(
                                 "big_check_W_G",
                                 utils::translate("app_bell_settings_advanced_language_set"),
                                 gui::window::name::bellSettingsAdvanced));

                return sys::msgHandled();
            }
            return sys::msgNotHandled();
        });

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
            gui::window::name::bellSettingsLanguage, [&](ApplicationCommon *app, const std::string &name) {
                auto presenter = std::make_unique<bell_settings::LanguageWindowPresenter>(this);
                return std::make_unique<gui::BellSettingsLanguageWindow>(app, std::move(presenter), name);
            });

        windowsFactory.attach(
            gui::window::name::bellSettingsFrontlight, [](ApplicationCommon *app, const std::string &name) {
                auto model =
                    std::make_shared<bell_settings::FrontlightModel>(app, static_cast<ApplicationBellSettings *>(app));
                auto presenter = std::make_unique<bell_settings::FrontlightWindowPresenter>(std::move(model));
                return std::make_unique<gui::BellSettingsFrontlightWindow>(app, std::move(presenter));
            });

        windowsFactory.attach(gui::window::bell_finished::defaultName,
                              [](ApplicationCommon *app, const std::string &name) {
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

        windowsFactory.attach(gui::BellTurnOffOptionWindow::defaultName,
                              [](ApplicationCommon *app, const std::string &name) {
                                  return std::make_unique<gui::BellTurnOffOptionWindow>(app);
                              });

        windowsFactory.attach(gui::BellTurnOffWindow::name, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::BellTurnOffWindow>(app, std::make_unique<gui::PowerOffPresenter>(app));
        });

        windowsFactory.attach(
            gui::BellSettingsPrewakeUpWindow::name, [this](ApplicationCommon *app, const std::string &name) {
                auto chimeDurationModel = std::make_unique<bell_settings::PrewakeUpChimeDurationModel>(this);
                auto chimeToneModel     = std::make_unique<bell_settings::PrewakeUpChimeToneModel>(this);
                auto chimeVolumeModel   = std::make_unique<bell_settings::PrewakeUpChimeVolumeModel>(this);
                auto lightDurationModel = std::make_unique<bell_settings::PrewakeUpLightDurationModel>(this);
                auto prewakeUpSettingsModel =
                    std::make_unique<bell_settings::PrewakeUpSettingsModel>(std::move(chimeDurationModel),
                                                                            std::move(chimeToneModel),
                                                                            std::move(chimeVolumeModel),
                                                                            std::move(lightDurationModel));
                auto provider  = std::make_shared<bell_settings::PrewakeUpListItemProvider>(*prewakeUpSettingsModel);
                auto presenter = std::make_unique<bell_settings::PrewakeUpWindowPresenter>(
                    provider, std::move(prewakeUpSettingsModel));
                return std::make_unique<gui::BellSettingsPrewakeUpWindow>(app, std::move(presenter));
            });

        // Alarm setup
        windowsFactory.attach(gui::BellSettingsAlarmSettingsMenuWindow::name,
                              [](ApplicationCommon *app, const std::string &name) {
                                  return std::make_unique<gui::BellSettingsAlarmSettingsMenuWindow>(app);
                              });
        windowsFactory.attach(
            gui::BellSettingsAlarmSettingsSnoozeWindow::name, [this](ApplicationCommon *app, const std::string &) {
                auto snoozeOnOffModel         = std::make_unique<bell_settings::SnoozeOnOffModel>(this);
                auto snoozeLengthModel        = std::make_unique<bell_settings::SnoozeLengthModel>(this);
                auto snoozeChimeIntervalModel = std::make_unique<bell_settings::SnoozeChimeIntervalModel>(this);
                auto snoozeChimeToneModel     = std::make_unique<bell_settings::SnoozeChimeToneModel>(this);
                auto snoozeChimeVolumeModel   = std::make_unique<bell_settings::SnoozeChimeVolumeModel>(this);
                auto snoozeSettingsModel =
                    std::make_unique<bell_settings::SnoozeSettingsModel>(std::move(snoozeOnOffModel),
                                                                         std::move(snoozeLengthModel),
                                                                         std::move(snoozeChimeIntervalModel),
                                                                         std::move(snoozeChimeToneModel),
                                                                         std::move(snoozeChimeVolumeModel));
                auto provider = std::make_shared<bell_settings::SnoozeListItemProvider>(*snoozeSettingsModel);
                auto presenter =
                    std::make_unique<bell_settings::SnoozePresenter>(provider, std::move(snoozeSettingsModel));
                return std::make_unique<gui::BellSettingsAlarmSettingsSnoozeWindow>(app, std::move(presenter));
            });
        windowsFactory.attach(
            gui::BellSettingsAlarmSettingsWindow::name, [this](ApplicationCommon *app, const std::string &) {
                auto alarmToneModel       = std::make_unique<bell_settings::AlarmToneModel>(this);
                auto alarmVolumeModel     = std::make_unique<bell_settings::AlarmVolumeModel>(this);
                auto alarmLightOnOffModel = std::make_unique<bell_settings::AlarmLightOnOffModel>(this);
                auto alarmSettingsModel   = std::make_unique<bell_settings::AlarmSettingsModel>(
                    std::move(alarmToneModel), std::move(alarmVolumeModel), std::move(alarmLightOnOffModel));
                auto provider = std::make_shared<bell_settings::AlarmSettingsListItemProvider>(*alarmSettingsModel);
                auto presenter =
                    std::make_unique<bell_settings::AlarmSettingsPresenter>(provider, std::move(alarmSettingsModel));
                return std::make_unique<gui::BellSettingsAlarmSettingsWindow>(app, std::move(presenter));
            });

        windowsFactory.attach(
            gui::AboutYourBellWindow::name, [](ApplicationCommon *app, [[maybe_unused]] const std::string &name) {
                auto aboutYourBellModel = std::make_shared<bell_settings::AboutYourBellModel>(app);
                auto aboutYourBellPresenter =
                    std::make_unique<bell_settings::AboutYourBellWindowPresenter>(std::move(aboutYourBellModel));
                return std::make_unique<gui::AboutYourBellWindow>(app, std::move(aboutYourBellPresenter));
            });

        attachPopups({gui::popup::ID::AlarmActivated,
                      gui::popup::ID::AlarmDeactivated,
                      gui::popup::ID::PowerOff,
                      gui::popup::ID::Reboot});
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
