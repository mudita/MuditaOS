// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Application.hpp"

#include "MessageType.hpp"
#include "application-settings/windows/EinkModeWindow.hpp"
#include "windows/BtScanWindow.hpp"
#include "windows/BtWindow.hpp"
#include "windows/DateTimeWindow.hpp"
#include "windows/FotaWindow.hpp"
#include "windows/Info.hpp"
#include "windows/LanguageWindow.hpp"
#include "windows/SettingsMainWindow.hpp"
#include "windows/ColorTestWindow.hpp"

#include "windows/UITestWindow.hpp"

#include "windows/TestMessageWindow.hpp"

#include "ApplicationSettings.hpp"

#include "service-cellular/ServiceCellular.hpp"
#include <service-cellular/CellularServiceAPI.hpp>
#include "windows/SettingsMainWindow.hpp"
#include "windows/SimSelectWindow.hpp"
#include "windows/CellularPassthroughWindow.hpp"
#include "OptionChangePin.hpp"

#include <i18n/i18n.hpp>
#include <service-evtmgr/EventManagerServiceAPI.hpp>
#include <service-bluetooth/BluetoothMessage.hpp>
#include <service-db/Settings.hpp>
#include <module-services/service-db/agents/settings/SystemSettings.hpp>

namespace gui::window::name
{
    inline constexpr auto date_time = "DateTime";
}

namespace app
{
    ApplicationSettings::ApplicationSettings(std::string name, std::string parent, StartInBackground startInBackground)
        : Application(name, parent, startInBackground)
    {
        bus.channels.push_back(sys::BusChannel::AntennaNotifications);
        addActionReceiver(manager::actions::SelectSimCard, [this](auto &&data) {
            switchWindow(app::sim_select);
            return actionHandled();
        });
    }

    ApplicationSettings::~ApplicationSettings()
    {}

    // Invoked upon receiving data message
    sys::MessagePointer ApplicationSettings::DataReceivedHandler(sys::DataMessage *msgl,
                                                                 sys::ResponseMessage * /*resp*/)
    {

        auto retMsg = Application::DataReceivedHandler(msgl);
        // if message was handled by application's template there is no need to process further.
        if (reinterpret_cast<sys::ResponseMessage *>(retMsg.get())->retCode == sys::ReturnCodes::Success) {
            return retMsg;
        }
        if (auto btmsg = dynamic_cast<BluetoothScanResultMessage *>(msgl); btmsg != nullptr) {
            auto devices = btmsg->devices;
            LOG_INFO("received BT Scan message!");
            auto data = std::make_unique<gui::DeviceData>(devices);
            windowsFactory.build(this, gui::name::window::name_btscan);
            switchWindow(gui::name::window::name_btscan, gui::ShowMode::GUI_SHOW_INIT, std::move(data));

            render(gui::RefreshModes::GUI_REFRESH_FAST);
        }
        if (auto btmsg = dynamic_cast<BluetoothPairResultMessage *>(msgl); btmsg != nullptr) {
            if (btmsg->isSucceed()) {
                LOG_INFO("Paired successfully");
            }
            else {
                LOG_ERROR("Pairing error!");
            }
        }
        // this variable defines whether message was processed.
        bool handled = true;

        if (handled)
            return std::make_shared<sys::ResponseMessage>();
        else
            return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Unresolved);
    }

    // Invoked during initialization
    sys::ReturnCodes ApplicationSettings::InitHandler()
    {
        board = EventManagerServiceAPI::GetBoard(this);

        auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success)
            return ret;

        settings->registerValueChange(
            settings::SystemProperties::lockPassHash,
            [this](std::string value) { lockPassChanged(value); },
            settings::SettingsScope::Global);

        createUserInterface();

        setActiveWindow(gui::name::window::main_window);

        return ret;
    }

    sys::ReturnCodes ApplicationSettings::DeinitHandler()
    {
        return sys::ReturnCodes::Success;
    }

    void ApplicationSettings::createUserInterface()
    {
        windowsFactory.attach(gui::name::window::main_window, [](Application *app, const std::string &name) {
            return std::make_unique<gui::OptionWindow>(
                app, utils::localize.get("app_settings_title_main"), mainWindowOptions(app));
        });

        windowsFactory.attach(app::sim_select, [this](Application *app, const std::string &name) {
            return std::make_unique<gui::OptionWindow>(app, name, simSelectWindow(app, this));
        });
        windowsFactory.attach(app::change_setting, [this](Application *app, const std::string &name) {
            return std::make_unique<gui::OptionWindow>(app, name, settingChangePinWindow(app, this, lockPassHash));
        });
        windowsFactory.attach("Languages", [](Application *app, const std::string &name) {
            return std::make_unique<gui::LanguageWindow>(app);
        });
        windowsFactory.attach("Bluetooth", [](Application *app, const std::string &name) {
            return std::make_unique<gui::BtWindow>(app);
        });
        windowsFactory.attach(gui::name::window::name_btscan, [](Application *app, const std::string &name) {
            return std::make_unique<gui::BtScanWindow>(app);
        });
        windowsFactory.attach("TEST_UI", [](Application *app, const std::string &name) {
            return std::make_unique<gui::UiTestWindow>(app);
        });
        windowsFactory.attach(gui::window::hw_info, [](Application *app, const std::string &name) {
            return std::make_unique<gui::Info>(app);
        });
        windowsFactory.attach(gui::window::name::date_time, [this](Application *app, const std::string &name) {
            return std::make_unique<gui::DateTimeWindow>(app, europeanDateTimeFormat);
        });
        windowsFactory.attach(gui::window::name::fota_window, [](Application *app, const std::string &name) {
            return std::make_unique<gui::FotaWindow>(app);
        });
        windowsFactory.attach(gui::window::name::eink, [](Application *app, const std::string &name) {
            return std::make_unique<gui::EinkModeWindow>(app);
        });
        windowsFactory.attach(gui::window::name::color_test_window, [](Application *app, const std::string &name) {
            return std::make_unique<gui::ColorTestWindow>(app);
        });

        if (board == bsp::Board::T4) {
            windowsFactory.attach(gui::window::cellular_passthrough::window_name,
                                  [](Application *app, const std::string &name) {
                                      return std::make_unique<gui::CellularPassthroughWindow>(app);
                                  });
        }

        attachPopups({gui::popup::ID::Volume});
    }

    void ApplicationSettings::destroyUserInterface()
    {}

    void ApplicationSettings::setSim(Store::GSM::SIM sim)
    {
        CellularServiceAPI::SetSimCard(this, sim);
    }

    void ApplicationSettings::setPin(unsigned int value)
    {
        settings->setValue(
            settings::SystemProperties::lockPassHash, std::to_string(value), settings::SettingsScope::Global);
        lockPassHash = value;
    }

    void ApplicationSettings::clearPin()
    {
        settings->setValue(settings::SystemProperties::lockPassHash, "", settings::SettingsScope::Global);
        lockPassHash = 0U;
    }

    void ApplicationSettings::lockPassChanged(std::string value)
    {
        auto newLockPassHash = 0U;
        if (!value.empty()) {
            newLockPassHash = utils::getNumericValue<unsigned int>(value);
        }
        if (lockPassHash != newLockPassHash) {
            lockPassHash       = newLockPassHash;
            auto currentWindow = getCurrentWindow();
            if (app::change_setting == currentWindow->getName()) {
                currentWindow->rebuild();
            }
        }
    }

    void ApplicationSettings::timeDateChanged(std::string value)
    {
        auto newTimeDateFormat = utils::getNumericValue<bool>(value);
        if (newTimeDateFormat != europeanDateTimeFormat) {
            europeanDateTimeFormat = newTimeDateFormat;
            auto currentWindow     = getCurrentWindow();
            if (gui::window::name::date_time == currentWindow->getName()) {
                currentWindow->rebuild();
            }
        }
    }

} /* namespace app */
