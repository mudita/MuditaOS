/*
 * EventManagerAlarm.cpp
 *
 *  Created on: 18 lip 2019
 *      Author: kuba
 */

#include "../EventManager.hpp"

#include "segger/log/log.hpp"

#include "bsp/keyboard/keyboard.hpp"
#include "../WorkerEvent.hpp"
#include "../messages/EVMessages.hpp"

#include "vfs.hpp"

#include "service-db/api/DBServiceAPI.hpp"
#include "service-appmgr/ApplicationManager.hpp"

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
            sapm::ApplicationManager::messageSwitchApplication(this, "bell", "main", std::move(switchMessage));
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
