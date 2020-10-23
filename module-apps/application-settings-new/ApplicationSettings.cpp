// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationSettings.hpp"

#include "windows/AddDeviceWindow.hpp"
#include "windows/AllDevicesWindow.hpp"
#include "windows/BluetoothWindow.hpp"
#include "windows/SettingsMainWindow.hpp"
#include "windows/DisplayAndKeypadWindow.hpp"
#include "windows/InputLanguageWindow.hpp"
#include "windows/LockedScreenWindow.hpp"
#include "windows/FontSizeWindow.hpp"
#include "windows/DisplayLightWindow.hpp"
#include "windows/KeypadLightWindow.hpp"
#include "windows/AppsAndToolsWindow.hpp"
#include "windows/NetworkWindow.hpp"
#include "windows/MessagesWindow.hpp"
#include "windows/PhoneNameWindow.hpp"
#include "windows/AutolockWindow.hpp"
#include "windows/WallpaperWindow.hpp"
#include "windows/QuotesMainWindow.hpp"
#include "windows/QuotesAddWindow.hpp"

#include "Dialog.hpp"

#include <service-evtmgr/api/EventManagerServiceAPI.hpp>
#include <service-bluetooth/messages/BluetoothMessage.hpp>
#include <i18/i18.hpp>

namespace app
{
    ApplicationSettingsNew::ApplicationSettingsNew(std::string name, std::string parent, bool startBackgound)
        : Application(name, parent, startBackgound)
    {}

    // Invoked upon receiving data message
    auto ApplicationSettingsNew::DataReceivedHandler(sys::DataMessage *msgl,
                                                     [[maybe_unused]] sys::ResponseMessage *resp) -> sys::Message_t
    {
        auto retMsg = Application::DataReceivedHandler(msgl);
        // if message was handled by application's template there is no need to process further.
        auto rm = dynamic_cast<sys::ResponseMessage *>(retMsg.get());
        if (nullptr != rm && sys::ReturnCodes::Success == rm->retCode) {
            return retMsg;
        }

        if (auto btMsg = dynamic_cast<BluetoothScanResultMessage *>(msgl); btMsg != nullptr) {
            auto scannedBtDevices = btMsg->devices;
            LOG_INFO("Received BT Scan message!");

            auto data = std::make_unique<gui::DeviceData>(scannedBtDevices);
            windowsFactory.build(this, gui::window::name::add_device);
            switchWindow(gui::window::name::add_device, gui::ShowMode::GUI_SHOW_INIT, std::move(data));
            render(gui::RefreshModes::GUI_REFRESH_FAST);
        }

        return std::make_shared<sys::ResponseMessage>();
    }

    // Invoked during initialization
    auto ApplicationSettingsNew::InitHandler() -> sys::ReturnCodes
    {
        board = EventManagerServiceAPI::GetBoard(this);

        auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success) {
            return ret;
        }

        createUserInterface();

        setActiveWindow(gui::name::window::main_window);

        return ret;
    }

    void ApplicationSettingsNew::createUserInterface()
    {
        windowsFactory.attach(gui::name::window::main_window, [](Application *app, const std::string &name) {
            return std::make_unique<gui::OptionWindow>(
                app, utils::localize.get("app_settings_title_main"), mainWindowOptionsNew(app));
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
            return std::make_unique<gui::KeypadLightWindow>(app);
        });
        windowsFactory.attach(gui::window::name::font_size, [](Application *app, const std::string &name) {
            return std::make_unique<gui::FontSizeWindow>(app);
        });
        windowsFactory.attach(gui::window::name::display_light, [](Application *app, const std::string &name) {
            return std::make_unique<gui::DisplayLightWindow>(app);
        });
        windowsFactory.attach(gui::window::name::apps_and_tools, [](Application *app, const std::string &name) {
            return std::make_unique<gui::AppsAndToolsWindow>(app);
        });
        windowsFactory.attach(gui::window::name::network, [](Application *app, const std::string &name) {
            return std::make_unique<gui::NetworkWindow>(app);
        });
        windowsFactory.attach(gui::window::name::messages, [](Application *app, const std::string &name) {
            return std::make_unique<gui::MessagesWindow>(app);
        });
        windowsFactory.attach(gui::window::name::phone_name, [](Application *app, const std::string &name) {
            return std::make_unique<gui::PhoneNameWindow>(app);
        });
        windowsFactory.attach(gui::window::name::autolock, [](Application *app, const std::string &name) {
            return std::make_unique<gui::AutolockWindow>(app);
        });
        windowsFactory.attach(gui::window::name::wallpaper, [](Application *app, const std::string &name) {
            return std::make_unique<gui::WallpaperWindow>(app);
        });
        windowsFactory.attach(gui::window::name::quotes, [](Application *app, const std::string &name) {
            return std::make_unique<gui::QuotesMainWindow>(app);
        });
        windowsFactory.attach(gui::window::name::new_quote, [](Application *app, const std::string &name) {
            return std::make_unique<gui::QuotesAddWindow>(app);
        });
    }

    void ApplicationSettingsNew::destroyUserInterface()
    {}
} /* namespace app */
