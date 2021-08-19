// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "include/application-bell-main/ApplicationBellMain.hpp"
#include "models/AlarmModel.hpp"
#include "models/TemperatureModel.hpp"
#include "models/TimeModel.hpp"
#include "presenters/HomeScreenPresenter.hpp"

#include "windows/BellHomeScreenWindow.hpp"
#include "windows/BellMainMenuWindow.hpp"

#include <windows/Dialog.hpp>

namespace app
{
    ApplicationBellMain::ApplicationBellMain(std::string name,
                                             std::string parent,
                                             sys::phone_modes::PhoneMode mode,
                                             StartInBackground startInBackground)
        : Application(name, parent, mode, startInBackground)
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
            auto timeModel        = std::make_unique<app::home_screen::TimeModel>();
            auto temperatureModel = std::make_unique<app::home_screen::TemperatureModel>(app);
            auto alarmModel       = std::make_unique<app::home_screen::AlarmModel>(app);
            auto presenter        = std::make_unique<app::home_screen::HomeScreenPresenter>(
                app, std::move(alarmModel), std::move(temperatureModel), std::move(timeModel));
            return std::make_unique<gui::BellHomeScreenWindow>(app, std::move(presenter));
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
