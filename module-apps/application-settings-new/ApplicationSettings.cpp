// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationSettings.hpp"

#include "windows/AddDeviceWindow.hpp"
#include "windows/AllDevicesWindow.hpp"
#include "windows/ApnSettingsWindow.hpp"
#include "windows/ApnOptionsWindow.hpp"
#include "windows/BluetoothWindow.hpp"
#include "windows/SettingsMainWindow.hpp"
#include "windows/DisplayAndKeypadWindow.hpp"
#include "windows/InputLanguageWindow.hpp"
#include "windows/LockedScreenWindow.hpp"
#include "windows/FontSizeWindow.hpp"
#include "windows/DisplayLightWindow.hpp"
#include "windows/KeypadLightWindow.hpp"
#include "windows/AppsAndToolsWindow.hpp"
#include "windows/NightshiftWindow.hpp"
#include "windows/NetworkWindow.hpp"
#include "windows/PhoneWindow.hpp"
#include "windows/CallRingtoneWindow.hpp"
#include "windows/MessagesWindow.hpp"
#include "windows/MessageSoundWindow.hpp"
#include "windows/CalendarWindow.hpp"
#include "windows/NotificationSoundWindow.hpp"
#include "windows/AlarmClockWindow.hpp"
#include "windows/PhoneNameWindow.hpp"
#include "windows/AutolockWindow.hpp"
#include "windows/TorchWindow.hpp"
#include "windows/WallpaperWindow.hpp"
#include "windows/QuotesMainWindow.hpp"
#include "windows/QuotesAddWindow.hpp"
#include "windows/EditQuotesWindow.hpp"
#include "windows/QuoteCategoriesWindow.hpp"
#include "windows/SecurityMainWindow.hpp"
#include "windows/QuotesOptionsWindow.hpp"
#include "windows/SARInfoWindow.hpp"
#include "windows/ChangePasscodeWindow.hpp"
#include "windows/SystemMainWindow.hpp"
#include "windows/NewApnWindow.hpp"
#include "windows/LanguagesWindow.hpp"
#include "windows/DateAndTimeMainWindow.hpp"
#include "windows/ChangeTimeZone.hpp"
#include "windows/ChangeDateAndTimeWindow.hpp"
#include "windows/PhoneModesWindow.hpp"
#include "windows/PINSettingsWindow.hpp"
#include "windows/DoNotDisturbWindow.hpp"
#include "windows/OfflineWindow.hpp"
#include "windows/ConnectionFrequencyWindow.hpp"
#include "windows/AboutYourPureWindow.hpp"
#include "windows/CertificationWindow.hpp"
#include "windows/TechnicalInformationWindow.hpp"

#include "Dialog.hpp"
#include "DialogMetadataMessage.hpp"

#include <service-evtmgr/EventManagerServiceAPI.hpp>
#include <service-cellular/CellularServiceAPI.hpp>
#include <service-bluetooth/BluetoothMessage.hpp>
#include <service-bluetooth/Constants.hpp>
#include <service-bluetooth/messages/Status.hpp>
#include <service-bluetooth/messages/BondedDevices.hpp>
#include <service-bluetooth/messages/Connect.hpp>
#include <service-bluetooth/messages/DeviceName.hpp>
#include <service-bluetooth/messages/InitializationResult.hpp>
#include <service-bluetooth/messages/Passkey.hpp>
#include <service-bluetooth/messages/ResponseVisibleDevices.hpp>
#include <service-bluetooth/messages/Unpair.hpp>
#include <service-db/agents/settings/SystemSettings.hpp>
#include <application-settings-new/data/ApnListData.hpp>
#include <application-settings-new/data/BondedDevicesData.hpp>
#include <application-settings-new/data/BluetoothStatusData.hpp>
#include <application-settings-new/data/DeviceData.hpp>
#include <application-settings-new/data/LanguagesData.hpp>
#include <application-settings-new/data/PhoneNameData.hpp>
#include <application-settings-new/data/PINSettingsLockStateData.hpp>
#include <module-services/service-db/agents/settings/SystemSettings.hpp>
#include <service-db/Settings.hpp>

#include <i18n/i18n.hpp>
#include <module-services/service-evtmgr/service-evtmgr/ScreenLightControlMessage.hpp>
#include <module-services/service-evtmgr/service-evtmgr/Constants.hpp>
#include <module-services/service-evtmgr/service-evtmgr/EVMessages.hpp>
#include <module-services/service-appmgr/service-appmgr/messages/Message.hpp>
#include <module-services/service-appmgr/service-appmgr/model/ApplicationManager.hpp>
#include <module-apps/application-desktop/windows/PinLockWindow.hpp>
#include <module-apps/application-desktop/windows/Names.hpp>

namespace app
{
    namespace settings
    {
        constexpr inline auto operators_on = "operators_on";
    } // namespace settings

    ApplicationSettingsNew::ApplicationSettingsNew(std::string name,
                                                   std::string parent,
                                                   sys::phone_modes::PhoneMode mode,
                                                   StartInBackground startInBackground)
        : Application(std::move(name), std::move(parent), mode, startInBackground), AsyncCallbackReceiver{this}
    {
        if ((Store::GSM::SIM::SIM1 == selectedSim || Store::GSM::SIM::SIM2 == selectedSim) &&
            Store::GSM::get()->sim == selectedSim) {
            selectedSimNumber = CellularServiceAPI::GetOwnNumber(this);
        }
    }

    ApplicationSettingsNew::~ApplicationSettingsNew()
    {}

    // Invoked upon receiving data message
    auto ApplicationSettingsNew::DataReceivedHandler(sys::DataMessage *msgl,
                                                     [[maybe_unused]] sys::ResponseMessage *resp) -> sys::MessagePointer
    {
        auto retMsg = Application::DataReceivedHandler(msgl);
        // if message was handled by application's template there is no need to process further.
        auto rm = dynamic_cast<sys::ResponseMessage *>(retMsg.get());
        if (nullptr != rm && sys::ReturnCodes::Success == rm->retCode) {
            return retMsg;
        }

        if (auto phoneMsg = dynamic_cast<CellularNotificationMessage *>(msgl); nullptr != phoneMsg) {
            selectedSim = Store::GSM::get()->selected;
            if (CellularNotificationMessage::Type::SIM_READY == phoneMsg->type) {
                selectedSimNumber = CellularServiceAPI::GetOwnNumber(this);
            }
            else if (CellularNotificationMessage::Type::SIM_NOT_READY == phoneMsg->type) {
                selectedSimNumber = {};
            }
            auto currentWindow = getCurrentWindow();
            if (gui::window::name::network == currentWindow->getName()) {
                currentWindow->rebuild();
            }
        }

        // handle database response
        if (resp != nullptr) {
            if (auto command = callbackStorage->getCallback(resp); command->execute()) {
                refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
            }
        }

        return sys::MessageNone{};
    }

    // Invoked during initialization
    auto ApplicationSettingsNew::InitHandler() -> sys::ReturnCodes
    {
        board = EventManagerServiceAPI::GetBoard(this);

        auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success) {
            return ret;
        }

        connect(typeid(::message::bluetooth::ResponseStatus), [&](sys::Message *msg) {
            auto responseStatusMsg = static_cast<::message::bluetooth::ResponseStatus *>(msg);
            if (gui::window::name::bluetooth == getCurrentWindow()->getName()) {
                const auto status = responseStatusMsg->getStatus();
                auto btStatusData = std::make_unique<gui::BluetoothStatusData>(status.state, status.visibility);
                switchWindow(gui::window::name::bluetooth, std::move(btStatusData));
            }
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

        connect(typeid(::message::bluetooth::ResponseBondedDevices), [&](sys::Message *msg) {
            auto responseBondedDevicesMsg = static_cast<::message::bluetooth::ResponseBondedDevices *>(msg);
            if (gui::window::name::all_devices == getCurrentWindow()->getName()) {
                auto bondedDevicesData = std::make_unique<gui::BondedDevicesData>(
                    responseBondedDevicesMsg->getDevices(), responseBondedDevicesMsg->getAddressOfConnectedDevice());
                switchWindow(gui::window::name::all_devices, std::move(bondedDevicesData));
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

        connect(typeid(BluetoothPairResultMessage), [&](sys::Message *msg) {
            auto bluetoothPairResultMsg = static_cast<BluetoothPairResultMessage *>(msg);
            if (bluetoothPairResultMsg->isSucceed()) {
                bus.sendUnicast(std::make_shared<::message::bluetooth::RequestBondedDevices>(),
                                service::name::bluetooth);
                return sys::MessageNone{};
            }
            auto addr = bluetoothPairResultMsg->getAddr();
            switchWindow(gui::window::name::dialog_retry,
                         gui::ShowMode::GUI_SHOW_INIT,
                         std::make_unique<gui::DialogMetadataMessage>(
                             gui::DialogMetadata{utils::localize.get("app_settings_bt"),
                                                 "info_big_circle_W_G",
                                                 utils::localize.get("app_settings_bluetooth_pairing_error_message"),
                                                 "",
                                                 [=]() -> bool {
                                                     bus.sendUnicast(std::make_shared<BluetoothPairMessage>(addr),
                                                                     service::name::bluetooth);

                                                     returnToPreviousWindow();
                                                     return true;
                                                 }}));

            return sys::MessageNone{};
        });

        connect(typeid(::message::bluetooth::RequestPasskey), [&](sys::Message *msg) {
            switchWindow(gui::window::name::bluetooth_check_passkey);
            return sys::MessageNone{};
        });

        connect(typeid(::message::bluetooth::UnpairResult), [&](sys::Message *msg) {
            auto unpairResultMsg = static_cast<::message::bluetooth::UnpairResult *>(msg);
            if (unpairResultMsg->isSucceed()) {
                return sys::MessageNone{};
            }
            auto addr = unpairResultMsg->getAddr();
            bus.sendUnicast(std::make_shared<::message::bluetooth::RequestBondedDevices>(), service::name::bluetooth);
            switchWindow(gui::window::name::dialog_retry,
                         gui::ShowMode::GUI_SHOW_INIT,
                         std::make_unique<gui::DialogMetadataMessage>(
                             gui::DialogMetadata{utils::localize.get("app_settings_bt"),
                                                 "info_big_circle_W_G",
                                                 utils::localize.get("app_settings_bluetooth_unpairing_error_message"),
                                                 "",
                                                 [=]() -> bool {
                                                     bus.sendUnicast(std::make_shared<message::bluetooth::Unpair>(addr),
                                                                     service::name::bluetooth);
                                                     returnToPreviousWindow();
                                                     return true;
                                                 }}));
            return sys::MessageNone{};
        });

        connect(typeid(::message::bluetooth::ConnectResult), [&](sys::Message *msg) {
            auto connectResultMsg = static_cast<::message::bluetooth::ConnectResult *>(msg);
            if (connectResultMsg->isSucceed()) {
                bus.sendUnicast(std::make_shared<::message::bluetooth::RequestBondedDevices>(),
                                service::name::bluetooth);
                return sys::MessageNone{};
            }
            auto addr = connectResultMsg->getAddr();
            switchWindow(gui::window::name::dialog_retry,
                         gui::ShowMode::GUI_SHOW_INIT,
                         std::make_unique<gui::DialogMetadataMessage>(gui::DialogMetadata{
                             utils::localize.get("app_settings_bt"),
                             "info_big_circle_W_G",
                             utils::localize.get("app_settings_bluetooth_connecting_error_message"),
                             "",
                             [=]() -> bool {
                                 bus.sendUnicast(std::make_shared<message::bluetooth::Connect>(addr),
                                                 service::name::bluetooth);
                                 returnToPreviousWindow();
                                 return true;
                             }}));

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
                             gui::DialogMetadata{utils::localize.get("app_settings_bt"),
                                                 "info_big_circle_W_G",
                                                 utils::localize.get("app_settings_bluetooth_init_error_message"),
                                                 "",
                                                 [=]() -> bool {
                                                     switchWindow(gui::window::name::bluetooth);
                                                     return true;
                                                 }}));

            return sys::MessageNone{};
        });

        connect(typeid(CellularGetAPNResponse), [&](sys::Message *msg) {
            if (gui::window::name::apn_settings == getCurrentWindow()->getName()) {
                auto apns = dynamic_cast<CellularGetAPNResponse *>(msg);
                if (apns != nullptr) {
                    auto apnsData = std::make_unique<gui::ApnListData>(apns->getAPNs());
                    switchWindow(gui::window::name::apn_settings, std::move(apnsData));
                }
            }
            return sys::MessageNone{};
        });

        connect(typeid(CellularSimCardPinLockStateResponseDataMessage), [&](sys::Message *msg) {
            auto simCardPinLockState = dynamic_cast<CellularSimCardPinLockStateResponseDataMessage *>(msg);
            if (simCardPinLockState != nullptr) {
                auto pinSettingsLockStateData =
                    std::make_unique<gui::PINSettingsLockStateData>(simCardPinLockState->getSimCardPinLockState());
                updateWindow(gui::window::name::pin_settings, std::move(pinSettingsLockStateData));
            }
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

        createUserInterface();

        setActiveWindow(gui::name::window::main_window);

        settings->registerValueChange(settings::operators_on,
                                      [this](const std::string &value) { operatorOnChanged(value); });
        settings->registerValueChange(
            ::settings::Cellular::volte_on,
            [this](const std::string &value) { volteChanged(value); },
            ::settings::SettingsScope::Global);
        settings->registerValueChange(
            ::settings::SystemProperties::lockPassHash,
            [this](const std::string &value) { lockPassHash = utils::getNumericValue<unsigned int>(value); },
            ::settings::SettingsScope::Global);

        /*
        settings->registerValueChange(
            ::settings::SystemProperties::usbSecurity,
            [this](std::string value) { usbSecured = utils::getNumericValue<bool>(value); },
            ::settings::SettingsScope::Global);
        */
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
        settings->registerValueChange(
            ::settings::Connected::messageSoundOn,
            [this](const std::string &value) { connectedMessageSoundOn = utils::getNumericValue<bool>(value); },
            ::settings::SettingsScope::Global);
        settings->registerValueChange(
            ::settings::Connected::messageVibrationOn,
            [this](const std::string &value) { connectedMessageVibrationOn = utils::getNumericValue<bool>(value); },
            ::settings::SettingsScope::Global);

        return ret;
    }

    void ApplicationSettingsNew::createUserInterface()
    {
        windowsFactory.attach(gui::name::window::main_window, [](Application *app, const std::string &name) {
            return std::make_unique<gui::OptionWindow>(
                app, utils::localize.get("app_settings_title_main_new"), mainWindowOptionsNew(app));
        });
        windowsFactory.attach(gui::window::name::bluetooth, [](Application *app, const std::string &name) {
            return std::make_unique<gui::BluetoothWindow>(app);
        });
        windowsFactory.attach(gui::window::name::add_device, [](Application *app, const std::string &name) {
            return std::make_unique<gui::AddDeviceWindow>(app);
        });
        windowsFactory.attach(gui::window::name::all_devices, [](Application *app, const std::string &name) {
            return std::make_unique<gui::AllDevicesWindow>(app);
        });
        windowsFactory.attach(gui::window::name::dialog_settings, [](Application *app, const std::string &name) {
            return std::make_unique<gui::Dialog>(app, name);
        });
        windowsFactory.attach(gui::window::name::display_and_keypad, [](Application *app, const std::string &name) {
            return std::make_unique<gui::DisplayAndKeypadWindow>(app);
        });
        windowsFactory.attach(gui::window::name::input_language, [](Application *app, const std::string &name) {
            return std::make_unique<gui::InputLanguageWindow>(app);
        });
        windowsFactory.attach(gui::window::name::locked_screen, [](Application *app, const std::string &name) {
            return std::make_unique<gui::LockedScreenWindow>(app);
        });
        windowsFactory.attach(gui::window::name::keypad_light, [](Application *app, const std::string &name) {
            return std::make_unique<gui::KeypadLightWindow>(app, static_cast<ApplicationSettingsNew *>(app));
        });
        windowsFactory.attach(gui::window::name::font_size, [](Application *app, const std::string &name) {
            return std::make_unique<gui::FontSizeWindow>(app);
        });
        windowsFactory.attach(gui::window::name::display_light, [](Application *app, const std::string &name) {
            return std::make_unique<gui::DisplayLightWindow>(app, static_cast<ApplicationSettingsNew *>(app));
        });
        windowsFactory.attach(gui::window::name::apps_and_tools, [](Application *app, const std::string &name) {
            return std::make_unique<gui::AppsAndToolsWindow>(app);
        });
        windowsFactory.attach(gui::window::name::nightshift, [](Application *app, const std::string &name) {
            return std::make_unique<gui::NightshiftWindow>(app);
        });
        windowsFactory.attach(gui::window::name::network, [](Application *app, const std::string &name) {
            return std::make_unique<gui::NetworkWindow>(
                app, static_cast<ApplicationSettingsNew *>(app), static_cast<ApplicationSettingsNew *>(app));
        });
        windowsFactory.attach(gui::window::name::apn_settings, [](Application *app, const std::string &name) {
            return std::make_unique<gui::ApnSettingsWindow>(app);
        });
        windowsFactory.attach(gui::window::name::apn_options, [](Application *app, const std::string &name) {
            return std::make_unique<gui::ApnOptionsWindow>(app);
        });
        windowsFactory.attach(gui::window::name::phone, [](Application *app, const std::string &name) {
            return std::make_unique<gui::PhoneWindow>(app);
        });
        windowsFactory.attach(gui::window::name::call_ringtone, [](Application *app, const std::string &name) {
            return std::make_unique<gui::CallRingtoneWindow>(app);
        });
        windowsFactory.attach(gui::window::name::messages, [](Application *app, const std::string &name) {
            return std::make_unique<gui::MessagesWindow>(app, static_cast<ApplicationSettingsNew *>(app));
        });
        windowsFactory.attach(gui::window::name::message_sound, [](Application *app, const std::string &name) {
            return std::make_unique<gui::MessageSoundWindow>(app);
        });
        windowsFactory.attach(gui::window::name::calendar, [](Application *app, const std::string &name) {
            return std::make_unique<gui::CalendarWindow>(app);
        });
        windowsFactory.attach(gui::window::name::notification_sound, [](Application *app, const std::string &name) {
            return std::make_unique<gui::NotificationSoundWindow>(app);
        });
        windowsFactory.attach(gui::window::name::alarm_clock, [](Application *app, const std::string &name) {
            return std::make_unique<gui::AlarmClockWindow>(app);
        });
        windowsFactory.attach(gui::window::name::phone_name, [](Application *app, const std::string &name) {
            return std::make_unique<gui::PhoneNameWindow>(app);
        });
        windowsFactory.attach(gui::window::name::autolock, [](Application *app, const std::string &name) {
            return std::make_unique<gui::AutolockWindow>(app);
        });
        windowsFactory.attach(gui::window::name::torch, [](Application *app, const std::string &name) {
            return std::make_unique<gui::TorchWindow>(app);
        });
        windowsFactory.attach(gui::window::name::wallpaper, [](Application *app, const std::string &name) {
            return std::make_unique<gui::WallpaperWindow>(app);
        });
        windowsFactory.attach(gui::window::name::quotes_dialog_yes_no, [](Application *app, const std::string &name) {
            return std::make_unique<gui::DialogYesNo>(app, name);
        });
        windowsFactory.attach(gui::window::name::security, [](Application *app, const std::string &name) {
            return std::make_unique<gui::SecurityMainWindow>(app, static_cast<ApplicationSettingsNew *>(app));
        });
        windowsFactory.attach(app::window::name::desktop_pin_lock, [&](Application *app, const std::string newname) {
            return std::make_unique<gui::PinLockWindow>(app, app::window::name::desktop_pin_lock);
        });
        windowsFactory.attach(gui::window::name::change_passcode, [](Application *app, const std::string &name) {
            return std::make_unique<gui::ChangePasscodeWindow>(app);
        });
        windowsFactory.attach(gui::window::name::dialog_confirm, [](Application *app, const std::string &name) {
            return std::make_unique<gui::DialogConfirm>(app, gui::window::name::dialog_confirm);
        });
        windowsFactory.attach(gui::window::name::system, [](Application *app, const std::string &name) {
            return std::make_unique<gui::SystemMainWindow>(app);
        });
        windowsFactory.attach(gui::window::name::pin_settings, [](Application *app, const std::string &name) {
            return std::make_unique<gui::PINSettingsWindow>(app);
        });
        windowsFactory.attach(gui::window::name::new_apn, [](Application *app, const std::string &name) {
            return std::make_unique<gui::NewApnWindow>(app);
        });
        windowsFactory.attach(gui::window::name::languages, [](Application *app, const std::string &name) {
            return std::make_unique<gui::LanguagesWindow>(app);
        });
        windowsFactory.attach(gui::window::name::date_and_time, [](Application *app, const std::string &name) {
            return std::make_unique<gui::DateAndTimeMainWindow>(app);
        });
        windowsFactory.attach(gui::window::name::about_your_pure, [](Application *app, const std::string &name) {
            return std::make_unique<gui::AboutYourPureWindow>(app);
        });
        windowsFactory.attach(gui::window::name::factory_reset, [](Application *app, const std::string &name) {
            return std::make_unique<gui::DialogYesNo>(app, name);
        });
        windowsFactory.attach(gui::window::name::certification, [](Application *app, const std::string &name) {
            return std::make_unique<gui::CertificationWindow>(app);
        });
        windowsFactory.attach(gui::window::name::technical_information, [](Application *app, const std::string &name) {
            return std::make_unique<gui::TechnicalInformationWindow>(app);
        });
        windowsFactory.attach(gui::window::name::sar, [&](Application *app, const std::string &name) {
            auto sarInfoRepository = std::make_unique<SARInfoRepository>("assets/certification_info", "sar.txt");
            auto presenter         = std::make_unique<SARInfoWindowPresenter>(std::move(sarInfoRepository));
            return std::make_unique<gui::SARInfoWindow>(app, std::move(presenter));
        });
        windowsFactory.attach(gui::window::name::change_time_zone, [](Application *app, const std::string &name) {
            return std::make_unique<gui::ChangeTimeZone>(app);
        });
        windowsFactory.attach(gui::window::name::change_date_and_time, [](Application *app, const std::string &name) {
            return std::make_unique<gui::ChangeDateAndTimeWindow>(app);
        });
        windowsFactory.attach(gui::window::name::dialog_retry, [](Application *app, const std::string &name) {
            return std::make_unique<gui::DialogRetry>(app, name);
        });
        windowsFactory.attach(gui::window::name::quotes, [](Application *app, const std::string &name) {
            return std::make_unique<gui::QuotesMainWindow>(app);
        });
        windowsFactory.attach(gui::window::name::new_quote, [](Application *app, const std::string &name) {
            return std::make_unique<gui::QuoteAddEditWindow>(app);
        });
        windowsFactory.attach(gui::window::name::options_quote, [](Application *app, const std::string &name) {
            return std::make_unique<gui::QuotesOptionsWindow>(app);
        });
        windowsFactory.attach(gui::window::name::edit_quotes, [](Application *app, const std::string &name) {
            return std::make_unique<gui::EditQuotesWindow>(app);
        });
        windowsFactory.attach(gui::window::name::quote_categories, [](Application *app, const std::string &name) {
            return std::make_unique<gui::QuoteCategoriesWindow>(app);
        });
        windowsFactory.attach(gui::window::name::phone_modes, [](Application *app, const std::string &name) {
            return std::make_unique<gui::PhoneModesWindow>(
                app, static_cast<ApplicationSettingsNew *>(app), static_cast<ApplicationSettingsNew *>(app));
        });
        windowsFactory.attach(gui::window::name::do_not_disturb, [](Application *app, const std::string &name) {
            return std::make_unique<gui::DoNotDisturbWindow>(app, static_cast<ApplicationSettingsNew *>(app));
        });
        windowsFactory.attach(gui::window::name::offline, [](Application *app, const std::string &name) {
            return std::make_unique<gui::OfflineWindow>(app, static_cast<ApplicationSettingsNew *>(app));
        });
        windowsFactory.attach(gui::window::name::connection_frequency, [](Application *app, const std::string &name) {
            return std::make_unique<gui::ConnectionFrequencyWindow>(app, static_cast<ApplicationSettingsNew *>(app));
        });
        attachPopups({gui::popup::ID::Volume,
                      gui::popup::ID::TetheringPhoneModeChangeProhibited,
                      gui::popup::ID::Tethering,
                      gui::popup::ID::PhoneModes});
    }

    void ApplicationSettingsNew::destroyUserInterface()
    {}

    std::string ApplicationSettingsNew::getNumber()
    {
        return selectedSimNumber;
    }

    void ApplicationSettingsNew::setSim(Store::GSM::SIM sim)
    {
        CellularServiceAPI::SetSimCard(this, sim);
    }

    Store::GSM::SIM ApplicationSettingsNew::getSim()
    {
        return selectedSim;
    }

    void ApplicationSettingsNew::operatorOnChanged(const std::string &value)
    {
        LOG_DEBUG("[ApplicationSettingsNew::operatorOnChanged] value=%s", value.c_str());
        if (!value.empty()) {
            operatorsOn = utils::getNumericValue<bool>(value);
        }
    }
    bool ApplicationSettingsNew::getOperatorsOn() const noexcept
    {
        LOG_DEBUG("[ApplicationSettingsNew::getOperatorsOn] %d", operatorsOn);
        return operatorsOn;
    }
    void ApplicationSettingsNew::setOperatorsOn(bool value)
    {
        operatorsOn = value;
        LOG_DEBUG("[ApplicationSettingsNew::setOperatorsOn] to %d", operatorsOn);
        settings->setValue(settings::operators_on, std::to_string(static_cast<int>(value)));
    }

    void ApplicationSettingsNew::setVoLTEOn(bool value)
    {
        voLteStateOn = value;
        CellularServiceAPI::SetVoLTE(this, voLteStateOn);
    };

    bool ApplicationSettingsNew::getVoLTEOn() const noexcept
    {
        return voLteStateOn;
    }

    void ApplicationSettingsNew::volteChanged(const std::string &value)
    {
        if (!value.empty()) {
            voLteStateOn = utils::getNumericValue<bool>(value);
        }
    }

    void ApplicationSettingsNew::setOsUpdateVersion(const std::string &value)
    {
        LOG_DEBUG("[ApplicationSettingsNew::setOsUpdateVersion] to value=%s", value.c_str());
        settings->setValue(::settings::SystemProperties::osUpdateVersion, value, ::settings::SettingsScope::Global);
    }

    void ApplicationSettingsNew::setLockPassHash(unsigned int value)
    {
        lockPassHash = value;
        settings->setValue(
            ::settings::SystemProperties::lockPassHash, std::to_string(value), ::settings::SettingsScope::Global);
    }

    auto ApplicationSettingsNew::getCurrentValues() -> settingsInterface::ScreenLightSettings::Values
    {
        constexpr int timeout = pdMS_TO_TICKS(1500);

        auto response = bus.sendUnicast(
            std::make_shared<sevm::ScreenLightControlRequestParameters>(), service::name::evt_manager, timeout);

        if (response.first == sys::ReturnCodes::Success) {
            auto msgState = dynamic_cast<sevm::ScreenLightControlParametersResponse *>(response.second.get());
            if (msgState == nullptr) {
                return {};
            }

            return {msgState->lightOn, msgState->mode, msgState->parameters};
        }

        return {};
    }

    void ApplicationSettingsNew::setBrightness(bsp::eink_frontlight::BrightnessPercentage value)
    {
        screen_light_control::Parameters parameters{value};
        bus.sendUnicast(std::make_shared<sevm::ScreenLightControlMessage>(
                            screen_light_control::Action::setManualModeBrightness, parameters),
                        service::name::evt_manager);
    }

    void ApplicationSettingsNew::setMode(bool isAutoLightSwitchOn)
    {
        bus.sendUnicast(std::make_shared<sevm::ScreenLightControlMessage>(
                            isAutoLightSwitchOn ? screen_light_control::Action::enableAutomaticMode
                                                : screen_light_control::Action::disableAutomaticMode),
                        service::name::evt_manager);
    }

    void ApplicationSettingsNew::setStatus(bool isDisplayLightSwitchOn)
    {
        bus.sendUnicast(std::make_shared<sevm::ScreenLightControlMessage>(isDisplayLightSwitchOn
                                                                              ? screen_light_control::Action::turnOn
                                                                              : screen_light_control::Action::turnOff),
                        service::name::evt_manager);
    }
    void ApplicationSettingsNew::setBrightnessFunction()
    {
        screen_light_control::Parameters parameters;
        parameters.functionPoints =
            screen_light_control::functions::BrightnessFunction({{0.0f, 0.0f}, {1000.0f, 100.0f}});
        bus.sendUnicast(std::make_shared<sevm::ScreenLightControlMessage>(
                            screen_light_control::Action::setAutomaticModeParameters, parameters),
                        service::name::evt_manager);
    }

    auto ApplicationSettingsNew::isKeypadBacklightOn() -> bool
    {
        constexpr int timeout = pdMS_TO_TICKS(1500);

        auto response =
            bus.sendUnicast(std::make_shared<sevm::KeypadBacklightMessage>(bsp::keypad_backlight::Action::checkState),
                            service::name::evt_manager,
                            timeout);

        if (response.first == sys::ReturnCodes::Success) {
            auto msgState = dynamic_cast<sevm::KeypadBacklightResponseMessage *>(response.second.get());
            if (msgState == nullptr) {
                return false;
            }

            return msgState->success;
        }
        return false;
    }

    void ApplicationSettingsNew::setKeypadBacklightState(bool newState)
    {
        bus.sendUnicast(std::make_shared<sevm::KeypadBacklightMessage>(
                            newState ? bsp::keypad_backlight::Action::turnOn : bsp::keypad_backlight::Action::turnOff),
                        service::name::evt_manager);
    }

    bool ApplicationSettingsNew::isUSBSecured() const
    {
        return usbSecured;
    }

    void ApplicationSettingsNew::setUSBSecurity(bool security)
    {
        usbSecured = security;
        settings->setValue(
            ::settings::SystemProperties::usbSecurity, std::to_string(security), ::settings::SettingsScope::Global);
    }

    auto ApplicationSettingsNew::getNotificationsWhenLocked() const noexcept -> bool
    {
        return notificationsWhenLocked;
    }
    void ApplicationSettingsNew::setNotificationsWhenLocked(bool on) noexcept
    {
        notificationsWhenLocked = on;
        settings->setValue(
            ::settings::Offline::notificationsWhenLocked, std::to_string(on), ::settings::SettingsScope::Global);
    }
    auto ApplicationSettingsNew::getCallsFromFavourite() const noexcept -> bool
    {
        return callsFromFavorites;
    }
    void ApplicationSettingsNew::setCallsFromFavourite(bool on) noexcept
    {
        callsFromFavorites = on;
        settings->setValue(
            ::settings::Offline::callsFromFavorites, std::to_string(on), ::settings::SettingsScope::Global);
    }

    auto ApplicationSettingsNew::isFlightMode() const noexcept -> bool
    {
        return flightModeOn;
    }

    void ApplicationSettingsNew::setFlightMode(bool flightModeOn) noexcept
    {
        this->flightModeOn = flightModeOn;
        CellularServiceAPI::SetFlightMode(this, flightModeOn);
    }

    auto ApplicationSettingsNew::isMessageSoundEnabled() const noexcept -> bool
    {
        return connectedMessageSoundOn;
    }

    void ApplicationSettingsNew::setMessageSoundEnabled(bool messageSoundOn) noexcept
    {
        connectedMessageSoundOn = messageSoundOn;
        settings->setValue(
            ::settings::Connected::messageSoundOn, std::to_string(messageSoundOn), ::settings::SettingsScope::Global);
    }

    auto ApplicationSettingsNew::isMessageVibrationEnabled() const noexcept -> bool
    {
        return connectedMessageVibrationOn;
    }

    void ApplicationSettingsNew::setMessageVibrationEnabled(bool messageVibrationOn) noexcept
    {
        connectedMessageVibrationOn = messageVibrationOn;
        settings->setValue(::settings::Connected::messageVibrationOn,
                           std::to_string(messageVibrationOn),
                           ::settings::SettingsScope::Global);
    }

    auto ApplicationSettingsNew::getConnectionFrequency() const noexcept -> uint8_t
    {
        return connectionFrequency;
    }

    void ApplicationSettingsNew::setConnectionFrequency(uint8_t val) noexcept
    {
        connectionFrequency = val;
        settings->setValue(
            ::settings::Offline::connectionFrequency, std::to_string(val), ::settings::SettingsScope::Global);
    }

} /* namespace app */
