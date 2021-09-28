// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationBellAlarm.hpp"
#include "presenter/BellAlarmWindowPresenter.hpp"
#include "presenter/BellAlarmSetPresenter.hpp"
#include "windows/BellAlarmWindow.hpp"
#include "windows/BellAlarmSetWindow.hpp"

#include <common/models/AlarmModel.hpp>
#include <common/models/TimeModel.hpp>

namespace app
{

    namespace internal
    {
        class BellAlarmPriv
        {
          public:
            std::shared_ptr<AlarmModel> alarmModel;
            std::shared_ptr<TimeModel> timeModel;
            std::shared_ptr<bell_alarm::BellAlarmWindowPresenter> alarmPresenter;
            std::shared_ptr<bell_alarm::BellAlarmSetPresenter> alarmSetPresenter;
        };
    } // namespace internal

    ApplicationBellAlarm::ApplicationBellAlarm(std::string name,
                                               std::string parent,
                                               StatusIndicators statusIndicators,
                                               StartInBackground startInBackground,
                                               uint32_t stackDepth)
        : Application(std::move(name), std::move(parent), statusIndicators, startInBackground, stackDepth),
          priv{std::make_unique<app::internal::BellAlarmPriv>()}
    {}
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
            priv->alarmModel = std::make_shared<app::AlarmModel>(app);
            priv->timeModel  = std::make_shared<app::TimeModel>();
            priv->alarmPresenter =
                std::make_shared<bell_alarm::BellAlarmWindowPresenter>(priv->alarmModel, priv->timeModel);
            return std::make_unique<gui::BellAlarmWindow>(app, priv->alarmPresenter);
        });

        windowsFactory.attach(gui::window::name::bellAlarmSet, [&](ApplicationCommon *app, const std::string &name) {
            priv->alarmSetPresenter = std::make_unique<bell_alarm::BellAlarmSetPresenter>(app, priv->alarmModel);
            return std::make_unique<gui::BellAlarmSetWindow>(app, priv->alarmSetPresenter);
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
