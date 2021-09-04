// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationBellPowerNap.hpp"
#include "presenter/PowerNapMainWindowPresenter.hpp"
#include "presenter/PowerNapProgressPresenter.hpp"
#include "windows/PowerNapMainWindow.hpp"
#include "windows/PowerNapProgressWindow.hpp"

namespace app
{
    ApplicationBellPowerNap::ApplicationBellPowerNap(std::string name,
                                                     std::string parent,
                                                     sys::phone_modes::PhoneMode mode,
                                                     sys::bluetooth::BluetoothMode bluetoothMode,
                                                     StartInBackground startInBackground)
        : Application(std::move(name), std::move(parent), mode, bluetoothMode, startInBackground)
    {}

    sys::ReturnCodes ApplicationBellPowerNap::InitHandler()
    {
        auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success) {
            return ret;
        }
        createUserInterface();

        return sys::ReturnCodes::Success;
    }

    void ApplicationBellPowerNap::createUserInterface()
    {
        windowsFactory.attach(gui::name::window::main_window, [this](Application *app, const std::string &name) {
            auto presenter = std::make_unique<powernap::PowerNapMainWindowPresenter>(app, settings.get());
            return std::make_unique<gui::PowerNapMainWindow>(app, std::move(presenter));
        });
        windowsFactory.attach(gui::window::name::powernapProgress, [this](Application *app, const std::string &name) {
            auto presenter = std::make_unique<powernap::PowerNapProgressPresenter>(app, settings.get());
            return std::make_unique<gui::PowerNapProgressWindow>(app, std::move(presenter));
        });
    }

    sys::MessagePointer ApplicationBellPowerNap::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
    {
        auto retMsg = Application::DataReceivedHandler(msgl);
        if (auto response = dynamic_cast<sys::ResponseMessage *>(retMsg.get());
            response != nullptr && response->retCode == sys::ReturnCodes::Success) {
            return retMsg;
        }
        return sys::msgHandled();
    }
} // namespace app
