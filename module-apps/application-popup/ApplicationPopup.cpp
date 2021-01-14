// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <module-apps/application-popup/windows/EventReminderWindow.hpp>
#include "ApplicationPopup.hpp"

namespace app
{

    ApplicationPopup::ApplicationPopup(std::string name, std::string parent, StartInBackground startInBackground)
        : Application{std::move(name), std::move(parent), startInBackground}
    {
        addActionReceiver(manager::actions::ShowReminder, [this](auto &&data) {
            switchWindow(style::window::name::event_reminder_window, std::move(data));
            return msgHandled();
        });
    }

    auto ApplicationPopup::InitHandler() -> sys::ReturnCodes
    {
        const auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success)
            return ret;

        createUserInterface();

        setActiveWindow(style::window::name::event_reminder_window);
        return ret;
    }

    auto ApplicationPopup::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
        -> sys::MessagePointer
    {
        return Application::DataReceivedHandler(msgl);
    }

    auto ApplicationPopup::SwitchPowerModeHandler(sys::ServicePowerMode mode) -> sys::ReturnCodes
    {
        return sys::ReturnCodes::Success;
    }

    void ApplicationPopup::createUserInterface()
    {
        windowsFactory.attach(
            style::window::name::event_reminder_window, [](Application *app, const std::string &name) {
                return std::make_unique<gui::EventReminderWindow>(app, style::window::name::event_reminder_window);
            });
    }

    void ApplicationPopup::destroyUserInterface()
    {}
} // namespace app
