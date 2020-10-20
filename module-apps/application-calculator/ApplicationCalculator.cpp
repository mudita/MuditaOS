// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationCalculator.hpp"
#include "windows/CalculatorMainWindow.hpp"
#include <i18/i18.hpp>

namespace app
{

    ApplicationCalculator::ApplicationCalculator(std::string name, std::string parent, bool startBackgound)
        : Application(name, parent, startBackgound, stack_size)
    {}

    sys::Message_t ApplicationCalculator::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
    {
        return Application::DataReceivedHandler(msgl);
    }

    // Invoked during initialization
    sys::ReturnCodes ApplicationCalculator::InitHandler()
    {
        auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success)
            return ret;

        createUserInterface();

        setActiveWindow(gui::name::window::main_window);

        return ret;
    }

    sys::ReturnCodes ApplicationCalculator::DeinitHandler()
    {
        return sys::ReturnCodes::Success;
    }

    void ApplicationCalculator::createUserInterface()
    {
        windowsFactory.attach(gui::name::window::main_window, [](Application *app, const std::string &name) {
            return std::make_unique<gui::CalculatorMainWindow>(app, name);
        });
    }

    void ApplicationCalculator::destroyUserInterface()
    {}

} /* namespace app */
