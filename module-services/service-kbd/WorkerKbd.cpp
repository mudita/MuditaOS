/*
 * WorkerKbd.cpp
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
#include "WorkerKbd.hpp"
#include "keyboard/keyboard.hpp"
#include "module-bsp/bsp/keyboard/key_codes.hpp"


static void keyboardTaskPressTimerCallback(TimerHandle_t xTimer)
{

	xQueueHandle qhandle = reinterpret_cast<xQueueHandle> (pvTimerGetTimerID( xTimer ));

	bool handled = true;

	if( qhandle != NULL)
		xQueueSend( qhandle, &handled, 1000);

}


bool WorkerKbd::handleMessage( uint32_t queueID ) {

	QueueHandle_t queue = queues[queueID];

	//service queue
	if( queueID == 0 ) {
		sys::WorkerCommand wcmd;
		if( xQueueReceive(queue, &wcmd, 0 ) != pdTRUE ) {
			return false;
		}
		wcmd.command = 1;
		//place some code here to handle messages from service

	}
	if( queueID == 1)
	{
		bool received;
		xQueueReceive(queue, &received, 0);

		auto message = std::make_shared<KbdMessage>();
		message->keyCode = lastPressed;
		message->keyRelaseTime = xTaskGetTickCount();
		message->keyState = bsp::KeyEvents::ReleasedLong;
		sys::Bus::SendUnicast(message, "ServiceKbd", this->service);

		lastState = bsp::KeyEvents::Released;
	}
	if( queueID == 2)
	{
		KeyState val;
		if( xQueueReceive(queue, &val, 0 ) != pdTRUE ) {
					return false;
			}

		std::map<uint32_t, uint32_t>::iterator it = longPressParamsList.find(static_cast<int>(val.code));
		if( (it != longPressParamsList.end()) && (val.event == bsp::KeyEvents::Pressed) )
		{
		longPressTimerStart(it->second);
		}

		keyboardEventCallback(val.event, val.code);
	}
	return true;
}

bool WorkerKbd::init( std::list<sys::WorkerQueueInfo> queues )
{
	std::pair<uint32_t, uint32_t> longPresKeyLeft = {static_cast<uint32_t>(bsp::KeyCodes::JoystickLeft), 500};

	longPressParamsList.insert(longPresKeyLeft);

	Worker::init(queues);
	//initialize keyboard
	bsp::keyboard keyboard;
	keyboard.Init(this);

	return true;
}
bool WorkerKbd::deinit(void)
{
	Worker::deinit();

	return true;
}



 void WorkerKbd::keyboardEventCallback(bsp::KeyEvents event, bsp::KeyCodes code)
 {
	 auto message = std::make_shared<KbdMessage>();

	message->keyCode = code;

	if(event == bsp::KeyEvents::Pressed)
	{
		if(lastState == bsp::KeyEvents::Pressed)
		{
			return;
		}


		message->keyState = event;
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

					message->keyState = bsp::KeyEvents::ReleasedShort;
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
				message->keyState = bsp::KeyEvents::ReleasedShort;
				message->keyRelaseTime = xTaskGetTickCount();
			}
		}
		else
		{
			message->keyState = bsp::KeyEvents::ReleasedShort;
			message->keyRelaseTime = xTaskGetTickCount();
		}
	}
	sys::Bus::SendUnicast(message, "ServiceKbd", this->service);
 }


 bool WorkerKbd::longPressTimerStart(uint32_t time)
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
