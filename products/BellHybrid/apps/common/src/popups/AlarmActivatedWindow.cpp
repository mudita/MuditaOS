// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <ApplicationCommon.hpp>
#include <apps-common/popups/data/PopupRequestParams.hpp>
#include <common/popups/AlarmActivatedWindow.hpp>
#include <common/TimeUtils.hpp>
#include <gui/input/InputEvent.hpp>
#include <gui/widgets/Icon.hpp>
#include <popups/Popups.hpp>
#include <service-appmgr/Controller.hpp>
#include <service-time/AlarmMessage.hpp>
#include <service-time/Constants.hpp>

namespace gui
{
    AlarmActivatedWindow::AlarmActivatedWindow(app::ApplicationCommon *app)
        : WindowWithTimer(app, popup::window::alarm_activated_window), app::AsyncCallbackReceiver{app}
    {
        buildInterface();
        timerCallback = [this](Item &, sys::Timer &) {
            returnToPreviousWindow();
            return true;
        };
    }

    void AlarmActivatedWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        WindowWithTimer::onBeforeShow(mode, data);
        auto task = app::AsyncRequest::createFromMessage(
            std::make_unique<alarms::AlarmGetFirstNextSingleEventRequestMessage>(), service::name::service_time);

        auto onResponseCallback = [this, mode](auto response) { return onAlarmResponseMessage(response, mode); };
        task->execute(this->application, this, onResponseCallback);
    }

    void AlarmActivatedWindow::activateAlarm(AlarmEventRecord &alarmEvent)
    {
        alarmEvent.enabled = true;
        auto task          = app::AsyncRequest::createFromMessage(
            std::make_unique<alarms::AlarmUpdateRequestMessage>(alarmEvent), service::name::service_time);

        task->execute(application, this);
    }

    bool AlarmActivatedWindow::onAlarmResponseMessage(sys::ResponseMessage *response, ShowMode mode)
    {
        auto result = dynamic_cast<alarms::AlarmGetFirstNextSingleEventResponseMessage *>(response);
        if (result == nullptr || result->retCode != sys::ReturnCodes::Success) {
            LOG_WARN("Get next single event request failed!");
            return false;
        }
        const auto &singleEventRecord = result->singleEvent;
        const auto &startDate         = singleEventRecord.startDate;
        LOG_DEBUG("Alarm time: %s", TimePointToString(startDate).c_str());
        const auto alarmTime = std::chrono::system_clock::to_time_t(startDate);
        showAlarmTime(mode, alarmTime);

        auto alarmEventRecord = dynamic_cast<AlarmEventRecord *>(singleEventRecord.parent.get());
        if (alarmEventRecord == nullptr) {
            LOG_WARN("Getting alarm event record failed!");
            return false;
        }
        activateAlarm(*alarmEventRecord);

        return true;
    }

    void AlarmActivatedWindow::showAlarmTime(ShowMode, time_t alarmTime)
    {
        icon->text->setRichText(utils::time::getBottomDescription(
            utils::time::calculateTimeDifference(alarmTime, utils::time::getCurrentTime())));
    }
    void AlarmActivatedWindow::buildInterface()
    {
        WindowWithTimer::buildInterface();

        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        bottomBar->setVisible(false);

        icon = new Icon(this, 0, 0, style::window_width, style::window_height, "big_alarm", {});
        icon->text->setFont(style::window::font::verybiglight);
    }
    void AlarmActivatedWindow::returnToPreviousWindow()
    {
        app::manager::Controller::sendAction(
            application,
            app::manager::actions::AbortPopup,
            std::make_unique<gui::PopupRequestParams>(gui::popup::ID::AlarmDeactivated));
        application->returnToPreviousWindow();
    }
    bool AlarmActivatedWindow::onInput(const InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease(KeyCode::KEY_ENTER) || inputEvent.isShortRelease(KeyCode::KEY_RF)) {
            returnToPreviousWindow();
            return true;
        }
        return false;
    }
} /* namespace gui */
