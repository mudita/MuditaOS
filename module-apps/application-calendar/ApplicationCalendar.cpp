﻿// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationCalendar.hpp"
#include <application-calendar/windows/CalendarMainWindow.hpp>

namespace app
{
    ApplicationCalendar::ApplicationCalendar(std::string name,
                                             std::string parent,
                                             sys::phone_modes::PhoneMode mode,
                                             StartInBackground startInBackground,
                                             uint32_t stackDepth,
                                             sys::ServicePriority priority)
        : Application(name, parent, mode, startInBackground, stackDepth, priority)
    {}

    sys::MessagePointer ApplicationCalendar::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
    {
        auto retMsg = Application::DataReceivedHandler(msgl);
        // if message was handled by application's template there is no need to process further.
        if (retMsg && (dynamic_cast<sys::ResponseMessage *>(retMsg.get())->retCode == sys::ReturnCodes::Success)) {
            return retMsg;
        }
        return std::make_shared<sys::ResponseMessage>();
    }

    sys::ReturnCodes ApplicationCalendar::InitHandler()
    {
        auto ret             = Application::InitHandler();
        createUserInterface();
        return ret;
    }

    sys::ReturnCodes ApplicationCalendar::DeinitHandler()
    {
        return Application::DeinitHandler();
    }

    void ApplicationCalendar::createUserInterface()
    {
        windowsFactory.attach(gui::name::window::main_window, [](Application *app, const std::string &name) {
            return std::make_unique<gui::CalendarMainWindow>(app, name);
        });

        attachPopups(
            {gui::popup::ID::Volume, gui::popup::ID::Tethering, gui::popup::ID::PhoneModes, gui::popup::ID::PhoneLock});
    }

    void ApplicationCalendar::destroyUserInterface()
    {}

} /* namespace app */
