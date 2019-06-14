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

#include "keyboard/keyboard.hpp"
#include "module-bsp/bsp/keyboard/key_codes.hpp"
//#include "module-bsp/board/rt1051/keyboard/rt1051_keyboard.hpp"
#include "EventManager.hpp"

#include <iostream>


static void keyboardTaskPressTimerCallback(TimerHandle_t xTimer)
{

	xQueueHandle qhandle = reinterpret_cast<xQueueHandle> (pvTimerGetTimerID( xTimer ));

	bool handled = true;

	if( qhandle != NULL)
		xQueueSend( qhandle, &handled, 1000);

}


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
	if( queueID == static_cast<uint32_t>(WorkerEventQueues::queueKeyboardTimer) )
	{
		bool received;
		xQueueReceive(queue, &received, 0);

		auto message = std::make_shared<KbdMessage>(MessageType::KBDKeyEvent);
		message->keyCode = lastPressed;
		message->keyRelaseTime = xTaskGetTickCount();
		message->keyState = KeyboardEvents::keyReleasedLong;
		sys::Bus::SendUnicast(message, "EventManager", this->service);

		lastState = bsp::KeyEvents::Released;
	}
	if( queueID == static_cast<uint32_t>(WorkerEventQueues::queueKeyboardEvent) )
	{
		KeyState val;
		if( xQueueReceive(queue, &val, 0 ) != pdTRUE ) {
					return false;
			}

		std::map<uint32_t, uint32_t>::iterator it = longPressParamsList.find(static_cast<int>(val.code));
		if( (it != longPressParamsList.end()) && (val.event == static_cast<uint8_t>(bsp::KeyEvents::Pressed)) )
		{
		longPressTimerStart(it->second);
		}

		keyboardEventCallback(static_cast<bsp::KeyEvents>(val.event), static_cast<bsp::KeyCodes>(val.code));
	}
	if( queueID == 3 )
	{
		uint8_t notification;
		if( xQueueReceive(queue, &notification, 0 ) != pdTRUE ) {
				return false;
		}
		KeyState val;
	//	bsp::keyboar_worker(notification, val);
		LOG_INFO("[EventManager] NOTIFY!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ") ;

		std::map<uint32_t, uint32_t>::iterator it = longPressParamsList.find(static_cast<int>(val.code));
			if( (it != longPressParamsList.end()) && (val.event == static_cast<uint8_t>(bsp::KeyEvents::Pressed)) )
			{
			longPressTimerStart(it->second);
			}
		keyboardEventCallback(static_cast<bsp::KeyEvents>(val.event), static_cast<bsp::KeyCodes>(val.code));

	}
	return true;
}

bool WorkerEvent::init( std::list<sys::WorkerQueueInfo> queues )
{
	std::pair<uint32_t, uint32_t> longPresKeyLeft = {static_cast<uint32_t>(bsp::KeyCodes::JoystickLeft), 500};

	longPressParamsList.insert(longPresKeyLeft);

	Worker::init(queues);
	//initialize keyboard
	bsp::keyboard keyboard;
	keyboard.Init(this);

	return true;
}
bool WorkerEvent::deinit(void)
{
	Worker::deinit();

	return true;
}



 void WorkerEvent::keyboardEventCallback(bsp::KeyEvents event, bsp::KeyCodes code)
 {
	 auto message = std::make_shared<KbdMessage>(MessageType::KBDKeyEvent);

	message->keyCode = code;

	if(event == bsp::KeyEvents::Pressed)
	{
		if(lastState == bsp::KeyEvents::Pressed)
		{
			return;
		}


		message->keyState =  KeyboardEvents::keyPressed;
		message->keyPressTime = xTaskGetTickCount();
		message->keyRelaseTime = 0;

		// Slider sends only press, not release state so it would block the entire keyboard
		if( (code != bsp::KeyCodes::SSwitchUp) && (code != bsp::KeyCodes::SSwitchMid) && (code != bsp::KeyCodes::SSwitchDown) )
		{
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
		// If timer not yet triggered - the user pressed the button shortly
		if( longPressTaskEnabled )
		{
			if(lastPressed == code)
			{
				if( xTimerIsTimerActive(longPressTimerHandle) )
				{
					xTimerStop(longPressTimerHandle, 0);

					message->keyState =  KeyboardEvents::keyReleasedShort;
					message->keyCode = code;
					message->keyRelaseTime = xTaskGetTickCount();
					longPressTaskEnabled = false;
				}
				else
				{
					longPressTaskEnabled = false;
					//message is currently send
					return;
				}
			}
			else
			{
				message->keyState =  KeyboardEvents::keyReleasedShort;
				message->keyRelaseTime = xTaskGetTickCount();
			}
		}
		else
		{
			message->keyState =  KeyboardEvents::keyReleasedShort;
			message->keyRelaseTime = xTaskGetTickCount();
		}
	}
	sys::Bus::SendUnicast(message, "EventManager", this->service);
 }


 bool WorkerEvent::longPressTimerStart(uint32_t time)
 {
	 //TODO change magic number to enum
	longPressTimerHandle = xTimerCreate("keyboardPressTimer", time, false,  queues[1], &keyboardTaskPressTimerCallback);
	longPressTaskEnabled = true;
	if( xTimerStart( longPressTimerHandle, 0 ) != pdPASS )
	{
		return false;
	}
	return true;
 }
