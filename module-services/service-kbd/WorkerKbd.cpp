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
#include "linux_keyboard.hpp"


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
		std::cout << "Timer callback przechwycony\n";
	}
	if( queueID == 2)
	{
		bsp::KeyState val;
		if( xQueueReceive(queue, &val, 0 ) != pdTRUE ) {
					return false;
			}
		//TODO check if long press is enabled for pressed key
		if(val.code == bsp::KeyCodes::JoystickLeft)
		{
			//TODO set long press timer timeout
			//TODO magic number to enum
			bsp::startKeyTimer(1000, queues[1]);
		}

		linux_keyboard_event_callback(val.event, val.code, this->service);
	}
	return true;
}

bool WorkerKbd::init( std::list<sys::WorkerQueueInfo> queues )
{


	Worker::init(queues);
	//initialize keyboard
	bsp::keyboard keyboard;
	keyboard.Init(this);

}
bool WorkerKbd::deinit(void)
{
	Worker::deinit();
}


