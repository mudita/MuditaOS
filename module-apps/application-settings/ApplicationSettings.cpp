﻿// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <application-settings/ApplicationSettings.hpp>

#include <application-settings/windows/SettingsMainWindow.hpp>
#include <application-settings/windows/advanced/AdvancedOptionsWindow.hpp>
#include <application-settings/windows/advanced/InformationWindow.hpp>
#include <application-settings/windows/advanced/TextImageColorWindow.hpp>
#include <application-settings/windows/advanced/UITestWindow.hpp>
#include <application-settings/windows/advanced/CPUModeTestWindow.hpp>
#include <application-settings/windows/advanced/ColorTestWindow.hpp>
#include <application-settings/windows/advanced/StatusBarImageTypeWindow.hpp>
#include <application-settings/windows/bluetooth/BluetoothWindow.hpp>
#include <application-settings/windows/bluetooth/AddDeviceWindow.hpp>
#include <application-settings/windows/bluetooth/AllDevicesWindow.hpp>
#include <application-settings/windows/bluetooth/PhoneNameWindow.hpp>
#include <application-settings/windows/network/NetworkWindow.hpp>
#include <application-settings/windows/network/SimPINSettingsWindow.hpp>
#include <application-settings/windows/network/SimCardsWindow.hpp>
#include <application-settings/windows/network/SimContactsImportWindow.hpp>
#include <application-settings/windows/network/NewApnWindow.hpp>
#include <application-settings/windows/network/ApnSettingsWindow.hpp>
#include <application-settings/windows/network/ApnOptionsWindow.hpp>
#include <application-settings/windows/display-keypad/DisplayAndKeypadWindow.hpp>
#include <application-settings/windows/display-keypad/DisplayLightWindow.hpp>
#include <application-settings/windows/display-keypad/WallpaperWindow.hpp>
#include <application-settings/windows/display-keypad/QuotesMainWindow.hpp>
#include <application-settings/windows/display-keypad/QuotesAddWindow.hpp>
#include <application-settings/windows/display-keypad/EditQuotesWindow.hpp>
#include <application-settings/windows/display-keypad/QuoteCategoriesWindow.hpp>
#include <application-settings/windows/display-keypad/QuotesOptionsWindow.hpp>
#include <application-settings/windows/display-keypad/KeypadLightWindow.hpp>
#include <application-settings/windows/display-keypad/InputLanguageWindow.hpp>
#include <application-settings/windows/phone-modes/PhoneModesWindow.hpp>
#include <application-settings/windows/phone-modes/DoNotDisturbWindow.hpp>
#include <application-settings/windows/phone-modes/OfflineWindow.hpp>
#include <application-settings/windows/phone-modes/ConnectionFrequencyWindow.hpp>
#include <application-settings/windows/apps/AppsWindow.hpp>
#include <application-settings/windows/apps/PhoneWindow.hpp>
#include <application-settings/windows/apps/MessagesWindow.hpp>
#include <application-settings/windows/apps/AlarmClockWindow.hpp>
#include <application-settings/windows/apps/SoundSelectWindow.hpp>
#include <application-settings/windows/security/SecurityMainWindow.hpp>
#include <application-settings/windows/security/AutolockWindow.hpp>
#include <application-settings/windows/system/SystemMainWindow.hpp>
#include <application-settings/windows/system/LanguagesWindow.hpp>
#include <application-settings/windows/system/DateAndTimeMainWindow.hpp>
#include <application-settings/windows/system/ChangeTimeZone.hpp>
#include <application-settings/windows/system/ChangeDateAndTimeWindow.hpp>
#include <application-settings/windows/system/TechnicalInformationWindow.hpp>
#include <application-settings/windows/system/CertificationWindow.hpp>
#include <application-settings/windows/system/SARInfoWindow.hpp>
#include <application-settings/windows/system/FactoryResetInProgressWindow.hpp>
#include <application-settings/data/ApnListData.hpp>
#include <application-settings/data/BondedDevicesData.hpp>
#include <application-settings/data/BluetoothStatusData.hpp>
#include <application-settings/data/DeviceData.hpp>
#include <application-settings/data/LanguagesData.hpp>
#include <application-settings/data/PhoneNameData.hpp>
#include <application-settings/data/PINSettingsLockStateData.hpp>
#include <application-settings/data/AutoLockData.hpp>
#include <application-settings/models/apps/SoundsModel.hpp>
#include <application-settings/models/display-keypad/DisplayModeModel.hpp>
#include <module-services/service-cellular/service-cellular/VolteState.hpp>
#include <service-evtmgr/EventManagerServiceAPI.hpp>
#include <service-audio/AudioServiceAPI.hpp>
#include <service-bluetooth/BluetoothMessage.hpp>
#include <service-bluetooth/Constants.hpp>
#include <service-bluetooth/ServiceBluetoothName.hpp>
#include <service-bluetooth/messages/BondedDevices.hpp>
#include <service-bluetooth/messages/BluetoothModeChanged.hpp>
#include <service-bluetooth/messages/Connect.hpp>
#include <service-bluetooth/messages/DeviceName.hpp>
#include <service-bluetooth/messages/InitializationResult.hpp>
#include <service-bluetooth/messages/ResponseVisibleDevices.hpp>
#include <service-bluetooth/messages/Status.hpp>
#include <service-bluetooth/messages/Unpair.hpp>
#include <service-bluetooth/messages/Disconnect.hpp>
#include <service-db/Settings.hpp>
#include <service-db/agents/settings/SystemSettings.hpp>
#include <module-services/service-appmgr/include/service-appmgr/ServiceApplicationManagerName.hpp>
#include <module-services/service-evtmgr/service-evtmgr/ScreenLightControlMessage.hpp>
#include <module-services/service-evtmgr/service-evtmgr/ServiceEventManagerName.hpp>
#include <module-services/service-evtmgr/service-evtmgr/EVMessages.hpp>
#include <service-appmgr/messages/Message.hpp>
#include <service-appmgr/model/ApplicationManagerCommon.hpp>
#include <apps-common/WindowsStack.hpp>
#include <apps-common/messages/DialogMetadataMessage.hpp>
#include <apps-common/windows/Dialog.hpp>
#include <apps-common/popups/lock-popups/SimInfoWindow.hpp>

#include <i18n/i18n.hpp>
#include <service-bluetooth/messages/SyncDevices.hpp>

namespace
{
    constexpr auto applicationSettingsStackSize = 1024 * 6;
}

namespace app
{
    namespace settings
    {
        constexpr inline auto operators_on = "operators_on";
    } // namespace settings

    ApplicationSettings::ApplicationSettings(std::string name,
                                             std::string parent,
                                             StatusIndicators statusIndicators,
                                             StartInBackground startInBackground)
        : Application(
              std::move(name), std::move(parent), statusIndicators, startInBackground, applicationSettingsStackSize),
          AsyncCallbackReceiver{this}, soundsPlayer{std::make_shared<SoundsPlayer>(this)}
    {
        bus.channels.push_back(sys::BusChannel::ServiceAudioNotifications);
        bus.channels.push_back(sys::BusChannel::BluetoothNotifications);
        bus.channels.push_back(sys::BusChannel::BluetoothModeChanges);
        bus.channels.push_back(sys::BusChannel::ServiceCellularNotifications);

        CellularServiceAPI::SubscribeForOwnNumber(this, [&](const std::string &number) {
            selectedSimNumber = number;
            LOG_DEBUG("Sim number changed");
        });
        if ((Store::GSM::SIM::SIM1 == simState || Store::GSM::SIM::SIM2 == simState) &&
            Store::GSM::get()->sim == simState) {
            CellularServiceAPI::RequestForOwnNumber(this);
        }
        bluetoothSettingsModel = std::make_shared<BluetoothSettingsModel>(this);
    }

    ApplicationSettings::~ApplicationSettings()
    {}

    // Invoked upon receiving data message
    auto ApplicationSettings::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
        -> sys::MessagePointer
    {
        auto retMsg = Application::DataReceivedHandler(msgl);
        // if message was handled by application's template there is no need to process further.
        auto rm = dynamic_cast<sys::ResponseMessage *>(retMsg.get());
        if (nullptr != rm && sys::ReturnCodes::Success == rm->retCode) {
            return retMsg;
        }

        if (auto phoneMsg = dynamic_cast<cellular::NotificationMessage *>(msgl); nullptr != phoneMsg) {
            auto currentWindow = getCurrentWindow();
            if (gui::window::name::network == currentWindow->getName()) {
                updateCurrentWindow();
            }
        }

        return handleAsyncResponse(resp);
    }

    // Invoked during initialization
    auto ApplicationSettings::InitHandler() -> sys::ReturnCodes
    {
        auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success) {
            return ret;
        }
        connect(typeid(cellular::msg::notification::SimReady), [&](sys::Message *) {
            CellularServiceAPI::RequestForOwnNumber(this);
            return handleSimNotification();
        });

        connect(typeid(cellular::msg::notification::SimBlocked),
                [&](sys::Message *) { return handleSimNotification(); });

        connect(typeid(cellular::msg::notification::SimNeedPin),
                [&](sys::Message *) { return handleSimNotification(); });

        connect(typeid(cellular::msg::notification::SimNeedPuk),
                [&](sys::Message *) { return handleSimNotification(); });

        connect(typeid(::sys::bluetooth::BluetoothModeChanged), [&](sys::Message *msg) {
            const auto message   = static_cast<::sys::bluetooth::BluetoothModeChanged *>(msg);
            const auto newState  = message->getBluetoothMode();
            const auto oldStatus = bluetoothSettingsModel->getStatus();
            const auto oldState  = oldStatus.state;
            if (newState == sys::bluetooth::BluetoothMode::Disabled && oldState == BluetoothStatus::State::On) {
                const auto isVisible = oldStatus.visibility;
                bluetoothSettingsModel->updateStatus(false, isVisible);
                if (getCurrentWindow()->getName() == gui::window::name::bluetooth) {
                    updateCurrentWindow();
                }
            }
            return sys::MessageNone{};
        });

        connect(typeid(::message::bluetooth::ResponseStatus), [&](sys::Message *msg) {
            auto responseStatusMsg    = static_cast<::message::bluetooth::ResponseStatus *>(msg);
            const auto status         = responseStatusMsg->getStatus();
            const auto bluetoothState = status.state == BluetoothStatus::State::On;
            bluetoothSettingsModel->setStatus(bluetoothState, status.visibility);
            return sys::MessageNone{};
        });

        connect(typeid(::message::bluetooth::ResponseDeviceName), [&](sys::Message *msg) {
            auto responseDeviceNameMsg = static_cast<::message::bluetooth::ResponseDeviceName *>(msg);
            if (gui::window::name::phone_name == getCurrentWindow()->getName()) {
                auto phoneNameData = std::make_unique<gui::PhoneNameData>(responseDeviceNameMsg->getName());
                switchWindow(gui::window::name::phone_name, std::move(phoneNameData));
            }
            return sys::MessageNone{};
        });

        connect(typeid(::message::bluetooth::SyncDevices), [&](sys::Message *msg) {
            auto message = static_cast<::message::bluetooth::SyncDevices *>(msg);
            bluetoothSettingsModel->replaceDevicesList(message->getDevices());

            if (gui::window::name::all_devices == getCurrentWindow()->getName()) {
                switchWindow(gui::window::name::all_devices);
            }
            return sys::MessageNone{};
        });
        connect(typeid(::message::bluetooth::ResponseBondedDevices), [&](sys::Message *msg) {
            auto message = static_cast<::message::bluetooth::ResponseBondedDevices *>(msg);
            bluetoothSettingsModel->replaceDevicesList(message->getDevices());

            if (gui::window::name::all_devices == getCurrentWindow()->getName()) {
                switchWindow(gui::window::name::all_devices);
            }
            return sys::MessageNone{};
        });

        connect(typeid(::message::bluetooth::ResponseVisibleDevices), [&](sys::Message *msg) {
            auto responseVisibleDevicesMsg = static_cast<::message::bluetooth::ResponseVisibleDevices *>(msg);
            if (gui::window::name::add_device == getCurrentWindow()->getName() ||
                gui::window::name::dialog_settings == getCurrentWindow()->getName()) {
                auto visibleDevicesData = std::make_unique<gui::DeviceData>(responseVisibleDevicesMsg->getDevices());
                if (gui::window::name::dialog_settings == getCurrentWindow()->getName()) {
                    visibleDevicesData->ignoreCurrentWindowOnStack = true;
                }
                switchWindow(gui::window::name::add_device, std::move(visibleDevicesData));
            }
            return sys::MessageNone{};
        });

        connect(typeid(::message::bluetooth::ProfileStatus), [&](sys::Message *msg) { return sys::MessageNone{}; });

        connect(typeid(::message::bluetooth::UnpairResult), [&](sys::Message *msg) {
            auto unpairResultMsg = static_cast<::message::bluetooth::UnpairResult *>(msg);
            if (unpairResultMsg->isSucceed()) {
                return sys::MessageNone{};
            }
            auto device  = unpairResultMsg->getDevice();
            auto message = std::make_shared<message::bluetooth::Unpair>(device);
            switchToAllDevicesViaBtErrorPrompt(std::move(message), "app_settings_bluetooth_unpairing_error_message");
            return sys::MessageNone{};
        });

        connect(typeid(::message::bluetooth::ConnectResult), [&](sys::Message *msg) {
            auto connectResultMsg = static_cast<::message::bluetooth::ConnectResult *>(msg);
            auto device           = connectResultMsg->getDevice();

            using message::bluetooth::ConnectResult;
            if (connectResultMsg->getResult() == ConnectResult::Result::Success) {
                return sys::MessageNone{};
            }

            auto message = std::make_shared<message::bluetooth::Connect>(device);
            switchToAllDevicesViaBtErrorPrompt(std::move(message), "app_settings_bluetooth_connecting_error_message");
            return sys::MessageNone{};
        });
        connect(typeid(::message::bluetooth::DisconnectResult), [&](sys::Message *msg) {
            if (gui::window::name::all_devices == getCurrentWindow()->getName()) {
                switchWindow(gui::window::name::all_devices);
            }
            return sys::MessageNone{};
        });
        connect(typeid(::message::bluetooth::InitializationResult), [&](sys::Message *msg) {
            auto initializationResultMsg = static_cast<::message::bluetooth::InitializationResult *>(msg);
            if (initializationResultMsg->isSucceed()) {
                return sys::MessageNone{};
            }
            switchWindow(gui::window::name::dialog_confirm,
                         gui::ShowMode::GUI_SHOW_INIT,
                         std::make_unique<gui::DialogMetadataMessage>(
                             gui::DialogMetadata{utils::translate("app_settings_bt"),
                                                 "fail_128px_W_G",
                                                 utils::translate("app_settings_bluetooth_init_error_message"),
                                                 "",
                                                 [=]() -> bool {
                                                     switchWindow(gui::window::name::bluetooth);
                                                     return true;
                                                 }}));

            return sys::MessageNone{};
        });

        connect(typeid(cellular::GetAPNResponse), [&](sys::Message *msg) {
            if (gui::window::name::apn_settings == getCurrentWindow()->getName()) {
                auto apns = dynamic_cast<cellular::GetAPNResponse *>(msg);
                if (apns != nullptr) {
                    auto apnsData = std::make_unique<gui::ApnListData>(apns->getAPNs());
                    switchWindow(gui::window::name::apn_settings, std::move(apnsData));
                }
            }
            return sys::MessageNone{};
        });

        connect(typeid(cellular::msg::request::sim::GetPinSettings::Response), [&](sys::Message *msg) {
            auto simCardPinSettings     = static_cast<cellular::msg::request::sim::GetPinSettings::Response *>(msg);
            auto simCardPinSettingsData = std::make_unique<gui::PINSettingsLockStateData>(simCardPinSettings->locked);
            updateCurrentWindow(std::move(simCardPinSettingsData));
            return sys::MessageNone{};
        });

        connect(typeid(manager::GetCurrentDisplayLanguageResponse), [&](sys::Message *msg) {
            if (gui::window::name::languages == getCurrentWindow()->getName()) {
                auto response = dynamic_cast<manager::GetCurrentDisplayLanguageResponse *>(msg);
                if (response != nullptr) {
                    auto languagesData = std::make_unique<LanguagesData>(response->getLanguage());
                    switchWindow(gui::window::name::languages, std::move(languagesData));
                }
            }
            return sys::MessageNone{};
        });

        connect(typeid(AudioStopNotification), [&](sys::Message *msg) -> sys::MessagePointer {
            auto notification = static_cast<AudioStopNotification *>(msg);
            return handleAudioStop(notification);
        });

        connect(typeid(AudioEOFNotification), [&](sys::Message *msg) -> sys::MessagePointer {
            auto notification = static_cast<AudioStopNotification *>(msg);
            return handleAudioStop(notification);
        });

        connect(typeid(cellular::GetVolteStateResponse),
                [&](sys::Message *msg) -> sys::MessagePointer { return handleVolteState(msg); });

        connect(typeid(cellular::VolteStateNotification),
                [&](sys::Message *msg) -> sys::MessagePointer { return handleVolteState(msg); });

        addActionReceiver(app::manager::actions::DisplayFactoryResetInProgressScreen, [this](auto &&data) {
            requestShutdownWindow(gui::window::name::factory_reset_in_progress);
            return actionHandled();
        });

        createUserInterface();

        settings->registerValueChange(settings::operators_on,
                                      [this](const std::string &value) { operatorOnChanged(value); });
        settings->registerValueChange(
            ::settings::Cellular::offlineMode,
            [this](const std::string &value) { flightModeOn = utils::getNumericValue<bool>(value); },
            ::settings::SettingsScope::Global);

        settings->registerValueChange(
            ::settings::Offline::callsFromFavorites,
            [this](const std::string &value) { callsFromFavorites = utils::getNumericValue<bool>(value); },
            ::settings::SettingsScope::Global);
        settings->registerValueChange(
            ::settings::Offline::connectionFrequency,
            [this](const std::string &value) { utils::toNumeric(value, connectionFrequency); },
            ::settings::SettingsScope::Global);
        settings->registerValueChange(
            ::settings::Offline::notificationsWhenLocked,
            [this](const std::string &value) { notificationsWhenLocked = utils::getNumericValue<bool>(value); },
            ::settings::SettingsScope::Global);

        bluetoothSettingsModel->requestBondedDevices();
        bluetoothSettingsModel->requestStatus();

        bus.sendUnicast(std::make_shared<cellular::GetVolteStateRequest>(), service::name::cellular);

        return ret;
    }

    void ApplicationSettings::createUserInterface()
    {
        windowsFactory.attach(gui::name::window::main_window, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::OptionWindow>(
                app, name, mainWindowOptionsNew(app), utils::translate("app_settings_title_main"));
        });

        // Advanced
        windowsFactory.attach(gui::window::name::advanced, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::OptionWindow>(
                app, name, advancedOptions(app), utils::translate("app_settings_advanced"));
        });
        windowsFactory.attach(gui::window::name::information, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::InformationWindow>(app);
        });
        windowsFactory.attach(gui::window::name::ui_test, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::UiTestWindow>(app);
        });
        windowsFactory.attach(gui::window::name::cpu_test_window, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::CPUModeTestWindow>(app);
        });
        windowsFactory.attach(gui::window::name::color_test_window,
                              [](ApplicationCommon *app, const std::string &name) {
                                  return std::make_unique<gui::ColorTestWindow>(app);
                              });
        windowsFactory.attach(gui::window::name::status_bar_img_type,
                              [](ApplicationCommon *app, const std::string &name) {
                                  return std::make_unique<gui::StatusBarImageTypeWindow>(app);
                              });
        windowsFactory.attach(gui::window::name::text_image_color, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::TextImageColorWindow>(app);
        });
        // Bluetooth
        windowsFactory.attach(gui::window::name::bluetooth, [this](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::BluetoothWindow>(app, bluetoothSettingsModel);
        });
        windowsFactory.attach(gui::window::name::add_device, [this](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::AddDeviceWindow>(app, bluetoothSettingsModel);
        });
        windowsFactory.attach(gui::window::name::all_devices, [this](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::AllDevicesWindow>(app, bluetoothSettingsModel);
        });
        windowsFactory.attach(gui::window::name::phone_name, [this](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::PhoneNameWindow>(app, bluetoothSettingsModel);
        });

        // Network
        windowsFactory.attach(gui::window::name::network, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::NetworkWindow>(app, static_cast<ApplicationSettings *>(app));
        });
        windowsFactory.attach(gui::window::name::sim_cards, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::SimCardsWindow>(app, static_cast<ApplicationSettings *>(app));
        });
        windowsFactory.attach(gui::window::name::sim_pin_settings,
                              [&](ApplicationCommon *app, const std::string &name) {
                                  auto presenter = std::make_unique<SimPINSettingsPresenter>(this);
                                  return std::make_unique<gui::SimPINSettingsWindow>(app, std::move(presenter));
                              });
        windowsFactory.attach(gui::window::name::import_contacts, [&](ApplicationCommon *app, const std::string &name) {
            auto repository = std::make_unique<SimContactsRepository>(this);
            auto model      = std::make_unique<SimContactsImportModel>(this, std::move(repository));
            auto presenter  = std::make_unique<SimContactsImportWindowPresenter>(this, std::move(model));
            return std::make_unique<gui::SimContactsImportWindow>(app, std::move(presenter));
        });

        windowsFactory.attach(gui::window::name::new_apn, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::NewApnWindow>(app);
        });
        windowsFactory.attach(gui::window::name::apn_settings, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::ApnSettingsWindow>(app);
        });
        windowsFactory.attach(gui::window::name::apn_options, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::ApnOptionsWindow>(app);
        });

        // Display and keypad
        windowsFactory.attach(gui::window::name::display_and_keypad,
                              [](ApplicationCommon *app, const std::string &name) {
                                  auto model = std::make_unique<display_mode::DisplayModeModel>(app);
                                  return std::make_unique<gui::DisplayAndKeypadWindow>(app, std::move(model));
                              });
        windowsFactory.attach(gui::window::name::display_light, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::DisplayLightWindow>(app, static_cast<ApplicationSettings *>(app));
        });
        windowsFactory.attach(gui::window::name::wallpaper, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::WallpaperWindow>(app, static_cast<ApplicationSettings *>(app));
        });
        windowsFactory.attach(gui::window::name::quotes, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::QuotesMainWindow>(app);
        });
        windowsFactory.attach(gui::window::name::new_quote, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::QuoteAddEditWindow>(app);
        });
        windowsFactory.attach(gui::window::name::options_quote, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::QuotesOptionsWindow>(app);
        });
        windowsFactory.attach(gui::window::name::edit_quotes, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::EditQuotesWindow>(app);
        });
        windowsFactory.attach(gui::window::name::quote_categories, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::QuoteCategoriesWindow>(app);
        });
        windowsFactory.attach(gui::window::name::quotes_dialog_yes_no,
                              [](ApplicationCommon *app, const std::string &name) {
                                  return std::make_unique<gui::DialogYesNo>(app, name);
                              });
        windowsFactory.attach(gui::window::name::keypad_light, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::KeypadLightWindow>(app, static_cast<ApplicationSettings *>(app));
        });
        windowsFactory.attach(gui::window::name::input_language, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::InputLanguageWindow>(app);
        });

        // Phone modes
        windowsFactory.attach(gui::window::name::phone_modes, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::PhoneModesWindow>(app, static_cast<ApplicationSettings *>(app));
        });
        windowsFactory.attach(gui::window::name::do_not_disturb, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::DoNotDisturbWindow>(app, static_cast<ApplicationSettings *>(app));
        });
        windowsFactory.attach(gui::window::name::offline, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::OfflineWindow>(app, static_cast<ApplicationSettings *>(app));
        });
        windowsFactory.attach(
            gui::window::name::connection_frequency, [](ApplicationCommon *app, const std::string &name) {
                return std::make_unique<gui::ConnectionFrequencyWindow>(app, static_cast<ApplicationSettings *>(app));
            });

        // Apps
        windowsFactory.attach(gui::window::name::apps, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::AppsWindow>(app);
        });
        windowsFactory.attach(gui::window::name::phone, [](ApplicationCommon *app, const std::string &name) {
            auto audioModel =
                std::make_unique<audio_settings::AudioSettingsModel>(app, audio_settings::PlaybackType::CallRingtone);
            return std::make_unique<gui::PhoneWindow>(app, std::move(audioModel));
        });
        windowsFactory.attach(gui::window::name::messages, [](ApplicationCommon *app, const std::string &name) {
            auto audioModel = std::make_unique<audio_settings::AudioSettingsModel>(
                app, audio_settings::PlaybackType::TextMessageRingtone);
            return std::make_unique<gui::MessagesWindow>(app, std::move(audioModel));
        });
        windowsFactory.attach(gui::window::name::alarm_clock, [](ApplicationCommon *app, const std::string &name) {
            auto audioModel =
                std::make_unique<audio_settings::AudioSettingsModel>(app, audio_settings::PlaybackType::Alarm);
            return std::make_unique<gui::AlarmClockWindow>(app, std::move(audioModel));
        });
        windowsFactory.attach(gui::window::name::sound_select, [this](ApplicationCommon *app, const std::string &name) {
            auto model = std::make_shared<SoundsModel>(soundsPlayer);
            return std::make_unique<gui::SoundSelectWindow>(app, name, std::move(model));
        });

        // Security
        windowsFactory.attach(gui::window::name::security, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::SecurityMainWindow>(app);
        });
        windowsFactory.attach(gui::window::name::autolock, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::AutolockWindow>(app, static_cast<ApplicationSettings *>(app));
        });

        // System
        windowsFactory.attach(gui::window::name::system, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::SystemMainWindow>(app);
        });
        windowsFactory.attach(gui::window::name::languages, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::LanguagesWindow>(app);
        });
        windowsFactory.attach(gui::window::name::date_and_time, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::DateAndTimeMainWindow>(app, gui::window::name::date_and_time);
        });
        windowsFactory.attach(gui::window::name::change_time_zone, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::ChangeTimeZone>(app);
        });
        windowsFactory.attach(gui::window::name::change_date_and_time,
                              [](ApplicationCommon *app, const std::string &name) {
                                  return std::make_unique<gui::ChangeDateAndTimeWindow>(app);
                              });
        windowsFactory.attach(
            gui::window::name::technical_information, [&](ApplicationCommon *app, const std::string &name) {
                auto factoryData = std::make_unique<FactoryData>(*settings);
                auto repository  = std::make_unique<TechnicalInformationRepository>(this);
                auto model = std::make_unique<TechnicalInformationModel>(std::move(factoryData), std::move(repository));
                auto presenter = std::make_unique<TechnicalWindowPresenter>(this, std::move(model));
                return std::make_unique<gui::TechnicalInformationWindow>(app, std::move(presenter));
            });
        windowsFactory.attach(gui::window::name::certification, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::CertificationWindow>(app);
        });
        windowsFactory.attach(gui::window::name::sar, [&](ApplicationCommon *app, const std::string &name) {
            const auto path        = purefs::dir::getSystemDataDirPath() / "certification_info";
            const auto filename    = "sar.txt";
            auto sarInfoRepository = std::make_unique<SARInfoRepository>(path, filename);
            auto presenter         = std::make_unique<SARInfoWindowPresenter>(std::move(sarInfoRepository));
            return std::make_unique<gui::SARInfoWindow>(app, std::move(presenter));
        });
        windowsFactory.attach(gui::window::name::factory_reset, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::DialogYesNo>(app, name);
        });
        windowsFactory.attach(gui::window::name::dialog_settings, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::Dialog>(app, name);
        });
        windowsFactory.attach(gui::window::name::dialog_confirm, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::DialogConfirm>(app, gui::window::name::dialog_confirm);
        });
        windowsFactory.attach(gui::window::name::dialog_retry, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::DialogRetry>(app, name);
        });
        windowsFactory.attach(gui::window::name::factory_reset_in_progress,
                              [](ApplicationCommon *app, const std::string &name) {
                                  return std::make_unique<gui::FactoryResetInProgressWindow>(app);
                              });

        attachPopups({gui::popup::ID::Volume,
                      gui::popup::ID::Tethering,
                      gui::popup::ID::BluetoothAuthenticate,
                      gui::popup::ID::PhoneModes,
                      gui::popup::ID::PhoneLock,
                      gui::popup::ID::SimLock,
                      gui::popup::ID::Alarm});
    }

    void ApplicationSettings::destroyUserInterface()
    {}

    std::string ApplicationSettings::getNumber()
    {
        return selectedSimNumber;
    }

    void ApplicationSettings::returnToPreviousWindow()
    {
        using namespace gui::popup::window;
        using namespace gui::window::name;

        if (!windowsStack().isWindowOnStack(sim_cards)) {
            return ApplicationCommon::returnToPreviousWindow();
        }

        auto previousWindowName = getPreviousWindow();
        if (!previousWindowName.has_value()) {
            return ApplicationCommon::returnToPreviousWindow();
        }

        auto backToSimCards = [this]() { switchWindow(sim_cards); };

        if (previousWindowName == sim_pin_settings || previousWindowName == import_contacts ||
            previousWindowName == sim_unlock_window || previousWindowName == sim_info_window) {
            return backToSimCards();
        }

        if (getCurrentWindow()->getName() == sim_info_window) {
            return backToSimCards();
        }

        ApplicationCommon::returnToPreviousWindow();
    }

    void ApplicationSettings::setSelectedSim(Store::GSM::SelectedSIM sim)
    {
        auto arg = (sim == Store::GSM::SelectedSIM::SIM2) ? cellular::api::SimSlot::SIM2 : cellular::api::SimSlot::SIM1;
        getSimLockSubject().setSim(arg);
    }

    void ApplicationSettings::updateSimParam()
    {
        selectedSim = Store::GSM::get()->selected;
        simState    = Store::GSM::get()->sim;
    }

    Store::GSM::SelectedSIM ApplicationSettings::getSelectedSim()
    {
        return selectedSim;
    }

    Store::GSM::SIM ApplicationSettings::getSimState()
    {
        return simState;
    }

    void ApplicationSettings::operatorOnChanged(const std::string &value)
    {
        LOG_DEBUG("OperatorOn changed: %s", value.c_str());
        if (!value.empty()) {
            operatorsOn = utils::getNumericValue<bool>(value);
        }
    }
    bool ApplicationSettings::getOperatorsOn() const noexcept
    {
        LOG_DEBUG("OperatorOn: %d", operatorsOn);
        return operatorsOn;
    }
    void ApplicationSettings::setOperatorsOn(bool value)
    {
        operatorsOn = value;
        LOG_DEBUG("New OperatorOn: %d", operatorsOn);
        settings->setValue(settings::operators_on, std::to_string(static_cast<int>(value)));
    }

    void ApplicationSettings::setOsUpdateVersion(const std::string &value)
    {
        LOG_DEBUG("New OS version: %s", value.c_str());
        settings->setValue(::settings::SystemProperties::osUpdateVersion, value, ::settings::SettingsScope::Global);
    }

    auto ApplicationSettings::getCurrentValues() -> settingsInterface::ScreenLightSettings::Values
    {
        constexpr int timeout = pdMS_TO_TICKS(1500);

        auto response = bus.sendUnicastSync(
            std::make_shared<sevm::ScreenLightControlRequestParameters>(), service::name::evt_manager, timeout);

        if (response.first == sys::ReturnCodes::Success) {
            auto msgState = dynamic_cast<sevm::ScreenLightControlParametersResponse *>(response.second.get());
            if (msgState == nullptr) {
                return {};
            }

            return {msgState->isLightOn(), msgState->getMode(), msgState->getParams()};
        }

        return {};
    }

    void ApplicationSettings::setBrightness(bsp::eink_frontlight::BrightnessPercentage value)
    {
        screen_light_control::ManualModeParameters parameters{value};
        bus.sendUnicast(std::make_shared<sevm::ScreenLightSetManualModeParams>(parameters), service::name::evt_manager);
    }

    void ApplicationSettings::setMode(bool isAutoLightSwitchOn)
    {
        bus.sendUnicast(std::make_shared<sevm::ScreenLightControlMessage>(
                            isAutoLightSwitchOn ? screen_light_control::Action::enableAutomaticMode
                                                : screen_light_control::Action::disableAutomaticMode),
                        service::name::evt_manager);
    }

    void ApplicationSettings::setStatus(bool isDisplayLightSwitchOn)
    {
        bus.sendUnicast(std::make_shared<sevm::ScreenLightControlMessage>(isDisplayLightSwitchOn
                                                                              ? screen_light_control::Action::turnOn
                                                                              : screen_light_control::Action::turnOff),
                        service::name::evt_manager);
    }

    auto ApplicationSettings::getKeypadBacklightState() -> bsp::keypad_backlight::State
    {
        return static_cast<bsp::keypad_backlight::State>(utils::getNumericValue<int>(
            settings->getValue(::settings::KeypadLight::state, ::settings::SettingsScope::Global)));
    }

    void ApplicationSettings::setKeypadBacklightState(bsp::keypad_backlight::State keypadLightState)
    {
        settings->setValue(::settings::KeypadLight::state,
                           std::to_string(static_cast<int>(keypadLightState)),
                           ::settings::SettingsScope::Global);
    }

    auto ApplicationSettings::getWallpaperOption() -> gui::WallpaperOption
    {
        return static_cast<gui::WallpaperOption>(utils::getNumericValue<int>(
            settings->getValue(::settings::Wallpaper::option, ::settings::SettingsScope::Global)));
    }

    void ApplicationSettings::setWallpaperOption(gui::WallpaperOption option)
    {
        settings->setValue(
            ::settings::Wallpaper::option, std::to_string(static_cast<int>(option)), ::settings::SettingsScope::Global);
    }

    auto ApplicationSettings::getNotificationsWhenLocked() const noexcept -> bool
    {
        return notificationsWhenLocked;
    }
    void ApplicationSettings::setNotificationsWhenLocked(bool on) noexcept
    {
        notificationsWhenLocked = on;
        settings->setValue(
            ::settings::Offline::notificationsWhenLocked, std::to_string(on), ::settings::SettingsScope::Global);
    }

    auto ApplicationSettings::getCallsFromFavourite() const noexcept -> bool
    {
        return callsFromFavorites;
    }
    void ApplicationSettings::setCallsFromFavourite(bool on) noexcept
    {
        callsFromFavorites = on;
        settings->setValue(
            ::settings::Offline::callsFromFavorites, std::to_string(on), ::settings::SettingsScope::Global);
    }

    auto ApplicationSettings::isFlightMode() const noexcept -> bool
    {
        return flightModeOn;
    }

    void ApplicationSettings::setFlightMode(bool flightModeOn) noexcept
    {
        this->flightModeOn = flightModeOn;
        CellularServiceAPI::SetFlightMode(this, flightModeOn);
    }

    auto ApplicationSettings::getConnectionFrequency() const noexcept -> uint8_t
    {
        return connectionFrequency;
    }

    void ApplicationSettings::setConnectionFrequency(uint8_t val) noexcept
    {
        connectionFrequency = val;
        CellularServiceAPI::SetConnectionFrequency(this, val);
    }

    auto ApplicationSettings::getAutoLockTime() -> std::chrono::seconds
    {
        auto intervalValue =
            settings->getValue(::settings::SystemProperties::autoLockTimeInSec, ::settings::SettingsScope::Global);
        return std::chrono::seconds{utils::getNumericValue<unsigned int>(intervalValue)};
    }

    auto ApplicationSettings::getCurrentPhoneMode() const noexcept -> sys::phone_modes::PhoneMode
    {
        return statusIndicators.phoneMode;
    }

    auto ApplicationSettings::getVolteState() const noexcept -> cellular::VolteState
    {
        return volteState;
    }

    void ApplicationSettings::setAutoLockTime(std::chrono::seconds lockTime)
    {
        bus.sendUnicast(std::make_shared<app::manager::SetAutoLockTimeoutRequest>(lockTime), service::name::appmgr);
    }

    void ApplicationSettings::switchToAllDevicesViaBtErrorPrompt(std::shared_ptr<sys::DataMessage> msg,
                                                                 const std::string &errorMsg)
    {
        switchWindow(gui::window::name::dialog_retry,
                     gui::ShowMode::GUI_SHOW_INIT,
                     std::make_unique<gui::DialogMetadataMessage>(
                         gui::DialogMetadata{utils::translate("app_settings_bt"),
                                             "fail_128px_W_G",
                                             utils::translate(errorMsg),
                                             "",
                                             [this, message{std::move(msg)}]() -> bool {
                                                 bus.sendUnicast(message, service::name::bluetooth);
                                                 switchWindow(gui::window::name::all_devices);
                                                 return true;
                                             }}));
    }

    auto ApplicationSettings::handleAudioStop(AudioStopNotification *notification) -> sys::MessagePointer
    {
        soundsPlayer->stop(notification->token);
        return sys::MessageNone{};
    }

    auto ApplicationSettings::handleSimNotification() -> sys::MessagePointer
    {
        auto currentWindow = getCurrentWindow();
        if (gui::window::name::sim_cards == currentWindow->getName()) {
            updateCurrentWindow();
        }
        return sys::MessageNone{};
    }

    auto ApplicationSettings::handleVolteState(sys::Message *msg) -> sys::MessagePointer
    {
        auto *volteStateMessage = static_cast<cellular::GetVolteStateResponse *>(msg);
        volteState              = volteStateMessage->volteState;

        if (isCurrentWindow(gui::window::name::network)) {
            // refreshWindow() would be insufficient because wouldn't trigger list rebuild
            switchWindow(gui::window::name::network);
        }

        return sys::MessageNone{};
    }

    void ApplicationSettings::sendVolteChangeRequest(bool enable)
    {
        auto message = std::make_shared<cellular::SwitchVolteOnOffRequest>(enable);
        bus.sendUnicast(std::move(message), service::name::cellular);
    };
} /* namespace app */
