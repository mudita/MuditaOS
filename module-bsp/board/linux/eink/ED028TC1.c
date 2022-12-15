// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

/**
 * @brief EInk ED028TC1 electronic paper display driver
 * @details This is hardware specific electronic paper display ED028TC1 driver.
 *
 * @note All the commands implemented in here are based on the datasheets:
 *              * ED028TC1 Product Preliminary Spec sheet v0.4 20171228.pdf
 *              * UC8177c.pdf
 */

#include "ED028TC1.h"
#include "board.h"

#include <stdbool.h>
#include <assert.h>
#include <math.h>
#include <string.h>

#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <spawn.h>

#include "macros.h"

// header of the eink's shared memory frame buffer.
typedef struct
{
    uint32_t frameCount;
    uint32_t width;
    uint32_t height;
} shared_memory_header;
// pointer to the header in the shared memory
static shared_memory_header *shared_header = NULL;
// pointe to the buffer ( located in memory right after shared header )
static uint8_t *shared_buffer = NULL;
// pointer to the buffer that simulates internal einks buffer. Image from the eink service is copied to first to this
// buffer durinig update operation. When refresh is called image from this buffer is copied to the shared memory.
static uint8_t *eink_internal_buffer = NULL;
// size of the buffer in bytes
static int shared_buffer_size = BOARD_EINK_DISPLAY_RES_X * BOARD_EINK_DISPLAY_RES_Y;

int shared_fd = 0;

static uint8_t s_einkIsPoweredOn = false; //  Variable which contains the state of the power of the EPD display

uint8_t EinkIsPoweredOn()
{
    return s_einkIsPoweredOn;
}

void EinkPowerOn()
{
    s_einkIsPoweredOn = true;
}

void EinkPowerOff()
{
    s_einkIsPoweredOn = false;
}

void EinkPowerDown(void)
{
    EinkPowerOff();
}

static shared_memory_header *createSHMBuffer(const char *name)
{
    if (shared_header != NULL)
        return shared_header;

    // check if shared memory blok is already created
    if ((shared_fd = shm_open(name, O_RDWR | O_CREAT, 0660)) == -1) {
        printf("shm is already created");
    }
    else {
        if (ftruncate(shared_fd, sizeof(shared_memory_header) + shared_buffer_size) == -1) {
            printf("shm is already created");
        }
    }
    if ((shared_header = mmap(NULL,
                              sizeof(shared_memory_header) + shared_buffer_size,
                              PROT_READ | PROT_WRITE,
                              MAP_SHARED,
                              shared_fd,
                              0)) == MAP_FAILED) {
        printf("mmap failed");
    }

    shared_buffer        = ((uint8_t *)shared_header) + sizeof(shared_memory_header);
    eink_internal_buffer = shared_buffer + shared_buffer_size;

    return shared_header;
}

EinkStatus_e EinkResetAndInitialize()
{
    s_einkIsPoweredOn = false;

    // create and map shared memory
    char *shared_name = "pure_gui_fmbuf";
    if (createSHMBuffer(shared_name) == NULL) {
        return EinkError;
    }

    pid_t pid;
    if (posix_spawn(&pid, "./service_renderer", NULL, NULL, NULL, NULL) != 0) {
        fprintf(stderr,"Could not start service_renderer");
        return EinkError;
    }
    return EinkOK;
}

EinkStatus_e EinkUpdateFrame(EinkFrame_t frame, const uint8_t *buffer)
{
    uint32_t offset_eink   = frame.pos_y * BOARD_EINK_DISPLAY_RES_X + frame.pos_x;
    uint32_t offset_buffer = 0;
    for (uint32_t h = 0; h < frame.height; ++h) {
        memcpy(shared_buffer + offset_eink, buffer + offset_buffer, frame.width);
        offset_eink += BOARD_EINK_DISPLAY_RES_X;
        offset_buffer += frame.width;
    }

    shared_header->frameCount++;
    return EinkOK;
}

EinkStatus_e EinkFillScreenWithColor(EinkDisplayColorFilling_e colorFill)
{
    if (shared_buffer && shared_header) {
        memset(shared_buffer, colorFill, shared_buffer_size);
        return EinkOK;
    }

    return EinkError;
}

EinkStatus_e EinkRefreshImage(EinkFrame_t frame)
{
    return EinkOK;
}

__attribute__((optimize("O3"))) void EinkARGBToLuminance(uint8_t *dataIn,
                                                         uint8_t *dataOut,
                                                         uint32_t displayWidth,
                                                         uint32_t displayHeight)
{
    uint8_t r, g, b;
    float fi;
    uint32_t *src;
    uint8_t *dst;

    src = (uint32_t *)dataIn;
    dst = (uint8_t *)dataOut;

    for (uint32_t i = 0; i < (displayWidth * displayHeight);
         i += 2) // increase by 8 pixels - 32bit word is 8 pixels in 4BPP
    {
        *dst = 0x00000000;
        for (uint8_t j = 0; j < 8; j += 4) {
            r  = (uint8_t)((*(src)) >> 16);
            g  = (uint8_t)((*(src)) >> 8);
            b  = (uint8_t) * (src);
            fi = (r + g + b) / 3;

            *dst |= ((uint32_t)(floor(fi / 16))) << (4 - j);

            src++;
        }
        dst++;
    }
}

EinkStatus_e EinkMemcpyDmaInit(edma_callback memcpyCallback)
{
    return EinkOK;
}
