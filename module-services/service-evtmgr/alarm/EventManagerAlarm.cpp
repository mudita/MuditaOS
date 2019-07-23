/*
 * EventManagerAlarm.cpp
 *
 *  Created on: 18 lip 2019
 *      Author: kuba
 */

#include "../EventManager.hpp"

#include "log/log.hpp"

#include "keyboard/keyboard.hpp"
#include "../WorkerEvent.hpp"
#include "../messages/EVMessages.hpp"

#include "vfs.hpp"

#include "service-db/api/DBServiceAPI.hpp"
#include "service-appmgr/ApplicationManager.hpp"

bool EventManager::GetAlarmDbEmpty(void)
{
	uint32_t dbSize = DBServiceAPI::AlarmGetCount(this);
	if(dbSize == 0)
		return true;
	else
		return false;
}

void EventManager::HandleAlarmTrigger(sys::DataMessage* msgl)
{
	LOG_INFO("HandleAlarmTrigger");
	sevm::RtcMinuteAlarmMessage* msg = reinterpret_cast<sevm::RtcMinuteAlarmMessage*>(msgl);

	// Send message to update time in application
	auto message = std::make_shared<sevm::RtcMinuteAlarmMessage>(MessageType::EVMMinuteUpdated);
	message->timestamp = msg->timestamp;

	if( targetApplication.empty() == false ){
		sys::Bus::SendUnicast(message, targetApplication, this);
	}

	time_t currentTime = message->timestamp;

	// it there is no valid alarm and DB is not empty
	if(alarmIsValid == false && alarmDBEmpty == false)
	{
		// get next alarm
		GetNextAlarmTimestamp(currentTime);
	}

	// there is valid alarm
	if(alarmIsValid == true)
	{
		//check if its time to trigger alarm
		//round time and compare to alaram timestamp
		if(alarmTimestamp == (currentTime % 86400))
		{
			// run bell application
			std::unique_ptr<gui::SwitchData> switchMessage = std::make_unique<sevm::EVMAlarmSwitchData>(alarmID);
			sapm::ApplicationManager::messageSwitchApplication(this, "bell", "main", std::move(switchMessage));
			LOG_INFO("ALARM!!!");
			alarmIsValid == false;
			//get next alarm
			//GetNextAlarmTimestamp( currentTime);
		}
		// check if alarm is not obsolete
		else if( (alarmTimestamp < (currentTime % 86400)) &&
				((currentTime + 60) % 86400) > (currentTime % 86400) )
		{
			alarmIsValid == false;
			//get next alarm
			//GetNextAlarmTimestamp( currentTime);
		}
	}
	//db upda

	LOG_INFO("alarmTimestamp: %d, currentTime : %d", alarmTimestamp, (currentTime % 86400));

}

void EventManager::GetNextAlarmTimestamp(time_t timestamp)
{
	AlarmsRecord record;
	if( ((timestamp + 60) % 86400) < (timestamp % 86400) )
	{
		LOG_INFO("Midnight!!");
		record = DBServiceAPI::AlarmGetNext(this, 0);
	}
	else
	{
		LOG_INFO("Normal update");
		record = DBServiceAPI::AlarmGetNext(this, ((timestamp + 60) % 86400));
	}

	if(record.ID != 0)
	{
		alarmIsValid = true;
		alarmUpdated = true;
		alarmTimestamp = record.time;
		alarmID = record.ID;
		LOG_INFO("Alarm updated time: %d", record.time);
	}
	else
	{
		LOG_INFO("No Alarm found");
		if(GetAlarmDbEmpty()){
			alarmDBEmpty = true;
			LOG_WARN("Alarm DB empty!!");
		}
	}
}
