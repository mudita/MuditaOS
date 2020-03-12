/*
 * battery_charger.cpp
 *
 *  Created on: Jul 1, 2019
 *      Author: kuba
 */
#include <stdint.h>

extern "C"
{
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
}

#include <sys/stat.h>
#include <fcntl.h>

#include "board.h"
#include "bsp/battery-charger/battery_charger.hpp"

#define BSP_BATTERY_CHARGER_I2C_ADDR (0xD2 >> 1)
#define BSP_FUEL_GAUGE_I2C_ADDR      (0x6C >> 1)
#define BSP_TOP_CONTROLLER_I2C_ADDR  (0xCC >> 1)

static xQueueHandle qHandleIrq            = NULL;
static TaskHandle_t battery_worker_handle = NULL;

static uint8_t battLevel = 100;
static bool plugged      = false;
namespace bsp
{

    static void battery_worker(void *pvp);

    int battery_Init(xQueueHandle qHandle)
    {
        qHandleIrq = qHandle;
        if (xTaskCreate(battery_worker, "battery", 512, qHandle, 0, &battery_worker_handle) != pdPASS) {
            return 1;
        }
        return 0;
    }

    void battery_Deinit(void)
    {
        qHandleIrq = NULL;
        vTaskDelete(battery_worker_handle);
    }
    void battery_getBatteryLevel(uint8_t &levelPercent)
    {
        levelPercent = battLevel;
    }

    void battery_getChargeStatus(bool &status)
    {
        status = plugged;
    }

    // TODO function unused in linux driver, left for compatibility with target driver
    void battery_ClearAllIRQs(void)
    {}
    // TODO function unused in linux driver, left for compatibility with target driver
    void battery_clearFuelGuageIRQ(void)
    {}

    static void battery_worker(void *pvp)
    {

        const char *myfifo = "/tmp/fifoBattKeys";

        // Creating the named file(FIFO)
        // mkfifo(<pathname>, <permission>)
        mkfifo(myfifo, 0666);

        // Open FIFO for write only
        int fd;
        fd = open(myfifo, O_RDONLY | O_NONBLOCK);

        while (1) {
            uint8_t buff[10];
            int32_t readedBytes = read(fd, buff, 10);

            if (readedBytes > 0) {

                uint8_t notification = 0;
                switch (buff[0]) {
                case 'p':
                    notification = 0x02;
                    plugged      = 1 - plugged;
                    break;
                case ']':
                    notification = 0x01;
                    if (battLevel < 100)
                        battLevel++;
                    break;
                case '[':
                    notification = 0x01;
                    if (battLevel > 1)
                        battLevel--;
                    break;
                }
                xQueueSend(qHandleIrq, &notification, 100);
            }
            vTaskDelay(50);
        }
    }
} // namespace bsp
