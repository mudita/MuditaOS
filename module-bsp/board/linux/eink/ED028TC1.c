/**
 * @file ED028TC1.c
 * @author Lukasz Skrzypczak (l.skrzypczak@mudita.com)
 * @date Sep 6, 2017
 * @brief EInk ED028TC1 electronic paper display driver
 * @copyright Copyright (C) 2017 mudita.com.
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

#include "macros.h"

//header of the eink's shared memory frame buffer.
typedef struct {
	uint32_t frameCount;
	uint32_t width;
	uint32_t height;
}shared_memory_header;
//pointer to the header in the shared memory
static shared_memory_header* shared_header = NULL;
//pointe to the buffer ( located in memory right after shared header )
static uint8_t* shared_buffer = NULL;
//pointer to the buffer that simulates internal einks buffer. Image from the eink service is copied to first to this buffer
//durinig update operation. When refresh is called image from this buffer is copied to the shared memory.
static uint8_t* eink_internal_buffer = NULL;
//size of the buffer in bytes
static int shared_buffer_size = BOARD_EINK_DISPLAY_RES_X*BOARD_EINK_DISPLAY_RES_Y;

int shared_fd = 0;

static uint8_t              s_einkIsPoweredOn = false;  //  Variable which contains the state of the power of the EPD display


/* Function bodies */
void EinkChangeDisplayUpdateTimings(EinkDisplayTimingsMode_e timingsMode)
{
}

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

int16_t EinkGetTemperatureInternal() {
	return 25;
}


static shared_memory_header* createSHMBuffer( const char* name )
{
	if( shared_header != NULL )
		return shared_header;

	//check if shared memory blok is already created
	if ((shared_fd = shm_open ( name, O_RDWR | O_CREAT , 0660)) == -1) {
//		std::cerr<<"shm is already created"<<std::endl;
	}
	else {
		if (ftruncate (shared_fd, sizeof (shared_memory_header)+2*shared_buffer_size ) == -1) {
//			std::cerr<<"shm is already created"<<std::endl;
		}
	}
	if ((shared_buffer =
		mmap (NULL, sizeof (shared_memory_header)+2*shared_buffer_size, PROT_READ | PROT_WRITE, MAP_SHARED, shared_fd, 0)) == MAP_FAILED) {
//		std::cerr<<"mmap failed"<<std::endl;
	}

	shared_buffer = ((uint8_t*)shared_header) + sizeof( shared_memory_header );
	eink_internal_buffer = shared_buffer + shared_buffer_size;

	return shared_header;
}

//on linux this function is responsible for creating/opeingin shared memory region where image from eink task will be copied
EinkStatus_e EinkResetAndInitialize()
{

	s_einkIsPoweredOn = false;

	//create and map shared memory
	char* shared_name = "pure_gui_fmbuf";
	if( createSHMBuffer( shared_name ) == NULL ){
		return EinkError;
	}

	//start renderer application
	system( "./gui_renderer &" );

    return EinkOK;
}

EinkStatus_e EinkUpdateWaveform( const EinkWaveFormSettings_t* settings )
{
    return EinkOK;
}

EinkStatus_e
EinkWaitTillPipelineBusy()
{
    return EinkOK;
}

EinkStatus_e
EinkDitherDisplay()
{
    return EinkOK;
}

EinkStatus_e
EinkUpdateFrame ( uint16_t X,
                  uint16_t Y,
                  uint16_t W,
                  uint16_t H,
                  uint8_t *buffer,
                  EinkBpp_e bpp,
                  EinkDisplayColorMode_e invertColors)
{
//    uint8_t buf[10];
//    uint8_t pixelsInByte = 8/bpp;
//
//    s_einkServiceRotatedBuf[0] = EinkDataStartTransmission1;
//    s_einkServiceRotatedBuf[1] = bpp - 1;    //  0 - 1Bpp, 1 - 2Bpp, 2 - 3Bpp, 3 - 4Bpp
//
//    if ((s_einkConfiguredWaveform == EinkWaveformA2) ||
//        (s_einkConfiguredWaveform == EinkWaveformDU2))
//    {
//        switch (bpp)
//        {
//            case Eink1Bpp:
//            {
//                s_EinkTransformAnimationFrameCoordinateSystem_1Bpp(buffer, W, H, s_einkServiceRotatedBuf + 2, invertColors);
//            }break;
//
//            case Eink2Bpp:
//            {
//                s_EinkTransformAnimationFrameCoordinateSystem_2Bpp(buffer, W, H, s_einkServiceRotatedBuf + 2, invertColors);
//            }break;
//
//            case Eink3Bpp:
//            {
//                s_EinkTransformAnimationFrameCoordinateSystem_3Bpp(buffer, W, H, s_einkServiceRotatedBuf + 2, invertColors);
//            }break;
//
//            case Eink4Bpp:
//            {
//                s_EinkTransformAnimationFrameCoordinateSystem_4Bpp(buffer, W, H, s_einkServiceRotatedBuf + 2, invertColors);
//            }break;
//        }
//    }
//    else
//    {
//        switch (bpp)
//        {
//            case Eink1Bpp:
//            {
//                s_EinkTransformFrameCoordinateSystem_1Bpp(buffer, W, H, s_einkServiceRotatedBuf + 2, invertColors);
//            }break;
//
//            case Eink2Bpp:
//            {
//                s_EinkTransformFrameCoordinateSystem_2Bpp(buffer, W, H, s_einkServiceRotatedBuf + 2, invertColors);
//            }break;
//
//            case Eink3Bpp:
//            {
//                s_EinkTransformFrameCoordinateSystem_3Bpp(buffer, W, H, s_einkServiceRotatedBuf + 2, invertColors);
//            }break;
//
//            case Eink4Bpp:
//            {
//                s_EinkTransformFrameCoordinateSystem_4Bpp(buffer, W, H, s_einkServiceRotatedBuf + 2, invertColors);
//            }break;
//        }
//    }
//
//    buf[0] = EinkDataStartTransmissionWindow;		        //set display window
//    buf[1] = (uint8_t) ((BOARD_EINK_DISPLAY_RES_Y - Y - H) >> 8);      //MSB of the X axis in the EPD display. Value converted from the standard GUI coords system to the ED028TC1 one
//    buf[2] = (uint8_t) BOARD_EINK_DISPLAY_RES_Y - Y - H;			    //LSB of the X axis in the EPD display. Value converted from the standard GUI coords system to the ED028TC1 one
//    buf[3] = (uint8_t) ((BOARD_EINK_DISPLAY_RES_X - X - W) >> 8);      //MSB of the Y axis in the EPD display. Value converted from the standard GUI coords system to the ED028TC1 one
//    buf[4] = (uint8_t) BOARD_EINK_DISPLAY_RES_X - X - W;			    //LSB of the Y axis in the EPD display. Value converted from the standard GUI coords system to the ED028TC1 one
//    buf[5] = (uint8_t) (H >> 8);                            //MSB of the window height in the EPD display. Value converted from the standard GUI coords system to the ED028TC1 one
//    buf[6] = (uint8_t) H;			                        //LSB of the window height in the EPD display. Value converted from the standard GUI coords system to the ED028TC1 one
//    buf[7] = (uint8_t) (W >> 8);                            //MSB of the window width in the EPD display. Value converted from the standard GUI coords system to the ED028TC1 one
//    buf[8] = (uint8_t) W;			                        //LSB of the window width in the EPD display. Value converted from the standard GUI coords system to the ED028TC1 one
//
//    if (BSP_EinkWriteData(buf, 9, SPI_AUTOMATIC_CS) != 0)
//    {
////        LOG_ERROR("Eink: transmitting the display update header FAILED");
//        EinkResetAndInitialize();
//        return EinkSPIErr;
//    }
//
//    uint32_t msgSize = 2 + ((uint32_t)W * (uint32_t)H / pixelsInByte); // command (1 byte) + bpp (1 byte) + dataSize(W*H/pixelsInByte bytes)
//    // Send the part of the image to the display memory
//
//
//    if (BSP_EinkWriteData(s_einkServiceRotatedBuf, msgSize, SPI_AUTOMATIC_CS) != 0)
//    {
////        LOG_ERROR("Eink: transmitting the display update image FAILED");
//        EinkResetAndInitialize();
//        return EinkSPIErr;
//    }

    return EinkOK;
}

//EinkStatus_e
//EinkClearScreenDeep (int8_t temperature)
//{
//    EinkWaveforms_e wv = s_einkConfiguredWaveform;
//
//    EinkUpdateWaveform(EinkWaveformA2, temperature);
//    EinkFillScreenWithColor(EinkDisplayColorWhite);
//    EinkFillScreenWithColor(EinkDisplayColorBlack);
//    EinkFillScreenWithColor(EinkDisplayColorWhite);
//    EinkFillScreenWithColor(EinkDisplayColorBlack);
//    EinkFillScreenWithColor(EinkDisplayColorWhite);
//
//    EinkUpdateWaveform(wv, temperature);
//
//    return EinkOK;
//}

EinkStatus_e
EinkFillScreenWithColor (EinkDisplayColorFilling_e colorFill)
{
    if( shared_buffer && shared_header ) {
    	memset(shared_buffer, colorFill, shared_header->width * shared_header->height );
    	return EinkOK;
    }

    return EinkError;
}

EinkStatus_e
EinkRefreshImage (uint16_t X, uint16_t Y, uint16_t W, uint16_t H, EinkDisplayTimingsMode_e refreshTimingsMode)
{
//    EinkChangeDisplayUpdateTimings(refreshTimingsMode);
//
//    s_EinkSetGateOrder();
//
//    uint8_t buf[10];
//
//    buf[0] = EinkDisplayRefresh;
//    buf[1] = UPD_CPY_TO_PRE;
//
//    buf[2] = (uint8_t) ((BOARD_EINK_DISPLAY_RES_Y - Y - H) >> 8);      //MSB of the X axis in the EPD display. Value converted from the standard GUI coords system to the ED028TC1 one
//    buf[3] = (uint8_t) BOARD_EINK_DISPLAY_RES_Y - Y - H;               //LSB of the X axis in the EPD display. Value converted from the standard GUI coords system to the ED028TC1 one
//    buf[4] = (uint8_t) ((BOARD_EINK_DISPLAY_RES_X - X - W) >> 8);      //MSB of the Y axis in the EPD display. Value converted from the standard GUI coords system to the ED028TC1 one
//    buf[5] = (uint8_t) BOARD_EINK_DISPLAY_RES_X - X - W;               //LSB of the Y axis in the EPD display. Value converted from the standard GUI coords system to the ED028TC1 one
//    buf[6] = (uint8_t) (H >> 8);                            //MSB of the window height in the EPD display. Value converted from the standard GUI coords system to the ED028TC1 one
//    buf[7] = (uint8_t) H;                                   //LSB of the window height in the EPD display. Value converted from the standard GUI coords system to the ED028TC1 one
//    buf[8] = (uint8_t) (W >> 8);                            //MSB of the window width in the EPD display. Value converted from the standard GUI coords system to the ED028TC1 one
//    buf[9] = (uint8_t) W;                                   //LSB of the window width in the EPD display. Value converted from the standard GUI coords system to the ED028TC1 one
//
//    if (BSP_EinkWriteData(buf, sizeof(buf), SPI_AUTOMATIC_CS) != 0)
//    {
////        LOG_ERROR("Eink: transmitting the refresh request image FAILED");
//        EinkResetAndInitialize();
//        return EinkSPIErr;
//    }
//
//    EINK_SUSPEND_TASK_TILL_EPD_BUSY();

    return EinkOK;
}

__attribute__((optimize("O3")))
void EinkARGBToLuminance(uint8_t *dataIn, uint8_t* dataOut, uint32_t displayWidth, uint32_t displayHeight)
{
//    uint32_t i, j;
    uint8_t r, g, b;
    float fi;
    uint32_t *src;
    uint8_t *dst;

    src = (uint32_t*)dataIn;
    dst = (uint8_t*)dataOut;

    for (uint32_t i=0; i<(displayWidth*displayHeight); i+=2)   //increase by 8 pixels - 32bit word is 8 pixels in 4BPP
    {
        *dst = 0x00000000;
        for (uint8_t j=0; j<8; j+=4)
        {
            r = (uint8_t)((*(src)) >> 16);
            g = (uint8_t)((*(src)) >> 8);
            b = (uint8_t)*(src);
            fi = (r + g + b)/3;

            *dst |= ((uint32_t)(floor(fi / 16))) << (4 - j);

            src++;
        }
        dst++;
    }
}

EinkStatus_e EinkMemcpyDmaInit( edma_callback memcpyCallback ) {
	return EinkOK;
}
