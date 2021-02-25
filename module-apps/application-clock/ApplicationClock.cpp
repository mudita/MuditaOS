// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

// module-gui
#include "AppWindow.hpp"
#include "Service/Timer.hpp"
#include "gui/widgets/Window.hpp"

// module-utils
#include "log/log.hpp"
#include <service-evtmgr/EVMessages.hpp>
#include <service-evtmgr/EventManager.hpp>
#include <service-appmgr/model/ApplicationManager.hpp>
// MessageType
#include "MessageType.hpp"
// this module
#include "windows/ClockMainWindow.hpp"
#include "ApplicationClock.hpp"

namespace app
{
    ApplicationClock::ApplicationClock(std::string name,
                                       std::string parent,
                                       StartInBackground startInBackground,
                                       uint32_t stackDepth,
                                       sys::ServicePriority priority)
        : Application(name, parent, startInBackground, stackDepth, priority)
    {
        timerClock = std::make_unique<sys::Timer>("Clock", this, 1000);
        timerClock->connect([&](sys::Timer &) { timerClockCallback(); });
        timerClock->start();
    }

    void ApplicationClock::timerClockCallback()
    {
        auto win = reinterpret_cast<gui::ClockMainWindow *>(windowsStack.get(gui::name::window::main_window));
        win->incrementSecond();
        win->updateLabels();
        render(gui::RefreshModes::GUI_REFRESH_FAST);
    }

    // Invoked upon receiving data message
    sys::MessagePointer ApplicationClock::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
    {

        auto retMsg = Application::DataReceivedHandler(msgl);
        // if message was handled by application's template there is no need to process further.
        if (reinterpret_cast<sys::ResponseMessage *>(retMsg.get())->retCode == sys::ReturnCodes::Success) {
            return retMsg;
        }

        // this variable defines whether message was processed.
        bool handled = false;
        // if keyboard message received

        if (handled)
            return std::make_shared<sys::ResponseMessage>();
        else
            return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Unresolved);
    }

    // Invoked during initialization
    sys::ReturnCodes ApplicationClock::InitHandler()
    {

        auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success)
            return ret;

        createUserInterface();

        setActiveWindow("MainWindow");

        return ret;
    }

    sys::ReturnCodes ApplicationClock::DeinitHandler()
    {
        timerClock->stop();
        return sys::ReturnCodes::Success;
    }

    void ApplicationClock::createUserInterface()
    {
        windowsFactory.attach(gui::name::window::main_window, [](Application *app, const std::string &name) {
            return std::make_unique<gui::ClockMainWindow>(app, name);
        });

        attachPopups({gui::popup::ID::Volume});
    }

    void ApplicationClock::destroyUserInterface()
    {}

} /* namespace app */
