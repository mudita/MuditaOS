// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationBellAlarm.hpp"
#include "presenter/BellAlarmWindowPresenter.hpp"
#include "presenter/BellAlarmSetPresenter.hpp"
#include "windows/BellAlarmWindow.hpp"
#include "windows/BellAlarmSetWindow.hpp"

#include <common/models/AlarmModel.hpp>
#include <appmgr/messages/AlarmMessage.hpp>
#include <common/models/TimeModel.hpp>

namespace app
{
    ApplicationBellAlarm::ApplicationBellAlarm(std::string name,
                                               std::string parent,
                                               StatusIndicators statusIndicators,
                                               StartInBackground startInBackground,
                                               uint32_t stackDepth)
        : Application(std::move(name), std::move(parent), statusIndicators, startInBackground, stackDepth)
    {
        bus.channels.push_back(sys::BusChannel::AlarmNotifications);
        connect(typeid(AlarmDeactivated), [this](sys::Message *request) -> sys::MessagePointer {
            alarmModel->activate(false);
            return sys::msgHandled();
        });
        connect(typeid(AlarmActivated), [this](sys::Message *request) -> sys::MessagePointer {
            alarmModel->activate(true);
            return sys::msgHandled();
        });
    }
    ApplicationBellAlarm::~ApplicationBellAlarm() = default;

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
        windowsFactory.attach(gui::name::window::main_window, [&](ApplicationCommon *app, const std::string &name) {
            auto timeModel      = std::make_shared<app::TimeModel>();
            auto alarmPresenter = std::make_shared<bell_alarm::BellAlarmWindowPresenter>(*alarmModel, timeModel);
            return std::make_unique<gui::BellAlarmWindow>(app, alarmPresenter, name);
        });

        windowsFactory.attach(gui::window::name::bellAlarmSet, [&](ApplicationCommon *app, const std::string &) {
            auto alarmPresenter = std::make_unique<bell_alarm::BellAlarmSetPresenter>(app, *alarmModel);
            return std::make_unique<gui::BellAlarmSetWindow>(app, std::move(alarmPresenter));
        });

        attachPopups({gui::popup::ID::AlarmActivated,
                      gui::popup::ID::AlarmDeactivated,
                      gui::popup::ID::PowerOff,
                      gui::popup::ID::Reboot,
                      gui::popup::ID::BedtimeNotification,
                      gui::popup::ID::ChargingNotification,
                      gui::popup::ID::ChargingDoneNotification});
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
