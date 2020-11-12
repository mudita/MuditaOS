// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

/*
 * EventManagerAlarm.cpp
 *
 *  Created on: 18 lip 2019
 *      Author: kuba
 */

#include <sys/types.h> // for time_t
#include <memory>      // for allocator, make_shared, make_unique, __shared_ptr_access, shared_ptr, unique_ptr
#include <string>      // for string
#include <utility>     // for move

#include "../EventManager.hpp"             // for EventManager
#include "../messages/EVMessages.hpp"      // for RtcMinuteAlarmMessage, EVMAlarmSwitchData
#include <service-db/DBServiceAPI.hpp>     // for DBServiceAPI
#include "service-appmgr/Controller.hpp"   // for Controller
#include "AlarmsRecord.hpp"                // for AlarmsRecord
#include "MessageType.hpp"                 // for MessageType, MessageType::EVMMinuteUpdated
#include "Service/Bus.hpp"                 // for Bus
#include "SwitchData.hpp"                  // for SwitchData

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
        sys::Bus::SendUnicast(message, targetApplication, this);
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
            // run bell application
            std::unique_ptr<gui::SwitchData> switchMessage = std::make_unique<sevm::EVMAlarmSwitchData>(alarmID);
            app::manager::Controller::switchApplication(this, "bell", "main", std::move(switchMessage));
        }
        // check if alarm is not obsolete
        else if ((alarmTimestamp < (currentTime % 86400)) && ((currentTime + 60) % 86400) > (currentTime % 86400)) {
            alarmIsValid = false;
        }
    }
}

void EventManager::GetNextAlarmTimestamp(time_t timestamp)
{
    AlarmsRecord record;
    record = DBServiceAPI::AlarmGetNext(this, ((timestamp) % 86400));

    if (record.ID != 0) {
        alarmIsValid   = true;
        alarmTimestamp = record.time;
        alarmID        = record.ID;
    }
    else {
        alarmDBEmpty = true;
    }
}
