/*
 * linux_keyboard.cpp
 *
 *  Created on: May 27, 2019
 *      Author: kuba
 */

#include "module-sys/Service/Message.hpp"
#include "service-kbd/ServiceKbd.hpp"
#include "service-kbd/WorkerKbd.hpp"
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

#include "bsp_keyboard.hpp"
#include "key_codes.hpp"

//#include "../board.h"

namespace bsp {

static TaskHandle_t linux_keyboard_worker_handle = NULL;
TimerHandle_t keyboardTaskPressTimer = NULL;
static std::function<void(xQueueHandle qhandle)>  user_event_callback = NULL;

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

		uint8_t buff [10];
		int32_t readedBytes = read(fd, buff, 10);

		if(readedBytes > 1)
		{

			bsp::KeyState key;
			key.event = static_cast<KeyEvents>(buff[0]);
			key.code = static_cast<KeyCodes>(buff[1]);

			//if(buff[2] == buff[0] + buff[1])
			xQueueHandle qhandle = reinterpret_cast<xQueueHandle>(pvp);

			xQueueSend(qhandle, &key, 100);
		}
		vTaskDelay(50);
	}

	close(fd);
}

RetCode linux_keyboard_Init(WorkerKbd* worker) {

	// Store user specified event callback
//	user_event_callback = event;
	std::vector<xQueueHandle> queues = worker->getQueues();
	//TODO change magic value to enum
	xQueueHandle qhandle = queues[2];
	if (xTaskCreate(linux_keyboard_worker, "keyboard", 512, qhandle, 0, &linux_keyboard_worker_handle) != pdPASS) {
		return RetCode::Failure;
	}

	 longPressTimerQueue = xQueueCreate(5, sizeof(bool));


	return RetCode::Success;
}
}




