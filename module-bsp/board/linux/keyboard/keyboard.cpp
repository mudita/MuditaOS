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
#include "queue.h"
#include "task.h"
#include "timers.h"

#include <sys/stat.h>
#include <fcntl.h>

#include "keyboard/keyboard.hpp"

namespace bsp {

	static TaskHandle_t linux_keyboard_worker_handle = NULL;

	static uint8_t kcode = 0;
	static uint8_t kevent = 0;

	static int fd;

	static void linux_keyboard_worker(void *pvp)
	{


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
				kevent = buff[0];
				kcode = buff[1];

				xQueueHandle qhandle = reinterpret_cast<xQueueHandle>(pvp);

				uint8_t notification = 0;
				if(kcode == static_cast<uint8_t>(bsp::KeyCodes::FnRight) )
				{
					if( kevent == static_cast<uint8_t>(bsp::KeyEvents::Pressed) )
						notification = 0x02;
					else
						notification = 0x04;
				}
				else
					notification = 0x01;
				xQueueSend(qhandle, &notification, 100);
			}
			vTaskDelay(50);
		}

		close(fd);
	}

	void keyboard_get_data(const uint8_t& notification, uint8_t& event, uint8_t& code)
	{
		event = kevent;
		code = kcode;
	}

	int32_t keyboard_Init(xQueueHandle qHandle){

		if (xTaskCreate(linux_keyboard_worker, "keyboard", 512, qHandle, 0, &linux_keyboard_worker_handle) != pdPASS) {
			return 1;
		}
		return 0;
	}

	int32_t keyboard_Deinit(void)
	{
		vTaskDelete(linux_keyboard_worker_handle);
		close(fd);
		return 0;
	}

}






