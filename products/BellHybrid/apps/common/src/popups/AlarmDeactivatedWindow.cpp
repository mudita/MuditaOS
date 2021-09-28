// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <ApplicationCommon.hpp>
#include <apps-common/messages/DialogMetadataMessage.hpp>
#include <apps-common/popups/data/PopupRequestParams.hpp>
#include <apps-common/popups/Popups.hpp>
#include <common/popups/AlarmDeactivatedWindow.hpp>
#include <gui/input/InputEvent.hpp>
#include <gui/widgets/Icon.hpp>
#include <i18n/i18n.hpp>
#include <service-appmgr/Controller.hpp>
#include <service-time/AlarmMessage.hpp>
#include <service-time/Constants.hpp>

namespace gui
{
    AlarmDeactivatedWindow::AlarmDeactivatedWindow(app::ApplicationCommon *app)
        : WindowWithTimer(app, popup::window::alarm_deactivated_window), app::AsyncCallbackReceiver{app}
    {
        buildInterface();
        timerCallback = [this](Item &, sys::Timer &) {
            returnToPreviousWindow();
            return true;
        };
    }

    bool AlarmDeactivatedWindow::onInput(const InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease(KeyCode::KEY_ENTER) || inputEvent.isShortRelease(KeyCode::KEY_RF)) {
            returnToPreviousWindow();
            return true;
        }
        return false;
    }

    void AlarmDeactivatedWindow::buildInterface()
    {
        WindowWithTimer::buildInterface();
        auto task = app::AsyncRequest::createFromMessage(
            std::make_unique<alarms::AlarmGetFirstNextSingleEventRequestMessage>(), service::name::service_time);

        auto onResponseCallback = [this](auto response) { return onAlarmResponseMessage(response); };
        task->execute(application, this, onResponseCallback);

        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        bottomBar->setVisible(false);

        icon = new Icon(this,
                        0,
                        0,
                        style::window_width,
                        style::window_height,
                        "big_no-alarm",
                        utils::translate("app_bell_alarm_deactivated"));
        icon->text->setFont(style::window::font::verybiglight);
    }

    void AlarmDeactivatedWindow::returnToPreviousWindow()
    {
        app::manager::Controller::sendAction(application,
                                             app::manager::actions::AbortPopup,
                                             std::make_unique<gui::PopupRequestParams>(gui::popup::ID::AlarmActivated));
        application->returnToPreviousWindow();
    }

    void AlarmDeactivatedWindow::deactivateAlarm(AlarmEventRecord &alarmEvent)
    {
        alarmEvent.enabled = false;
        auto task          = app::AsyncRequest::createFromMessage(
            std::make_unique<alarms::AlarmUpdateRequestMessage>(alarmEvent), service::name::service_time);

        task->execute(this->application, this);
    }

    bool AlarmDeactivatedWindow::onAlarmResponseMessage(sys::ResponseMessage *response)
    {
        auto result = dynamic_cast<alarms::AlarmGetFirstNextSingleEventResponseMessage *>(response);
        if (result == nullptr || result->retCode != sys::ReturnCodes::Success) {
            LOG_WARN("Get next single event request failed!");
            return false;
        }
        auto alarmEventRecord = dynamic_cast<AlarmEventRecord *>(result->singleEvent.parent.get());
        if (alarmEventRecord == nullptr) {
            LOG_WARN("Getting alarm event record failed!");
            return false;
        }

        deactivateAlarm(*alarmEventRecord);
        return true;
    }
} /* namespace gui */
