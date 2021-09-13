// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationBellAlarm.hpp"
#include "presenter/BellAlarmWindowPresenter.hpp"
#include "windows/BellAlarmWindow.hpp"

namespace app
{
    ApplicationBellAlarm::ApplicationBellAlarm(std::string name,
                                               std::string parent,
                                               sys::phone_modes::PhoneMode mode,
                                               sys::bluetooth::BluetoothMode bluetoothMode,
                                               StartInBackground startInBackground)
        : Application(std::move(name), std::move(parent), mode, bluetoothMode, startInBackground)
    {}

    sys::ReturnCodes ApplicationBellAlarm::InitHandler()
    {
        auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success) {
            return ret;
        }

        createUserInterface();

        return sys::ReturnCodes::Success;
    }

    void ApplicationBellAlarm::createUserInterface()
    {
        windowsFactory.attach(gui::name::window::main_window, [](ApplicationCommon *app, const std::string &name) {
            auto presenter = std::make_unique<bell_alarm::BellAlarmWindowPresenter>();
            return std::make_unique<gui::BellAlarmWindow>(app, std::move(presenter));
        });

        attachPopups({gui::popup::ID::AlarmActivated, gui::popup::ID::AlarmDeactivated});
    }

    sys::MessagePointer ApplicationBellAlarm::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
    {
        auto retMsg = Application::DataReceivedHandler(msgl);
        if (dynamic_cast<sys::ResponseMessage *>(retMsg.get())->retCode == sys::ReturnCodes::Success) {
            return retMsg;
        }
        return handleAsyncResponse(resp);
    }
} // namespace app
