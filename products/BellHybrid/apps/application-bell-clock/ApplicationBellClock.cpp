// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationBellClock.hpp"
#include "windows/BellClockWindow.hpp"
#include "windows/BellRefreshClockWindow.hpp"
#include "models/DeepRefreshInterval.hpp"
#include "presenter/BellClockWindowPresenter.hpp"
#include "presenter/SettingsPresenter.hpp"
#include <common/windows/BellFinishedWindow.hpp>
#include <common/models/TimeModel.hpp>
#include <apps-common/messages/AppMessage.hpp>

namespace app
{
    ApplicationBellClock::ApplicationBellClock(std::string name,
                                               std::string parent,
                                               StatusIndicators statusIndicators,
                                               StartInBackground startInBackground,
                                               std::uint32_t stackDepth)
        : Application(name, parent, statusIndicators, startInBackground, stackDepth)
    {}

    sys::ReturnCodes ApplicationBellClock::InitHandler()
    {
        auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success) {
            return ret;
        }

        refreshModel = std::make_unique<clock::models::DeepRefreshInterval>(this);
        createUserInterface();

        return sys::ReturnCodes::Success;
    }

    void ApplicationBellClock::createUserInterface()
    {
        windowsFactory.attach(gui::name::window::main_window, [this](ApplicationCommon *app, const std::string &name) {
            auto presenter = std::make_unique<app::clock::SettingsPresenter>(*refreshModel);
            return std::make_unique<clock::BellRefreshClockWindow>(app, std::move(presenter));
        });

        windowsFactory.attach(gui::BellClockWindow::name, [this](ApplicationCommon *app, const std::string &) {
            auto timeModel = std::make_unique<app::TimeModel>();
            auto presenter =
                std::make_unique<bell_clock::BellClockWindowPresenter>(std::move(timeModel), *refreshModel);
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

    sys::MessagePointer ApplicationBellClock::handleSwitchWindow(sys::Message *msgl)
    {
        if (auto msg = dynamic_cast<AppSwitchWindowMessage *>(msgl); msg) {
            const auto newWindowName = msg->getWindowName();
            if (newWindowName == gui::BellClockWindow::name) {
                stopIdleTimer();
            }
            else {
                startIdleTimer();
            }
        }
        return ApplicationCommon::handleSwitchWindow(msgl);
    }
} // namespace app
