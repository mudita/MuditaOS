// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <application-calculator/ApplicationCalculator.hpp>
#include <windows/CalculatorMainWindow.hpp>
#include <i18n/i18n.hpp>

namespace app
{
    ApplicationCalculator::ApplicationCalculator(std::string name,
                                                 std::string parent,
                                                 StatusIndicators statusIndicators,
                                                 StartInBackground startInBackground)
        : Application(name, parent, statusIndicators, startInBackground, stack_size)
    {}

    sys::MessagePointer ApplicationCalculator::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
    {
        auto retMsg = Application::DataReceivedHandler(msgl);
        // if message was handled by application's template there is no need to process further.
        if (retMsg && (dynamic_cast<sys::ResponseMessage *>(retMsg.get())->retCode == sys::ReturnCodes::Success)) {
            return retMsg;
        }

        return handleAsyncResponse(resp);
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

    void ApplicationCalculator::createUserInterface()
    {
        windowsFactory.attach(gui::name::window::main_window, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::CalculatorMainWindow>(app, name);
        });

        attachPopups({gui::popup::ID::Volume,
                      gui::popup::ID::Tethering,
                      gui::popup::ID::BluetoothAuthenticate,
                      gui::popup::ID::PhoneModes,
                      gui::popup::ID::PhoneLock,
                      gui::popup::ID::SimLock,
                      gui::popup::ID::Alarm});
    }

    void ApplicationCalculator::destroyUserInterface()
    {}

} /* namespace app */
