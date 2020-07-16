/*
 * @file ApplicationCalendar.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 3 cze 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */

#include "ApplicationCalendar.hpp"
#include "windows/CalendarMainWindow.hpp"
#include "windows/DayWindow.hpp"
#include "NoEvents.hpp"
#include "application-calendar/widgets/CalendarStyle.hpp"
#include <map>

namespace app
{

    ApplicationCalendar::ApplicationCalendar(std::string name,
                                             std::string parent,
                                             uint32_t stackDepth,
                                             sys::ServicePriority priority)
        : Application(name, parent, false, stackDepth, priority)
    {}

    sys::Message_t ApplicationCalendar::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
    {
        return Application::DataReceivedHandler(msgl);
    }

    sys::ReturnCodes ApplicationCalendar::InitHandler()
    {
        auto ret = Application::InitHandler();
        createUserInterface();
        return ret;
    }

    sys::ReturnCodes ApplicationCalendar::DeinitHandler()
    {
        return sys::ReturnCodes::Success;
    }

    void ApplicationCalendar::createUserInterface()
    {

        windows.insert(std::pair<std::string, gui::AppWindow *>(
            gui::name::window::main_window, new gui::CalendarMainWindow(this, gui::name::window::main_window)));

        windows.insert(std::pair<std::string, gui::AppWindow *>(
            style::window::calendar::name::day_window, new DayWindow(this, style::window::calendar::name::day_window)));
        windows.insert(std::pair<std::string, gui::AppWindow *>(
            style::window::calendar::name::no_events_window,
            new gui::NoEvents(this, style::window::calendar::name::no_events_window, gui::NoEvents::Meta())));
    }

    void ApplicationCalendar::destroyUserInterface()
    {}

} /* namespace app */
