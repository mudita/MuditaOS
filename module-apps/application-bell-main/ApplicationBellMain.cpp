// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "include/application-bell-main/ApplicationBellMain.hpp"
#include "windows/BellMainMenuWindow.hpp"
#include "windows/BellMainSetHour.hpp"
#include "windows/BellMainWindow.hpp"
#include <windows/Dialog.hpp>
#include <service-alarm/ServiceAlarmMessages.hpp>

namespace app
{
    ApplicationBellMain::ApplicationBellMain(std::string name,
                                             std::string parent,
                                             sys::phone_modes::PhoneMode mode,
                                             StartInBackground startInBackground)
        : Application(name, parent, mode, startInBackground)
    {
        bus.channels.push_back(sys::BusChannel::ServiceAlarmNotifications);

        // FAKE/EXAMPLE OF HANDLING SERVICE ALARM TRIGGER MESSAGE
        connect(typeid(AlarmResponseMessage), [&](sys::Message *msg) {
            auto alarmResponse = static_cast<AlarmResponseMessage *>(msg);
            if (alarmResponse->retCode == sys::ReturnCodes::Success) {
                LOG_INFO("Service Alarm response success");
            }
            else {
                LOG_INFO("Service Alarm response fail");
            }
            return sys::MessageNone{};
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
            return std::make_unique<gui::BellMainWindow>(app);
        });

        windowsFactory.attach(gui::window::name::bell_main_menu, [](Application *app, const std::string &name) {
            return std::make_unique<gui::BellMainMenuWindow>(app);
        });

        // for demo only - to be removed
        windowsFactory.attach(gui::window::name::bell_main_menu_dialog, [](Application *app, const std::string &name) {
            return std::make_unique<gui::Dialog>(app, name);
        });

        windowsFactory.attach(gui::window::name::bell_main_set_hour, [](Application *app, const std::string &name) {
            return std::make_unique<gui::BellMainSetHour>(app);
        });
    }

    sys::MessagePointer ApplicationBellMain::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
    {
        // FAKE/EXAMPLE OF HANDLING SERVICE ALARM TRIGGER MESSAGE
        if (msgl->messageType == MessageType::AlarmMessage) {
            LOG_INFO("Service Alarm triggered notification received");
            return sys::msgHandled();
        }

        auto retMsg = Application::DataReceivedHandler(msgl);

        if (dynamic_cast<sys::ResponseMessage *>(retMsg.get())->retCode == sys::ReturnCodes::Success) {
            return retMsg;
        }
        return std::make_shared<sys::ResponseMessage>();
    }
} // namespace app
