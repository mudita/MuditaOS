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
#include "windows/NightshiftWindow.hpp"
#include "windows/NetworkWindow.hpp"
#include "windows/MessagesWindow.hpp"
#include "windows/PhoneNameWindow.hpp"
#include "windows/AutolockWindow.hpp"
#include "windows/TorchWindow.hpp"
#include "windows/WallpaperWindow.hpp"
#include "windows/QuotesMainWindow.hpp"
#include "windows/QuotesAddWindow.hpp"
#include "windows/SecurityMainWindow.hpp"

#include "Dialog.hpp"

#include <service-evtmgr/EventManagerServiceAPI.hpp>
#include <service-bluetooth/BluetoothMessage.hpp>
#include <service-cellular/CellularServiceAPI.hpp>
#include <service-db/Settings.hpp>
#include <module-services/service-bluetooth/service-bluetooth/messages/Status.hpp>

namespace app
{
    namespace settings
    {
        constexpr inline auto operators_on = "operators_on";
    }

    ApplicationSettingsNew::ApplicationSettingsNew(std::string name,
                                                   std::string parent,
                                                   StartInBackground startInBackground)
        : Application(name, parent, startInBackground)
    {
        if ((Store::GSM::SIM::SIM1 == selectedSim || Store::GSM::SIM::SIM2 == selectedSim) &&
            Store::GSM::get()->sim == selectedSim) {
            selectedSimNumber = CellularServiceAPI::GetOwnNumber(this);
        }
        settings->registerValueChange(settings::operators_on,
                                      [this](const std::string &value) { operatorOnChanged(value); });
    }

    ApplicationSettingsNew::~ApplicationSettingsNew()
    {
        settings->unregisterValueChange(settings::operators_on);
    }

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

        if (auto btMsg = dynamic_cast<BluetoothScanResultMessage *>(msgl); btMsg != nullptr) {
            auto scannedBtDevices = btMsg->devices;
            LOG_INFO("Received BT Scan message!");

            auto data = std::make_unique<gui::DeviceData>(scannedBtDevices);
            windowsFactory.build(this, gui::window::name::add_device);
            switchWindow(gui::window::name::add_device, gui::ShowMode::GUI_SHOW_INIT, std::move(data));
            render(gui::RefreshModes::GUI_REFRESH_FAST);
        }
        else if (auto phoneMsg = dynamic_cast<CellularNotificationMessage *>(msgl); nullptr != phoneMsg) {
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
        else if (auto responseStatusMsg = dynamic_cast<message::bluetooth::ResponseStatus *>(msgl);
                 nullptr != responseStatusMsg) {
            if (gui::window::name::bluetooth == getCurrentWindow()->getName()) {
                auto btStatusData = std::make_unique<gui::BluetoothStatusData>(responseStatusMsg->getStatus());
                switchWindow(gui::window::name::bluetooth, std::move(btStatusData));
            }
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
        windowsFactory.attach(gui::window::name::nightshift, [](Application *app, const std::string &name) {
            return std::make_unique<gui::NightshiftWindow>(app);
        });
        windowsFactory.attach(gui::window::name::network, [](Application *app, const std::string &name) {
            return std::make_unique<gui::NetworkWindow>(
                app, static_cast<ApplicationSettingsNew *>(app), static_cast<ApplicationSettingsNew *>(app));
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
        windowsFactory.attach(gui::window::name::torch, [](Application *app, const std::string &name) {
            return std::make_unique<gui::TorchWindow>(app);
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
        windowsFactory.attach(gui::window::name::security, [](Application *app, const std::string &name) {
            return std::make_unique<gui::SecurityMainWindow>(app);
        });
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
        if (!value.empty()) {
            operatorsOn = utils::getNumericValue<bool>(value);
        }
    }
    bool ApplicationSettingsNew::getOperatorsOn() const noexcept
    {
        return operatorsOn;
    }
    void ApplicationSettingsNew::setOperatorsOn(bool value)
    {
        operatorsOn = value;
        settings->setValue(settings::operators_on, std::to_string(value));
    }
} /* namespace app */
