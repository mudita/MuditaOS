/*
 * @file ApplicationCalendar.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 3 cze 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */

#include <map>
#include "windows/CalendarMainWindow.hpp"
#include "ApplicationCalendar.hpp"

namespace app
{

    ApplicationCalendar::ApplicationCalendar(std::string name,
                                             std::string parent,
                                             uint32_t stackDepth,
                                             sys::ServicePriority priority)
        : Application(name, parent, false, stackDepth, priority)
    {}

    ApplicationCalendar::~ApplicationCalendar()
    {}

    // Invoked upon receiving data message
    sys::Message_t ApplicationCalendar::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
    {
        return std::make_shared<sys::ResponseMessage>();
    }

    // Invoked during initialization
    sys::ReturnCodes ApplicationCalendar::InitHandler()
    {

        createUserInterface();

        setActiveWindow("Main");

        return sys::ReturnCodes::Success;
    }

    sys::ReturnCodes ApplicationCalendar::DeinitHandler()
    {
        return sys::ReturnCodes::Success;
    }

    void ApplicationCalendar::createUserInterface()
    {

        gui::AppWindow *win = new CalendarMainWindow(this, "Main");
        windows.insert(std::pair<std::string, gui::AppWindow *>(win->getName(), win));
    }

    void ApplicationCalendar::destroyUserInterface()
    {}

} /* namespace app */
