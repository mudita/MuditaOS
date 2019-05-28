/*
 * linux_keyboard.cpp
 *
 *  Created on: May 27, 2019
 *      Author: kuba
 */

#include "linux_keyboard.hpp"
#include "key_codes_linux.hpp"

#include <iostream>

#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#include <sys/stat.h>
#include <fcntl.h>

//#include "../board.h"

namespace bsp {

static TaskHandle_t linux_keyboard_worker_handle = NULL;
static std::function<void(KeyEvents event,KeyCodes code)> user_event_callback = NULL;

static void linux_keyboard_worker(void *pvp)
{
	int fd;
	struct stat status;
	char * myfifo = "/tmp/pure_fifo";

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
			KeyEvents event;
			KeyCodes code;

			event = static_cast<KeyEvents>(buff[0]);
			code = static_cast<KeyCodes>(buff[1]);

			if(buff[2] == buff[0] + buff[1])
				user_event_callback(event, code);
		}
		vTaskDelay(50);
	}

	close(fd);
}

void linux_keyboard_event_callback(KeyEvents event,KeyCodes code)
{

}
RetCode linux_keyboard_Init(std::function<void(bsp::KeyEvents event,bsp::KeyCodes code)> event) {

	// Store user specified event callback
	user_event_callback = event;

	if (xTaskCreate(linux_keyboard_worker, "keyboard", 512, NULL, 0, &linux_keyboard_worker_handle) != pdPASS) {
		return RetCode::Failure;
	}
	return RetCode::Success;
}

}


