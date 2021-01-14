// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <service-evtmgr/EventManager.hpp>
#include <service-evtmgr/EVMessages.hpp>

#include <AlarmsRecord.hpp>
#include <MessageType.hpp>
#include <SwitchData.hpp>
#include <service-appmgr/Controller.hpp>
#include <service-db/DBServiceAPI.hpp>     // for DBServiceAPI

#include <memory>
#include <string>
#include <sys/types.h>
#include <utility>

namespace sys
{
    class DataMessage;
} // namespace sys

void EventManager::HandleAlarmTrigger(sys::DataMessage *msgl)
{
    sevm::RtcMinuteAlarmMessage *msg = reinterpret_cast<sevm::RtcMinuteAlarmMessage *>(msgl);

    // Send message to update time in application
    auto message       = std::make_shared<sevm::RtcMinuteAlarmMessage>(MessageType::EVMMinuteUpdated);
    message->timestamp = msg->timestamp;

    if (targetApplication.empty() == false) {
        bus.sendUnicast(message, targetApplication);
    }

    time_t currentTime = message->timestamp;

    // it there is no valid alarm and DB is not empty
    if (alarmIsValid == false) {
        // check if its midnight
        if (currentTime % 86400 == 0) {
            alarmDBEmpty = false;
        }

        if (alarmDBEmpty == false)
            // get next alarm
            GetNextAlarmTimestamp(currentTime);
    }

    // there is valid alarm
    if (alarmIsValid == true) {
        // check if its time to trigger alarm
        // round time and compare to alaram timestamp
        if (alarmTimestamp == (currentTime % 86400)) {
            alarmIsValid = false;
            // Run "alarm" application.
            // std::unique_ptr<gui::SwitchData> switchMessage = std::make_unique<sevm::EVMAlarmSwitchData>(alarmID);
            // app::manager::Controller::sendAction(this, app::manager::actions::ShowAlarm, std::move(switchMessage));
        }
        // check if alarm is not obsolete
        else if ((alarmTimestamp < (currentTime % 86400)) && ((currentTime + 60) % 86400) > (currentTime % 86400)) {
            alarmIsValid = false;
        }
    }
}
