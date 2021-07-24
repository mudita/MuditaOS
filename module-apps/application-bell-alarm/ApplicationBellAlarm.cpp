// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <application-bell-alarm/ApplicationBellAlarm.hpp>
#include <application-bell-alarm/windows/BellAlarmWindow.hpp>

namespace app
{
    ApplicationBellAlarm::ApplicationBellAlarm(std::string name,
                                               std::string parent,
                                               sys::phone_modes::PhoneMode mode,
                                               StartInBackground startInBackground)
        : Application(std::move(name), std::move(parent), mode, startInBackground)
    {}

    sys::ReturnCodes ApplicationBellAlarm::InitHandler()
    {
        auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success) {
            return ret;
        }

        createUserInterface();

        return sys::ReturnCodes::Success;
    }

    void ApplicationBellAlarm::createUserInterface()
    {
        windowsFactory.attach(gui::name::window::main_window, [](Application *app, const std::string &name) {
            return std::make_unique<gui::BellAlarmWindow>(app);
        });
    }

    sys::MessagePointer ApplicationBellAlarm::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
    {
        auto retMsg = Application::DataReceivedHandler(msgl);
        if (dynamic_cast<sys::ResponseMessage *>(retMsg.get())->retCode == sys::ReturnCodes::Success) {
            return retMsg;
        }
        return std::make_shared<sys::ResponseMessage>();
    }
} // namespace app
