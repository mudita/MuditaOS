// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "include/application-bell-main/ApplicationBellMain.hpp"
#include "models/TemperatureModel.hpp"
#include "models/TimeModel.hpp"
#include "presenters/HomeScreenPresenter.hpp"

#include "windows/BellHomeScreenWindow.hpp"
#include "windows/BellMainMenuWindow.hpp"

#include <common/models/AlarmModel.hpp>
#include <service-db/DBNotificationMessage.hpp>
#include <windows/Dialog.hpp>

namespace app
{
    ApplicationBellMain::ApplicationBellMain(std::string name,
                                             std::string parent,
                                             sys::phone_modes::PhoneMode mode,
                                             sys::bluetooth::BluetoothMode bluetoothMode,
                                             StartInBackground startInBackground)
        : Application(name, parent, mode, bluetoothMode, startInBackground)
    {
        bus.channels.push_back(sys::BusChannel::ServiceDBNotifications);
        addActionReceiver(manager::actions::ShowAlarm, [this](auto &&data) {
            switchWindow(gui::name::window::main_window, std::move(data));
            return actionHandled();
        });
    }

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
            auto alarmModel       = std::make_unique<app::AlarmModel>(app);
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
        auto retMsg            = Application::DataReceivedHandler(msgl);
        const auto respMessage = dynamic_cast<sys::ResponseMessage *>(retMsg.get());
        if (respMessage != nullptr && respMessage->retCode == sys::ReturnCodes::Success) {
            return retMsg;
        }
        auto msg = dynamic_cast<db::NotificationMessage *>(msgl);
        if (msg != nullptr) {
            for (auto &[name, window] : windowsStack.windows) {
                window->onDatabaseMessage(msg);
            }
            return sys::msgHandled();
        }
        return handleAsyncResponse(resp);
    }

    void ApplicationBellMain::showPopup(gui::popup::ID id, const gui::PopupRequestParams *params)
    {
        if (id == gui::popup::ID::AlarmActivated) {
            if (not isHomeScreenFocused()) {
                switchWindow(gui::popup::resolveWindowName(id));
            }
        }
        else {
            Application::showPopup(id, params);
        }
    }

    auto ApplicationBellMain::isHomeScreenFocused() -> bool
    {
        return GetName() == app::applicationBellName && getCurrentWindow()->getName() == gui::name::window::main_window;
    }
} // namespace app
