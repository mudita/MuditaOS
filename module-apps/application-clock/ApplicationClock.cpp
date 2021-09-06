// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

// module-gui
#include "AppWindow.hpp"
#include "gui/widgets/Window.hpp"

// module-utils
#include <log.hpp>
#include <service-evtmgr/EVMessages.hpp>
#include <service-evtmgr/EventManagerCommon.hpp>
#include <module-sys/Timers/TimerFactory.hpp>
// MessageType
#include "MessageType.hpp"
// this module
#include "windows/ClockMainWindow.hpp"
#include "ApplicationClock.hpp"

namespace app
{
    ApplicationClock::ApplicationClock(std::string name,
                                       std::string parent,
                                       sys::phone_modes::PhoneMode phoneMode,
                                       sys::bluetooth::BluetoothMode bluetoothMode,
                                       StartInBackground startInBackground,
                                       uint32_t stackDepth,
                                       sys::ServicePriority priority)
        : Application(name, parent, phoneMode, bluetoothMode, startInBackground, stackDepth, priority)
    {
        timerClock = sys::TimerFactory::createPeriodicTimer(
            this, "Clock", std::chrono::milliseconds{1000}, [&](sys::Timer &) { timerClockCallback(); });
        timerClock.start();
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
        timerClock.stop();
        return Application::DeinitHandler();
    }

    void ApplicationClock::createUserInterface()
    {
        windowsFactory.attach(gui::name::window::main_window, [](Application *app, const std::string &name) {
            return std::make_unique<gui::ClockMainWindow>(app, name);
        });

        attachPopups(
            {gui::popup::ID::Volume, gui::popup::ID::Tethering, gui::popup::ID::PhoneModes, gui::popup::ID::PhoneLock});
    }

    void ApplicationClock::destroyUserInterface()
    {}

} /* namespace app */
