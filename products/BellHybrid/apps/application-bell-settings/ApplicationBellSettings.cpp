// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationBellSettings.hpp"
#include "presenter/TimeUnitsPresenter.hpp"
#include "presenter/LayoutWindowPresenter.hpp"
#include "models/TemperatureUnitModel.hpp"
#include "models/AboutYourBellModel.hpp"
#include "models/alarm_settings/AlarmSettingsListItemProvider.hpp"
#include "models/alarm_settings/PrewakeUpListItemProvider.hpp"
#include "models/alarm_settings/BedtimeSettingsListItemProvider.hpp"
#include "models/alarm_settings/PrewakeUpSettingsModel.hpp"
#include "models/alarm_settings/SnoozeListItemProvider.hpp"
#include "models/alarm_settings/SnoozeSettingsModel.hpp"
#include "presenter/BedtimeSettingsPresenter.hpp"
#include "presenter/AboutYourBellWindowPresenter.hpp"
#include "presenter/alarm_settings/SnoozePresenter.hpp"
#include "presenter/FrontlightPresenter.hpp"
#include "windows/AboutYourBellWindow.hpp"
#include "windows/BellSettingsLanguageWindow.hpp"
#include "windows/BellSettingsLayoutWindow.hpp"
#include "windows/BellSettingsFrontlightWindow.hpp"
#include "windows/alarm_settings/BellSettingsAlarmSettingsMenuWindow.hpp"
#include "windows/alarm_settings/BellSettingsAlarmSettingsSnoozeWindow.hpp"
#include "windows/alarm_settings/BellSettingsAlarmSettingsWindow.hpp"
#include "windows/alarm_settings/BellSettingsPrewakeUpWindow.hpp"
#include "windows/BellSettingsBedtimeToneWindow.hpp"
#include "windows/BellSettingsHomeViewWindow.hpp"
#include "windows/BellSettingsTimeUnitsWindow.hpp"
#include "windows/BellSettingsWindow.hpp"
#include "widgets/DialogYesNo.hpp"

#include <AlarmSoundPaths.hpp>
#include <apps-common/windows/Dialog.hpp>
#include <common/layouts/BaseHomeScreenLayoutProvider.hpp>
#include <common/BellPowerOffPresenter.hpp>
#include <common/models/BedtimeModel.hpp>
#include <common/models/LayoutModel.hpp>
#include <common/windows/BellFinishedWindow.hpp>
#include <common/windows/BellTurnOffWindow.hpp>
#include <common/popups/BellTurnOffOptionWindow.hpp>
#include <common/models/AudioModel.hpp>
#include <common/models/TimeModel.hpp>
#include <common/models/AlarmSettingsModel.hpp>
#include <service-evtmgr/EventManagerServiceAPI.hpp>
#include <service-appmgr/messages/GetCurrentDisplayLanguageResponse.hpp>

namespace app
{
    ApplicationBellSettings::ApplicationBellSettings(std::string name,
                                                     std::string parent,
                                                     StatusIndicators statusIndicators,
                                                     StartInBackground startInBackground,
                                                     uint32_t stackDepth)
        : Application(std::move(name), std::move(parent), statusIndicators, startInBackground, stackDepth),
          audioModel{std::make_unique<AudioModel>(this)}
    {
        bus.channels.push_back(sys::BusChannel::ServiceAudioNotifications);
    }

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
                             gui::BellFinishedWindowData::Factory::create("circle_success_big",
                                                                          gui::window::name::bellSettings));

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

        windowsFactory.attach(
            gui::window::name::bellSettingsLanguage, [&](ApplicationCommon *app, const std::string &name) {
                auto presenter = std::make_unique<bell_settings::LanguageWindowPresenter>(this);
                return std::make_unique<gui::BellSettingsLanguageWindow>(app, std::move(presenter), name);
            });

        windowsFactory.attach(
            gui::window::name::bellSettingsLayout, [this](ApplicationCommon *app, const std::string &name) {
                auto layoutModel = std::make_unique<bell_settings::LayoutModel>(this);
                auto timeModel   = std::make_unique<app::TimeModel>();
                auto presenter   = std::make_unique<bell_settings::LayoutWindowPresenter>(
                    this, std::move(layoutModel), std::move(timeModel));
                return std::make_unique<gui::BellSettingsLayoutWindow>(app, std::move(presenter), name);
            });

        windowsFactory.attach(
            gui::BellSettingsFrontlightWindow::name, [](ApplicationCommon *app, const std::string &name) {
                auto model    = std::make_unique<bell_settings::FrontlightModel>(app);
                auto provider = std::make_shared<bell_settings::FrontlightListItemProvider>(*model);
                auto presenter =
                    std::make_unique<bell_settings::FrontlightPresenter>(std::move(provider), std::move(model));
                return std::make_unique<gui::BellSettingsFrontlightWindow>(app, std::move(presenter));
            });

        windowsFactory.attach(gui::window::bell_finished::defaultName,
                              [](ApplicationCommon *app, const std::string &name) {
                                  return std::make_unique<gui::BellFinishedWindow>(app, name);
                              });

        windowsFactory.attach(gui::window::name::bellSettingsHomeView,
                              [](ApplicationCommon *app, const std::string &name) {
                                  return std::make_unique<gui::BellSettingsHomeViewWindow>(app);
                              });

        windowsFactory.attach(
            gui::window::name::bellSettingsBedtimeTone, [this](ApplicationCommon *app, const std::string &name) {
                auto bedtimeModel = std::make_shared<bell_bedtime::BedtimeModel>(app, *audioModel);
                auto soundsRepository =
                    std::make_unique<SoundsRepository>(alarms::paths::getBedtimeReminderChimesDir());
                auto provider = std::make_shared<bell_settings::BedtimeSettingsListItemProvider>(
                    bedtimeModel, soundsRepository->getSongTitles());
                auto presenter = std::make_unique<bell_settings::BedtimeSettingsPresenter>(
                    provider, bedtimeModel, *audioModel, std::move(soundsRepository));
                return std::make_unique<gui::BellSettingsBedtimeToneWindow>(app, std::move(presenter));
            });

        windowsFactory.attach(
            gui::window::name::bellSettingsTimeUnits, [this](ApplicationCommon *app, const std::string &name) {
                auto layoutModel          = std::make_unique<bell_settings::LayoutModel>(this);
                auto temperatureUnitModel = std::make_unique<bell_settings::TemperatureUnitModel>(app);
                auto timeUnitsProvider    = std::make_shared<bell_settings::TimeUnitsModel>(app);
                auto presenter            = std::make_unique<bell_settings::TimeUnitsWindowPresenter>(
                    this, timeUnitsProvider, std::move(temperatureUnitModel), std::move(layoutModel));
                return std::make_unique<gui::BellSettingsTimeUnitsWindow>(app, std::move(presenter));
            });

        windowsFactory.attach(gui::BellTurnOffOptionWindow::defaultName,
                              [](ApplicationCommon *app, const std::string &name) {
                                  return std::make_unique<gui::BellTurnOffOptionWindow>(app);
                              });

        windowsFactory.attach(gui::BellTurnOffWindow::name, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::BellTurnOffWindow>(app, std::make_unique<gui::BellPowerOffPresenter>(app));
        });

        windowsFactory.attach(
            gui::BellSettingsPrewakeUpWindow::name, [this](ApplicationCommon *app, const std::string &name) {
                auto chimeDurationModel = std::make_unique<bell_settings::PrewakeUpChimeDurationModel>(this);
                auto chimeToneModel     = std::make_unique<bell_settings::PrewakeUpChimeToneModel>(this);
                auto chimeVolumeModel   = std::make_unique<bell_settings::PrewakeUpChimeVolumeModel>(*audioModel);
                auto lightDurationModel = std::make_unique<bell_settings::PrewakeUpLightDurationModel>(this);
                auto prewakeUpSettingsModel =
                    std::make_unique<bell_settings::PrewakeUpSettingsModel>(std::move(chimeDurationModel),
                                                                            std::move(chimeToneModel),
                                                                            std::move(chimeVolumeModel),
                                                                            std::move(lightDurationModel));
                auto soundsRepository = std::make_unique<SoundsRepository>(alarms::paths::getPreWakeUpChimesDir());
                auto provider         = std::make_shared<bell_settings::PrewakeUpListItemProvider>(
                    *prewakeUpSettingsModel, soundsRepository->getSongTitles());
                auto presenter = std::make_unique<bell_settings::PrewakeUpWindowPresenter>(
                    provider, std::move(prewakeUpSettingsModel), *audioModel, std::move(soundsRepository));
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
                auto snoozeChimeVolumeModel   = std::make_unique<bell_settings::SnoozeChimeVolumeModel>(*audioModel);
                auto snoozeSettingsModel =
                    std::make_unique<bell_settings::SnoozeSettingsModel>(std::move(snoozeOnOffModel),
                                                                         std::move(snoozeLengthModel),
                                                                         std::move(snoozeChimeIntervalModel),
                                                                         std::move(snoozeChimeToneModel),
                                                                         std::move(snoozeChimeVolumeModel));
                auto soundsRepository = std::make_unique<SoundsRepository>(alarms::paths::getSnoozeChimesDir());
                auto provider         = std::make_shared<bell_settings::SnoozeListItemProvider>(
                    *snoozeSettingsModel, soundsRepository->getSongTitles());
                auto presenter = std::make_unique<bell_settings::SnoozePresenter>(
                    provider, std::move(snoozeSettingsModel), *audioModel, std::move(soundsRepository));
                return std::make_unique<gui::BellSettingsAlarmSettingsSnoozeWindow>(app, std::move(presenter));
            });
        windowsFactory.attach(
            gui::BellSettingsAlarmSettingsWindow::name, [this](ApplicationCommon *app, const std::string &) {
                auto alarmToneModel       = std::make_unique<bell_settings::AlarmToneModel>(this);
                auto alarmVolumeModel     = std::make_unique<bell_settings::AlarmVolumeModel>(*audioModel);
                auto alarmLightOnOffModel = std::make_unique<bell_settings::AlarmLightOnOffModel>(this);
                auto alarmSettingsModel   = std::make_unique<bell_settings::AlarmSettingsModel>(
                    std::move(alarmToneModel), std::move(alarmVolumeModel), std::move(alarmLightOnOffModel));
                auto soundsRepository = std::make_unique<SoundsRepository>(alarms::paths::getAlarmDir());
                auto provider         = std::make_shared<bell_settings::AlarmSettingsListItemProvider>(
                    *alarmSettingsModel, soundsRepository->getSongTitles());
                auto presenter = std::make_unique<bell_settings::AlarmSettingsPresenter>(
                    provider, std::move(alarmSettingsModel), *audioModel, std::move(soundsRepository));
                return std::make_unique<gui::BellSettingsAlarmSettingsWindow>(app, std::move(presenter));
            });

        windowsFactory.attach(
            gui::AboutYourBellWindow::name, [](ApplicationCommon *app, [[maybe_unused]] const std::string &name) {
                auto aboutYourBellModel = std::make_shared<bell_settings::AboutYourBellModel>(app);
                auto aboutYourBellPresenter =
                    std::make_unique<bell_settings::AboutYourBellWindowPresenter>(std::move(aboutYourBellModel));
                return std::make_unique<gui::AboutYourBellWindow>(app, std::move(aboutYourBellPresenter));
            });

        windowsFactory.attach(gui::window::name::bellSettingsFactoryReset,
                              [](ApplicationCommon *app, const std::string &name) {
                                  return std::make_unique<gui::BellDialogYesNo>(app, name);
                              });

        attachPopups({gui::popup::ID::AlarmActivated,
                      gui::popup::ID::AlarmDeactivated,
                      gui::popup::ID::PowerOff,
                      gui::popup::ID::Reboot,
                      gui::popup::ID::BedtimeNotification});
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

    void ApplicationBellSettings::onStop()
    {
        Application::onStop();
        audioModel->stopPlayedByThis({});
    }
} // namespace app
