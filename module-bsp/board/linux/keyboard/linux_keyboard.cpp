/*
 * linux_keyboard.cpp
 *
 *  Created on: May 27, 2019
 *      Author: kuba
 */

#include "linux_keyboard.hpp"
#include "key_codes_linux.hpp"
#include "module-sys/Service/Message.hpp"
#include "service-kbd/ServiceKbd.hpp"

#include <iostream>

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <memory>

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#include <sys/stat.h>
#include <fcntl.h>

//#include "../board.h"

namespace bsp {

static TaskHandle_t linux_keyboard_worker_handle = NULL;
TimerHandle_t keyboardTaskPressTimer = NULL;
static std::function<void(KeyEvents event,KeyCodes code, sys::Service* s)> user_event_callback = NULL;

struct key_t{
	KeyEvents lastState = KeyEvents::Released;
	KeyCodes lastPressed = static_cast<KeyCodes>(0);

	bool longPressTaskEnabled = false;
};
static struct key_t key;

bool logPressTimerCallbackHandled = false;

static QueueHandle_t longPressTimerQueue = NULL;

static void linux_keyboard_worker(void *pvp)
{
	int fd;
	struct stat status;
	char * myfifo = "/tmp/myfifo3";

	// Creating the named file(FIFO)
	// mkfifo(<pathname>, <permission>)
	mkfifo(myfifo, 0666);

	// Open FIFO for write only
	fd = open(myfifo, O_RDONLY | O_NONBLOCK );

	while(1)
	{
		bool callbackHandled = false;
		if( longPressTimerQueue != NULL)
			xQueueReceive( longPressTimerQueue, &callbackHandled, 50 );
		if(callbackHandled)
		{
			sys::Service* s = reinterpret_cast<sys::Service*>(pvp);
			user_event_callback(KeyEvents::Released, key.lastPressed, s );
		}
		uint8_t buff [10];
		int32_t readedBytes = read(fd, buff, 10);

		if(readedBytes > 1)
		{
			KeyEvents event;
			KeyCodes code;

			event = static_cast<KeyEvents>(buff[0]);
			code = static_cast<KeyCodes>(buff[1]);

			std::cout << "Event: "  << static_cast<int>(buff[0])<< " code: " << static_cast<int>(buff[1]) <<std::endl;
			//if(buff[2] == buff[0] + buff[1])
			sys::Service* s = reinterpret_cast<sys::Service*>(pvp);
				user_event_callback(event, code, s);
		}
		vTaskDelay(50);
	}

	close(fd);
}


static void keyboardTaskPressTimerCallback(TimerHandle_t xTimer)
{
	auto message = std::make_shared<KbdMessage>();
	std::cout << "Timer callback\n";

	bool handled = true;

	if( longPressTimerQueue != NULL)
		xQueueSend( longPressTimerQueue, &handled, 100);
/*	ServiceKbd* serv = reinterpret_cast<ServiceKbd*> (pvTimerGetTimerID( xTimer ));
	sys::Bus::SendUnicast(message, "ServiceKbd", serv);
	std::cout << "Timer callback\n";*/
}

void startKeyTimer(void)
{
	std::cout << "Timer start\n";

	keyboardTaskPressTimer = xTimerCreate("keyboardPressTimer", 1000, false,  ( void * ) 0, &keyboardTaskPressTimerCallback);
	if( xTimerStart( keyboardTaskPressTimer, 0 ) != pdPASS )
	{

	}
}

void linux_keyboard_event_callback(KeyEvents event,KeyCodes code, sys::Service* s)
{
	auto message = std::make_shared<KbdMessage>();

	message->keyCode = code;

	if(event == KeyEvents::Pressed)
	{
		if(key.lastState == KeyEvents::Pressed)
		{
			return;
		}


		message->keyState = event;
		message->keyPressTime = xTaskGetTickCount();
		message->keyRelaseTime = 0;

		// Slider sends only press, not release state so it would block the entire keyboard
		if( (code != KeyCodes::SSwitchUp) && (code != KeyCodes::SSwitchMid) && (code != KeyCodes::SSwitchDown) )
		{
			key.lastPressed = code;
			key.lastState = event;
			/*TEST*/
			if(code == KeyCodes::JoystickLeft)
			{
				key.longPressTaskEnabled = true;
				startKeyTimer();
			}
			else
				key.longPressTaskEnabled = false;
			/*TEST*/

		}
	}
	else
	{
		if( key.lastState != KeyEvents::Pressed)
		{
			return;
		}
		if( key.lastPressed != code)
		{
			return;
		}

		key.lastState = KeyEvents::Released;
		// If timer not yet triggered - the user pressed the button shortly
		if( key.longPressTaskEnabled )
		{
			if(key.lastPressed == code)
			{
			if( xTimerIsTimerActive(keyboardTaskPressTimer) )
				{
					xTimerStop(keyboardTaskPressTimer, 0);

					message->keyState = KeyEvents::ReleasedShort;
					message->keyCode = code;
					message->keyRelaseTime = xTaskGetTickCount();
				}
				//TODO clear last pressed key after long press timer callback
				else
				{
					key.longPressTaskEnabled = false;
					message->keyState = KeyEvents::ReleasedLong;
					message->keyCode = code;
					message->keyRelaseTime = xTaskGetTickCount();
				}
			}
			else
			{
				message->keyState = KeyEvents::ReleasedShort;
				message->keyRelaseTime = xTaskGetTickCount();
			}
		}
		else
		{
			message->keyState = KeyEvents::ReleasedShort;
			message->keyRelaseTime = xTaskGetTickCount();
		}
	}

	sys::Bus::SendUnicast(message, "ServiceKbd", s);
}
RetCode linux_keyboard_Init(std::function<void(bsp::KeyEvents event,bsp::KeyCodes code, sys::Service* s)> event, sys::Service* s) {

	// Store user specified event callback
	user_event_callback = event;

	if (xTaskCreate(linux_keyboard_worker, "keyboard", 512, s, 0, &linux_keyboard_worker_handle) != pdPASS) {
		return RetCode::Failure;
	}

	 longPressTimerQueue = xQueueCreate(5, sizeof(bool));


	return RetCode::Success;
}

}


