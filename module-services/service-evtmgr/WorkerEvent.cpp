/*
 * WorkerEvent.cpp
 *
 *  Created on: May 31, 2019
 *      Author: kuba
 */

#include <string.h>

extern "C" {
	#include "FreeRTOS.h"
	#include "task.h"
}

#include "Service/Service.hpp"
#include "Service/Message.hpp"
#include "Service/Worker.hpp"
#include "MessageType.hpp"



#include "WorkerEvent.hpp"
#include "EventManager.hpp"
#include "service-evtmgr/messages/EVMessages.hpp"

#include "bsp/battery-charger/battery_charger.hpp"
#include "bsp/cellular/bsp_cellular.hpp"
#include "bsp/keyboard/keyboard.hpp"
#include "bsp/rtc/rtc.hpp"

#include "bsp/harness/bsp_harness.hpp"
#include "harness/Parser.hpp"

bool WorkerEvent::handleMessage( uint32_t queueID ) {

	QueueHandle_t queue = queues[queueID];

	//service queue
	if( queueID == static_cast<uint32_t>(WorkerEventQueues::queueService) ) {
		sys::WorkerCommand wcmd;
		if( xQueueReceive(queue, &wcmd, 0 ) != pdTRUE ) {
			return false;
		}
		wcmd.command = 1;
		//place some code here to handle messages from service
	}

	if( queueID == static_cast<uint32_t>(WorkerEventQueues::queueKeyboardIRQ) )
	{
		uint8_t notification;
		if( xQueueReceive(queue, &notification, 0 ) != pdTRUE ) {
				return false;
		}
		uint8_t state, code;
		bsp::keyboard_get_data(notification, state, code);

		processKeyEvent(static_cast<bsp::KeyEvents>(state), static_cast<bsp::KeyCodes>(code));

	}

	if( queueID == static_cast<uint32_t>(WorkerEventQueues::queueBattery) )
	{
        uint8_t notification;
        if( xQueueReceive(queue, &notification, 0 ) != pdTRUE ) {
				return false;
		}
		if(notification & static_cast<uint8_t>(bsp::batteryIRQSource::INTB))
		{
            uint8_t battLevel = 0;
            bsp::battery_getBatteryLevel(battLevel);
            bsp::battery_ClearAllIRQs();
			auto message = std::make_shared<sevm::BatteryLevelMessage>(MessageType::EVMBatteryLevel);
			message->levelPercents = battLevel;
			message->fullyCharged = false;
			sys::Bus::SendUnicast(message, "EventManager", this->service);
		}
		if(notification & static_cast<uint8_t>(bsp::batteryIRQSource::INOKB))
		{
			bool status;
			bsp::battery_getChargeStatus(status);
			bsp::battery_ClearAllIRQs();
			auto message = std::make_shared<sevm::BatteryPlugMessage>(MessageType::EVMChargerPlugged);
			message->plugged = status;
            sys::Bus::SendUnicast(message, "EventManager", this->service);
        }
	}

	if( queueID == static_cast<uint32_t>(WorkerEventQueues::queueRTC) )
	{
		uint8_t notification;
		if( xQueueReceive(queue, &notification, 0 ) != pdTRUE ) {
				return false;
		}

		time_t timestamp;
		bsp::rtc_GetCurrentTimestamp(&timestamp);
		bsp::rtc_SetMinuteAlarm(timestamp);

		struct tm time;

		bsp::rtc_GetCurrentDateTime(&time);

		auto message = std::make_shared<sevm::RtcMinuteAlarmMessage>(MessageType::EVMMinuteUpdated);
		message->timestamp = timestamp;
		sys::Bus::SendUnicast(message, "EventManager", this->service);
	}

    if (queueID == static_cast<uint32_t>(WorkerEventQueues::queueHarness))
    {
        uint8_t notification;
        if (xQueueReceive(queue, &notification, 0) != pdTRUE)
        {
            return false;
        }
        if (notification == STX)
        {
            if (bsp::harness::flush() != true)
            {
                LOG_ERROR("processing in progres...");
            }
        }
        else if (notification == ETX)
        {
            std::string text = bsp::harness::read();
            auto ret = harness::parse(text);
            if (ret.first == harness::Error::Success)
            {
                // LOG_INFO("EVENT! %x: %s", notification, text.c_str());
                sys::Bus::SendUnicast(ret.second, "EventManager", this->service);
            }
            else
            {
                LOG_ERROR("Harness parser error: %d", ret.first);
            }
        }
        else
        {
            LOG_ERROR("Unknown event!");
        }
    }

    if (queueID == static_cast<uint32_t>(WorkerEventQueues::queueSIM))
    {
        uint8_t notification;
        if (xQueueReceive(queue, &notification, 0) != pdTRUE)
        {
            return false;
        }
        int sim1 = bsp::cellular::SIM1_Read();
        int sim2 = bsp::cellular::SIM2_Read();
        LOG_DEBUG("SIM state change: s1: %d, s2: %d notification: %d", sim1, sim2, notification);
        auto message = std::make_shared<sevm::SIMMessage>();
        sys::Bus::SendUnicast(message, "EventManager", this->service);
    }

    return true;
}

#include "harness/events/SysStart.hpp"

bool WorkerEvent::init( std::list<sys::WorkerQueueInfo> queues )
{
	Worker::init(queues);
	std::vector<xQueueHandle> qhandles = this->getQueues();
	bsp::keyboard_Init(qhandles[static_cast<int32_t>(WorkerEventQueues::queueKeyboardIRQ)]);
	bsp::battery_Init(qhandles[static_cast<int32_t>(WorkerEventQueues::queueBattery)]);
	bsp::rtc_Init(qhandles[static_cast<int32_t>(WorkerEventQueues::queueRTC)]);
    bsp::harness::Init(qhandles[static_cast<int32_t>(WorkerEventQueues::queueHarness)]);
    bsp::cellular::Init(qhandles[static_cast<int32_t>(WorkerEventQueues::queueSIM)]);

    time_t timestamp;
	bsp::rtc_GetCurrentTimestamp(&timestamp);
	bsp::rtc_SetMinuteAlarm(timestamp);
    bsp::harness::emit(harness::SysStart().encode());
    return true;
}

bool WorkerEvent::deinit(void)
{
	Worker::deinit();
	bsp::keyboard_Deinit();
	bsp::battery_Deinit();

	return true;
}

 void WorkerEvent::processKeyEvent(bsp::KeyEvents event, bsp::KeyCodes code)
 {
	auto message = std::make_shared<sevm::KbdMessage>(MessageType::KBDKeyEvent);

	message->key.key_code = code;

	if(event == bsp::KeyEvents::Pressed)
	{
		if(lastState == bsp::KeyEvents::Pressed) {
			return;
		}

		message->key.state =  RawKey::State::Pressed;
		message->key.time_press = xTaskGetTickCount();

		// Slider sends only press, not release state so it would block the entire keyboard
		if( (code != bsp::KeyCodes::SSwitchUp) && (code != bsp::KeyCodes::SSwitchMid) && (code != bsp::KeyCodes::SSwitchDown) ) {
			lastPressed = code;
			lastState = event;
		}
	}
	else
	{
		if( lastState != bsp::KeyEvents::Pressed)
		{
			return;
		}
		if( lastPressed != code)
		{
			return;
		}

		lastState = bsp::KeyEvents::Released;
		{
			message->key.state = RawKey::State::Released;
			message->key.time_release = xTaskGetTickCount();
		}
	}
	sys::Bus::SendUnicast(message, "EventManager", this->service);
 }

