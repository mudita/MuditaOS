// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "include/application-bell-main/ApplicationBellMain.hpp"
#include "windows/BellMainMenuWindow.hpp"
#include "windows/BellMainWindow.hpp"
#include <windows/Dialog.hpp>

namespace app
{
    ApplicationBellMain::ApplicationBellMain(std::string name,
                                             std::string parent,
                                             sys::phone_modes::PhoneMode phoneMode,
                                             sys::bluetooth::BluetoothMode bluetoothMode,
                                             StartInBackground startInBackground)
        : Application(name, parent, phoneMode, bluetoothMode, startInBackground)
    {}

    sys::ReturnCodes ApplicationBellMain::InitHandler()
    {
        auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success) {
            return ret;
        }

        createUserInterface();

        return sys::ReturnCodes::Success;
    }

    void ApplicationBellMain::createUserInterface()
    {
        windowsFactory.attach(gui::name::window::main_window, [](Application *app, const std::string &name) {
            return std::make_unique<gui::BellMainWindow>(app);
        });

        windowsFactory.attach(gui::window::name::bell_main_menu, [](Application *app, const std::string &name) {
            return std::make_unique<gui::BellMainMenuWindow>(app);
        });

        // for demo only - to be removed
        windowsFactory.attach(gui::window::name::bell_main_menu_dialog, [](Application *app, const std::string &name) {
            return std::make_unique<gui::Dialog>(app, name);
        });
    }

    sys::MessagePointer ApplicationBellMain::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
    {
        auto retMsg = Application::DataReceivedHandler(msgl);
        if (dynamic_cast<sys::ResponseMessage *>(retMsg.get())->retCode == sys::ReturnCodes::Success) {
            return retMsg;
        }
        return std::make_shared<sys::ResponseMessage>();
    }
} // namespace app
