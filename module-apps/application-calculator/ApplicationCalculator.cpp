// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <application-calculator/ApplicationCalculator.hpp>
#include <windows/CalculatorMainWindow.hpp>
#include <i18n/i18n.hpp>

namespace app
{
    ApplicationCalculator::ApplicationCalculator(std::string name,
                                                 std::string parent,
                                                 sys::phone_modes::PhoneMode mode,
                                                 StartInBackground startInBackground)
        : Application(name, parent, mode, startInBackground, stack_size)
    {}

    sys::MessagePointer ApplicationCalculator::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
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

        attachPopups(
            {gui::popup::ID::Volume, gui::popup::ID::Tethering, gui::popup::ID::PhoneModes, gui::popup::ID::PhoneLock});
    }

    void ApplicationCalculator::destroyUserInterface()
    {}

} /* namespace app */
