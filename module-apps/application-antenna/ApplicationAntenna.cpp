/*
 * ApplicationAntenna.cpp
 *
 *  Created on: 18 lut 2020
 *      Author: kuba
 */

#include "ApplicationAntenna.hpp"
#include "windows/AntennaMainWindow.hpp"
#include <map>

namespace app
{

    ApplicationAntenna::ApplicationAntenna(std::string name, std::string parent, bool startBackgound) : Application(name, parent, startBackgound, 4096 * 2)
    {
    }

    ApplicationAntenna::~ApplicationAntenna()
    {
    }

    // Invoked upon receiving data message
    sys::Message_t ApplicationAntenna::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
    {
        return std::make_shared<sys::ResponseMessage>();
    }

    // Invoked during initialization
    sys::ReturnCodes ApplicationAntenna::InitHandler()
    {

        auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success)
            return ret;

        createUserInterface();

        setActiveWindow(gui::name::window::main_window);

        return ret;
    }

    sys::ReturnCodes ApplicationAntenna::DeinitHandler()
    {
        return sys::ReturnCodes::Success;
    }

    void ApplicationAntenna::createUserInterface()
    {

        gui::AppWindow *win = new gui::AntennaMainWindow(this);
        win->setSize(480, 600);

        windows.insert(std::pair<std::string, gui::AppWindow *>(gui::name::window::main_window, win));
    }

    void ApplicationAntenna::destroyUserInterface()
    {
    }

} /* namespace app */
