// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationBellClock.hpp"
#include "presenter/BellClockWindowPresenter.hpp"
#include "windows/BellClockWindow.hpp"
#include <common/windows/BellFinishedWindow.hpp>

namespace app
{
    ApplicationBellClock::ApplicationBellClock(std::string name,
                                               std::string parent,
                                               StatusIndicators statusIndicators,
                                               StartInBackground startInBackground,
                                               uint32_t stackDepth)
        : Application(name, parent, statusIndicators, startInBackground, stackDepth)
    {}

    sys::ReturnCodes ApplicationBellClock::InitHandler()
    {
        auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success) {
            return ret;
        }

        createUserInterface();

        return sys::ReturnCodes::Success;
    }

    void ApplicationBellClock::createUserInterface()
    {
        windowsFactory.attach(gui::name::window::main_window, [](ApplicationCommon *app, const std::string &) {
            auto presenter = std::make_unique<bell_clock::BellClockWindowPresenter>();
            return std::make_unique<gui::BellClockWindow>(app, std::move(presenter));
        });

        windowsFactory.attach(gui::window::bell_finished::defaultName,
                              [](ApplicationCommon *app, const std::string &name) {
                                  return std::make_unique<gui::BellFinishedWindow>(app, name);
                              });

        attachPopups({gui::popup::ID::AlarmActivated,
                      gui::popup::ID::AlarmDeactivated,
                      gui::popup::ID::PowerOff,
                      gui::popup::ID::Reboot,
                      gui::popup::ID::ChargingNotification,
                      gui::popup::ID::ChargingDoneNotification});
    }

    sys::MessagePointer ApplicationBellClock::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
    {
        auto retMsg = Application::DataReceivedHandler(msgl);
        if (dynamic_cast<sys::ResponseMessage *>(retMsg.get())->retCode == sys::ReturnCodes::Success) {
            return retMsg;
        }
        return handleAsyncResponse(resp);
    }
} // namespace app
