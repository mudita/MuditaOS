/*
 * linux_keyboard.cpp
 *
 *  Created on: May 27, 2019
 *      Author: kuba
 */

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
#include "module-bsp/bsp/keyboard/key_codes.hpp"


namespace bsp {

static TaskHandle_t linux_keyboard_worker_handle = NULL;

static void linux_keyboard_worker(void *pvp)
{
	int fd;

	const char * myfifo = "/tmp/myfifo3";

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
			KeyState key;
			key.event = buff[0];
			key.code = buff[1];

			//if(buff[2] == buff[0] + buff[1])
			xQueueHandle qhandle = reinterpret_cast<xQueueHandle>(pvp);

			xQueueSend(qhandle, &key, 100);
		}
		vTaskDelay(50);
	}

	close(fd);
}

RetCode linux_keyboard_Init(WorkerEvent* worker) {

	std::vector<xQueueHandle> queues = worker->getQueues();
	xQueueHandle qhandle = queues[static_cast<uint32_t>(WorkerEventQueues::queueKeyboardEvent)];
	if (xTaskCreate(linux_keyboard_worker, "keyboard", 512, qhandle, 0, &linux_keyboard_worker_handle) != pdPASS) {
		return RetCode::Failure;
	}
	return RetCode::Success;
}
}




