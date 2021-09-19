// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "dma_config.h"
#include "fsl_dmamux.h"
#include "fsl_edma.h"
#include "ED028TC1.h"
#include "bsp_eink.h"
#include <board/board.h>

#include <stdbool.h>
#include <assert.h>
#include <math.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
//#include "fat_media.h"
//#include "vfs.h"

//#include "log.h"
#include <board/board.h>
#include "eink_binarization_luts.h"
#include <board/macros.h>

#include <magic_enum.hpp>
#include "drivers/pll/DriverPLL.hpp"
#include "drivers/dmamux/DriverDMAMux.hpp"
#include "drivers/dma/DriverDMA.hpp"
#include "board/BoardDefinitions.hpp"

#define EPD_BOOSTER_START_PERIOD_10MS 0
#define EPD_BOOSTER_START_PERIOD_20MS 1
#define EPD_BOOSTER_START_PERIOD_30MS 2
#define EPD_BOOSTER_START_PERIOD_40MS 3
#define EPD_BOOSTER_START_PERIOD_POS  6

#define EPD_BOOSTER_DRIVING_STRENGTH_1   0
#define EPD_BOOSTER_DRIVING_STRENGTH_2   1
#define EPD_BOOSTER_DRIVING_STRENGTH_3   2
#define EPD_BOOSTER_DRIVING_STRENGTH_4   3
#define EPD_BOOSTER_DRIVING_STRENGTH_5   4
#define EPD_BOOSTER_DRIVING_STRENGTH_6   5
#define EPD_BOOSTER_DRIVING_STRENGTH_7   6
#define EPD_BOOSTER_DRIVING_STRENGTH_8   7
#define EPD_BOOSTER_DRIVING_STRENGTH_POS 3

#define EPD_BOOSTER_OFF_TIME_GDR_0_27uS 0
#define EPD_BOOSTER_OFF_TIME_GDR_0_34uS 1
#define EPD_BOOSTER_OFF_TIME_GDR_0_40uS 2
#define EPD_BOOSTER_OFF_TIME_GDR_0_54uS 3
#define EPD_BOOSTER_OFF_TIME_GDR_0_80uS 4
#define EPD_BOOSTER_OFF_TIME_GDR_1_54uS 5
#define EPD_BOOSTER_OFF_TIME_GDR_3_34uS 6
#define EPD_BOOSTER_OFF_TIME_GDR_6_58uS 7
#define EPD_BOOSTER_OFF_TIME_GDR_POS    0

#if defined(EINK_ROTATE_90_CLOCKWISE)
#define EINK_DISPLAY_RES_X         (BOARD_EINK_DISPLAY_RES_Y)
#define EINK_DISPLAY_RES_Y         (BOARD_EINK_DISPLAY_RES_X)
#define EINK_DISPLAY_X_AXIS        (BOARD_EINK_DISPLAY_RES_Y - Y - H)
#define EINK_DISPLAY_Y_AXIS        (BOARD_EINK_DISPLAY_RES_X - X - W)
#define EINK_DISPLAY_WINDOW_WIDTH  (H)
#define EINK_DISPLAY_WINDOW_HEIGHT (W)
#else
#define EINK_DISPLAY_RES_X         (BOARD_EINK_DISPLAY_RES_X)
#define EINK_DISPLAY_RES_Y         (BOARD_EINK_DISPLAY_RES_Y)
#define EINK_DISPLAY_X_AXIS        (BOARD_EINK_DISPLAY_RES_X - X - W)
#define EINK_DISPLAY_Y_AXIS        (BOARD_EINK_DISPLAY_RES_Y - Y - H)
#define EINK_DISPLAY_WINDOW_WIDTH  (W)
#define EINK_DISPLAY_WINDOW_HEIGHT (H)
#endif

#define EINK_BLACK_PIXEL_MASK      0x00 // This is the mask for the black pixel value
#define EINK_1BPP_WHITE_PIXEL_MASK 0x01 // This is the mask for the white pixel in 1bpp mode
#define EINK_2BPP_WHITE_PIXEL_MASK 0x03 // This is the mask for the white pixel in 2bpp mode
#define EINK_4BPP_WHITE_PIXEL_MASK 0x0F // This is the mask for the white pixel in 4bpp mode

#define ED028TC1_BUSY_STATE_TIMEOUT_MS 2000 // Time after the display should for sure exit the busy state

#define _delay_ms(ms) vTaskDelay(pdMS_TO_TICKS(ms))

/// This is DMA handle for internal frame buffer memory-to-memory copying operation
static edma_handle_t s_einkMemcpyDma_handle __attribute__((used));

using namespace drivers;
using namespace magic_enum;
static std::shared_ptr<drivers::DriverDMA> dma;
static std::shared_ptr<drivers::DriverDMAMux> dmamux;

/* Internal variable definitions */
static uint8_t s_einkIsPoweredOn = false; //  Variable which contains the state of the power of the EPD display

static EinkWaveforms_e s_einkConfiguredWaveform =
    EinkWaveformGC16; //  This variable contains the current waveform set in the display

static CACHEABLE_SECTION_SDRAM(uint8_t s_einkServiceRotatedBuf[BOARD_EINK_DISPLAY_RES_X * BOARD_EINK_DISPLAY_RES_Y / 2 +
                                                               2]); // Plus 2 for the EPD command and BPP config

/**
 * @brief This lut is used for convertion of the 4bp input grayscale pixel to the 1bpp output pixel
 */
static uint8_t s_einkMaskLut_1Bpp[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1};

/**
 * @brief This lut is used for convertion of the 4bp input grayscale pixel to the 2bpp output pixel
 */
static uint8_t s_einkMaskLut_2Bpp[16] = {0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3};

/* External variable definitions */

/* Internal function prototypes */

/**
 *  This function makes rotation of the image from the standard GUI coordinate system to the coord system used by the
 * ED028TC1 display.
 *
 *  @note IT ROTATES only the 1Bpp image
 *
 *  @param uint8_t* dataIn          [in]  - input image to be translated. Each byte of that array must represent the
 * single pixel
 *  @param uint16_t x               [in]  - x coordinate of image in pixels
 *  @param uint16_t y               [in]  - y coordinate of image in pixels
 *  @param uint16_t windowWidthPx   [in]  - width of the image in pixels
 *  @param uint16_t windowHeightPx  [in]  - height of the image in pixels
 *  @param uint8_t* dataOut         [out] - the buffer for rotated image
 *  @param invertColors[in] - true if colors of the image are to be inverted, false otherwise
 *
 * @note Assumed dataIn coordinate system is the standard image coordinates system:
 *
 *   (0,0)   X
 *       *-------->
 *       |
 *     Y |
 *       |
 *       v
 *
 *
 * The result of the function is such a conversion of the dataIn array to make the Eink display show it the very same
 * way in its own coordinate system which is:
 *
 *              displayWidth
 *                 _______  ^
 *                |       | |
 *                |       | |
 *  displayHeight |       | |
 *                |       | |
 *                |_______| |  X
 *                   \ /    |
 *    signal tape -  | |    |
 *                          |
 *                 <--------*
 *                    Y     (0,0)
 *
 * @return
 */
static uint8_t *s_EinkTransformFrameCoordinateSystem_1Bpp(uint8_t *dataIn,
                                                          uint16_t windowWidthPx,
                                                          uint16_t windowHeightPx,
                                                          uint8_t *dataOut,
                                                          EinkDisplayColorMode_e invertColors);

/**
 *  This function makes rotation of the image from the standard GUI coordinate system to the coord system used by the
 * ED028TC1 display.
 *
 *  @note IT ROTATES only the 2Bpp image
 *
 *  @param uint8_t* dataIn          [in]  - input image to be translated. Each byte of that array must represent the
 * single pixel
 *  @param uint16_t x               [in]  - x coordinate of image in pixels
 *  @param uint16_t y               [in]  - y coordinate of image in pixels
 *  @param uint16_t windowWidthPx   [in]  - width of the image in pixels
 *  @param uint16_t windowHeightPx  [in]  - height of the image in pixels
 *  @param uint8_t* dataOut         [out] - the buffer for rotated image
 *  @param invertColors[in] - true if colors of the image are to be inverted, false otherwise
 *
 * @note Assumed dataIn coordinate system is the standard image coordinates system:
 *
 *   (0,0)   X
 *       *-------->
 *       |
 *     Y |
 *       |
 *       v
 *
 *
 * The result of the function is such a conversion of the dataIn array to make the Eink display show it the very same
 * way in its own coordinate system which is:
 *
 *              displayWidth
 *                 _______  ^
 *                |       | |
 *                |       | |
 *  displayHeight |       | |
 *                |       | |
 *                |_______| |  X
 *                   \ /    |
 *    signal tape -  | |    |
 *                          |
 *                 <--------*
 *                    Y     (0,0)
 *
 * @return
 */
static uint8_t *s_EinkTransformFrameCoordinateSystem_2Bpp(uint8_t *dataIn,
                                                          uint16_t windowWidthPx,
                                                          uint16_t windowHeightPx,
                                                          uint8_t *dataOut,
                                                          EinkDisplayColorMode_e invertColors);

/**
 *  This function makes rotation of the image from the standard GUI coordinate system to the coord system used by the
 * ED028TC1 display.
 *
 *  @note IT ROTATES only the 3Bpp image
 *
 *  @param uint8_t* dataIn          [in]  - input image to be translated. Each byte of that array must represent the
 * single pixel
 *  @param uint16_t x               [in]  - x coordinate of image in pixels
 *  @param uint16_t y               [in]  - y coordinate of image in pixels
 *  @param uint16_t windowWidthPx   [in]  - width of the image in pixels
 *  @param uint16_t windowHeightPx  [in]  - height of the image in pixels
 *  @param uint8_t* dataOut         [out] - the buffer for rotated image
 *  @param invertColors[in] - true if colors of the image are to be inverted, false otherwise
 *
 * @note Assumed dataIn coordinate system is the standard image coordinates system:
 *
 *   (0,0)   X
 *       *-------->
 *       |
 *     Y |
 *       |
 *       v
 *
 *
 * The result of the function is such a conversion of the dataIn array to make the Eink display show it the very same
 * way in its own coordinate system which is:
 *
 *              displayWidth
 *                 _______  ^
 *                |       | |
 *                |       | |
 *  displayHeight |       | |
 *                |       | |
 *                |_______| |  X
 *                   \ /    |
 *    signal tape -  | |    |
 *                          |
 *                 <--------*
 *                    Y     (0,0)
 *
 * @return
 */
static uint8_t *s_EinkTransformFrameCoordinateSystem_3Bpp(uint8_t *dataIn,
                                                          uint16_t windowWidthPx,
                                                          uint16_t windowHeightPx,
                                                          uint8_t *dataOut,
                                                          EinkDisplayColorMode_e invertColors);

/**
 *  This function makes rotation of the image from the standard GUI coordinate system to the coord system used by the
 * ED028TC1 display.
 *
 *  @note IT ROTATES only the 4Bpp image
 *
 *  @param uint8_t* dataIn          [in]  - input image to be translated. Each byte of that array must represent the
 * single pixel
 *  @param uint16_t x               [in]  - x coordinate of image in pixels
 *  @param uint16_t y               [in]  - y coordinate of image in pixels
 *  @param uint16_t windowWidthPx   [in]  - width of the image in pixels
 *  @param uint16_t windowHeightPx  [in]  - height of the image in pixels
 *  @param uint8_t* dataOut         [out] - the buffer for rotated image
 *  @param invertColors[in] - true if colors of the image are to be inverted, false otherwise
 *
 * @note Assumed dataIn coordinate system is the standard image coordinates system:
 *
 *   (0,0)   X
 *       *-------->
 *       |
 *     Y |
 *       |
 *       v
 *
 *
 * The result of the function is such a conversion of the dataIn array to make the Eink display show it the very same
 * way in its own coordinate system which is:
 *
 *              displayWidth
 *                 _______  ^
 *                |       | |
 *                |       | |
 *  displayHeight |       | |
 *                |       | |
 *                |_______| |  X
 *                   \ /    |
 *    signal tape -  | |    |
 *                          |
 *                 <--------*
 *                    Y     (0,0)
 *
 * @return
 */
static uint8_t *s_EinkTransformFrameCoordinateSystem_4Bpp(uint8_t *dataIn,
                                                          uint16_t windowWidthPx,
                                                          uint16_t windowHeightPx,
                                                          uint8_t *dataOut,
                                                          EinkDisplayColorMode_e invertColors);

/**
 *  This function makes rotation of the image from the standard GUI coordinate system to the coord system used by the
 * ED028TC1 display.
 *
 *  @note IT ROTATES only the 1Bpp image. It also makes sure that the image is black and white only
 *
 *  @param uint8_t* dataIn          [in]  - input image to be translated. Each byte of that array must represent the
 * single pixel
 *  @param uint16_t x               [in]  - x coordinate of image in pixels
 *  @param uint16_t y               [in]  - y coordinate of image in pixels
 *  @param uint16_t windowWidthPx   [in]  - width of the image in pixels
 *  @param uint16_t windowHeightPx  [in]  - height of the image in pixels
 *  @param uint8_t* dataOut         [out] - the buffer for rotated image
 *  @param invertColors[in] - true if colors of the image are to be inverted, false otherwise
 *
 * @note Assumed dataIn coordinate system is the standard image coordinates system:
 *
 *   (0,0)   X
 *       *-------->
 *       |
 *     Y |
 *       |
 *       v
 *
 *
 * The result of the function is such a conversion of the dataIn array to make the Eink display show it the very same
 * way in its own coordinate system which is:
 *
 *              displayWidth
 *                 _______  ^
 *                |       | |
 *                |       | |
 *  displayHeight |       | |
 *                |       | |
 *                |_______| |  X
 *                   \ /    |
 *    signal tape -  | |    |
 *                          |
 *                 <--------*
 *                    Y     (0,0)
 *
 * @return
 */
static uint8_t *s_EinkTransformAnimationFrameCoordinateSystem_1Bpp(uint8_t *dataIn,
                                                                   uint16_t windowWidthPx,
                                                                   uint16_t windowHeightPx,
                                                                   uint8_t *dataOut,
                                                                   EinkDisplayColorMode_e invertColors);

/**
 *  This function makes rotation of the image from the standard GUI coordinate system to the coord system used by the
 * ED028TC1 display.
 *
 *  @note IT ROTATES only the 2Bpp image. It also makes sure that the image is black and white only
 *
 *  @param uint8_t* dataIn          [in]  - input image to be translated. Each byte of that array must represent the
 * single pixel
 *  @param uint16_t x               [in]  - x coordinate of image in pixels
 *  @param uint16_t y               [in]  - y coordinate of image in pixels
 *  @param uint16_t windowWidthPx   [in]  - width of the image in pixels
 *  @param uint16_t windowHeightPx  [in]  - height of the image in pixels
 *  @param uint8_t* dataOut         [out] - the buffer for rotated image
 *  @param invertColors[in] - true if colors of the image are to be inverted, false otherwise
 *
 * @note Assumed dataIn coordinate system is the standard image coordinates system:
 *
 *   (0,0)   X
 *       *-------->
 *       |
 *     Y |
 *       |
 *       v
 *
 *
 * The result of the function is such a conversion of the dataIn array to make the Eink display show it the very same
 * way in its own coordinate system which is:
 *
 *              displayWidth
 *                 _______  ^
 *                |       | |
 *                |       | |
 *  displayHeight |       | |
 *                |       | |
 *                |_______| |  X
 *                   \ /    |
 *    signal tape -  | |    |
 *                          |
 *                 <--------*
 *                    Y     (0,0)
 *
 * @return
 */
static uint8_t *s_EinkTransformAnimationFrameCoordinateSystem_2Bpp(uint8_t *dataIn,
                                                                   uint16_t windowWidthPx,
                                                                   uint16_t windowHeightPx,
                                                                   uint8_t *dataOut,
                                                                   EinkDisplayColorMode_e invertColors);

/**
 *  This function makes rotation of the image from the standard GUI coordinate system to the coord system used by the
 * ED028TC1 display.
 *
 *  @note IT ROTATES only the 3Bpp image. It also makes sure that the image is black and white only
 *
 *  @param uint8_t* dataIn          [in]  - input image to be translated. Each byte of that array must represent the
 * single pixel
 *  @param uint16_t x               [in]  - x coordinate of image in pixels
 *  @param uint16_t y               [in]  - y coordinate of image in pixels
 *  @param uint16_t windowWidthPx   [in]  - width of the image in pixels
 *  @param uint16_t windowHeightPx  [in]  - height of the image in pixels
 *  @param uint8_t* dataOut         [out] - the buffer for rotated image
 *  @param invertColors[in] - true if colors of the image are to be inverted, false otherwise
 *
 * @note Assumed dataIn coordinate system is the standard image coordinates system:
 *
 *   (0,0)   X
 *       *-------->
 *       |
 *     Y |
 *       |
 *       v
 *
 *
 * The result of the function is such a conversion of the dataIn array to make the Eink display show it the very same
 * way in its own coordinate system which is:
 *
 *              displayWidth
 *                 _______  ^
 *                |       | |
 *                |       | |
 *  displayHeight |       | |
 *                |       | |
 *                |_______| |  X
 *                   \ /    |
 *    signal tape -  | |    |
 *                          |
 *                 <--------*
 *                    Y     (0,0)
 *
 * @return
 */
static uint8_t *s_EinkTransformAnimationFrameCoordinateSystem_3Bpp(uint8_t *dataIn,
                                                                   uint16_t windowWidthPx,
                                                                   uint16_t windowHeightPx,
                                                                   uint8_t *dataOut,
                                                                   EinkDisplayColorMode_e invertColors);

/**
 *  This function makes rotation of the image from the standard GUI coordinate system to the coord system used by the
 * ED028TC1 display.
 *
 *  @note IT ROTATES only the 4Bpp image. It also makes sure that the image is black and white only
 *
 *  @param uint8_t* dataIn          [in]  - input image to be translated. Each byte of that array must represent the
 * single pixel
 *  @param uint16_t x               [in]  - x coordinate of image in pixels
 *  @param uint16_t y               [in]  - y coordinate of image in pixels
 *  @param uint16_t windowWidthPx   [in]  - width of the image in pixels
 *  @param uint16_t windowHeightPx  [in]  - height of the image in pixels
 *  @param uint8_t* dataOut         [out] - the buffer for rotated image
 *  @param invertColors[in] - true if colors of the image are to be inverted, false otherwise
 *
 * @note Assumed dataIn coordinate system is the standard image coordinates system:
 *
 *   (0,0)   X
 *       *-------->
 *       |
 *     Y |
 *       |
 *       v
 *
 *
 * The result of the function is such a conversion of the dataIn array to make the Eink display show it the very same
 * way in its own coordinate system which is:
 *
 *              displayWidth
 *                 _______  ^
 *                |       | |
 *                |       | |
 *  displayHeight |       | |
 *                |       | |
 *                |_______| |  X
 *                   \ /    |
 *    signal tape -  | |    |
 *                          |
 *                 <--------*
 *                    Y     (0,0)
 *
 * @return
 */

/*
 * Not rotating version of s_EinkTransformAnimationFrameCoordinateSystem_4Bpp.
 * It is used when EINK_ROTATE_90_CLOCKWISE is not defined.
 */

static uint8_t *s_EinkTransformFrameCoordinateSystemNoRotation_4Bpp(uint8_t *dataIn,
                                                                    uint16_t windowWidthPx,
                                                                    uint16_t windowHeightPx,
                                                                    uint8_t *dataOut,
                                                                    EinkDisplayColorMode_e invertColors);

/* Function bodies */

void EinkChangeDisplayUpdateTimings(EinkDisplayTimingsMode_e timingsMode)
{
    char tmpbuf[4];
    tmpbuf[0] = EinkTCONSetting; // 0x60

    switch (timingsMode) {
    case EinkDisplayTimingsFastRefreshMode: {
        tmpbuf[1] = 0x10;
        tmpbuf[2] = 0x08;
        tmpbuf[3] = 0x08;
    } break;

    case EinkDisplayTimingsHighContrastMode: {
        tmpbuf[1] = 0x3F;
        tmpbuf[2] = 0x09;
        tmpbuf[3] = 0x2D;
    } break;

    case EinkDisplayTimingsDeepCleanMode: {
        tmpbuf[1] = 0x3F;
        tmpbuf[2] = 0x09;
        tmpbuf[3] = 0x50;
    } break;
    }

    if (BSP_EinkWriteData(tmpbuf, 4, SPI_AUTOMATIC_CS) != 0) {
        EinkResetAndInitialize();
        return;
    }
}

uint8_t EinkIsPoweredOn()
{
    return s_einkIsPoweredOn;
}

void EinkPowerOn()
{
    if (!s_einkIsPoweredOn) {
        uint8_t cmd = EinkPowerON; // 0x04
        if (BSP_EinkWriteData(&cmd, sizeof(cmd), SPI_AUTOMATIC_CS) != 0) {
            EinkResetAndInitialize();
            return;
        }

        BSP_EinkWaitUntilDisplayBusy(pdMS_TO_TICKS(BSP_EinkBusyTimeout));
        s_einkIsPoweredOn = true;
    }
}

void EinkPowerOff()
{
    if (s_einkIsPoweredOn) {
        uint8_t cmd = EinkPowerOFF; // 0x02
        if (BSP_EinkWriteData(&cmd, sizeof(cmd), SPI_AUTOMATIC_CS) != 0) {
            EinkResetAndInitialize();
            return;
        }

        BSP_EinkWaitUntilDisplayBusy(pdMS_TO_TICKS(BSP_EinkBusyTimeout));
        s_einkIsPoweredOn = false;
    }
}

void EinkPowerDown(void)
{
    EinkPowerOff();
    BSP_EinkDeinit();
}

int16_t EinkGetTemperatureInternal()
{
    uint8_t cmd[1];
    int8_t temp[2] = {0, 0};

    cmd[0] = EinkTemperatureSensorCalibration;

    BSP_EinkWriteCS(BSP_Eink_CS_Clr);

    if (BSP_EinkWriteData(cmd, sizeof(cmd), SPI_MANUAL_CS) != 0) {
        BSP_EinkWriteCS(BSP_Eink_CS_Set);
        EinkResetAndInitialize();
        return -1;
    }

    BSP_EinkWaitUntilDisplayBusy(pdMS_TO_TICKS(BSP_EinkBusyTimeout));

    if (BSP_EinkReadData(temp, sizeof(temp), SPI_MANUAL_CS) != 0) {
        BSP_EinkWriteCS(BSP_Eink_CS_Set);
        EinkResetAndInitialize();
        return -1;
    }

    BSP_EinkWriteCS(BSP_Eink_CS_Set);

    // First byte of the temp describes the integer part of the temperature in degrees Celsius
    int8_t temperatureInteger = temp[0];
    // The MSB bit of the second byte describes the fraction of the temperature. Bit value of 1 means .5 degree Celsius,
    // bit value of 0 means .0 degree Celsius
    // int8_t temperatureFraction =    ((temp[1] & 0x80) >> 7);

    return temperatureInteger;
}

static void s_EinkSetGateOrder()
{
    uint8_t buf[3];

    // Set the order of gate refreshing
    buf[0] = EinkGDOrderSetting;
    buf[1] = 0x02; // Magic value required by the ED028TC1 display manufacturer
    buf[2] = 0x00;
    if (BSP_EinkWriteData(buf, 3, SPI_AUTOMATIC_CS) != 0) {
        EinkResetAndInitialize();
        return;
    }
}

static void s_EinkSetInitialConfig()
{
    // send initialization data
    unsigned char tmpbuf[10];

    tmpbuf[0] = EinkPowerSetting; // 0x01
    tmpbuf[1] = 0x03;
    tmpbuf[2] = 0x04;
    tmpbuf[3] = 0x00; // 0x06
    tmpbuf[4] = 0x00;
    if (BSP_EinkWriteData(tmpbuf, 5, SPI_AUTOMATIC_CS) != 0) {
        return;
    }

    tmpbuf[0] = EinkPanelSetting;      // 0x00
    tmpbuf[1] = LUT_SEL | SHL | RST_N; // 0x25 -> _XON _RES0 LUT_SEL _DM - SHL _SPIWM RST_N
                                       // If 0x35 (DM - 1 is used (2bpp)) the SPI speed can be 25MHz
    tmpbuf[2] = 0x00;
    if (BSP_EinkWriteData(tmpbuf, 3, SPI_AUTOMATIC_CS) != 0) {
        return;
    }

    tmpbuf[0] = EinkPowerSaving; // 0x26
    tmpbuf[1] = 0x82;            // B2
    if (BSP_EinkWriteData(tmpbuf, 2, SPI_AUTOMATIC_CS) != 0) {
        return;
    }

    tmpbuf[0] = EinkPowerOFFSequenceSetting; // 0x03
    tmpbuf[1] = 0x01;                        // 0x00;//0x03;
    if (BSP_EinkWriteData(tmpbuf, 2, SPI_AUTOMATIC_CS) != 0) {
        return;
    }

    tmpbuf[0] = EinkBoosterSoftStart; // 0x07
    tmpbuf[1] = (EPD_BOOSTER_OFF_TIME_GDR_6_58uS << EPD_BOOSTER_OFF_TIME_GDR_POS) |
                (EPD_BOOSTER_DRIVING_STRENGTH_6 << EPD_BOOSTER_DRIVING_STRENGTH_POS) |
                (EPD_BOOSTER_START_PERIOD_10MS << EPD_BOOSTER_START_PERIOD_POS);

    tmpbuf[2] = (EPD_BOOSTER_OFF_TIME_GDR_6_58uS << EPD_BOOSTER_OFF_TIME_GDR_POS) |
                (EPD_BOOSTER_DRIVING_STRENGTH_6 << EPD_BOOSTER_DRIVING_STRENGTH_POS) |
                (EPD_BOOSTER_START_PERIOD_10MS << EPD_BOOSTER_START_PERIOD_POS);

    tmpbuf[3] = (EPD_BOOSTER_OFF_TIME_GDR_0_40uS << EPD_BOOSTER_OFF_TIME_GDR_POS) |
                (EPD_BOOSTER_DRIVING_STRENGTH_7 << EPD_BOOSTER_DRIVING_STRENGTH_POS) |
                (EPD_BOOSTER_START_PERIOD_10MS << EPD_BOOSTER_START_PERIOD_POS);
    if (BSP_EinkWriteData(tmpbuf, 4, SPI_AUTOMATIC_CS) != 0) {
        return;
    }

    tmpbuf[0] = EinkPLLControl; // 0x30
    tmpbuf[1] = 0x0E;
    if (BSP_EinkWriteData(tmpbuf, 2, SPI_AUTOMATIC_CS) != 0) {
        return;
    }

    tmpbuf[0] = EinkTemperatureSensorSelection;              // Temp. sensor setting TSE
    tmpbuf[1] = EINK_TEMPERATURE_SENSOR_USE_INTERNAL | 0x00; // Temperature offset
    tmpbuf[2] = 0x00;                                        // Host forced temperature value
    if (BSP_EinkWriteData(tmpbuf, 3, SPI_AUTOMATIC_CS) != 0) {
        return;
    }

    tmpbuf[0] = EinkVcomAndDataIntervalSetting; // 0x50
    tmpbuf[1] = DDX;                            // 0x01;   // 0x0D
    tmpbuf[2] = 0x00;                           // 0x22;
    if (BSP_EinkWriteData(tmpbuf, 3, SPI_AUTOMATIC_CS) != 0) {
        return;
    }

    tmpbuf[0] = EinkTCONSetting; // 0x60
    tmpbuf[1] = 0x3F;
    tmpbuf[2] = 0x09;
    tmpbuf[3] = 0x2D;
    if (BSP_EinkWriteData(tmpbuf, 4, SPI_AUTOMATIC_CS) != 0) {
        return;
    }

    tmpbuf[0] = EinkResolutionSetting; // 0x61
    tmpbuf[1] = 0x02;                  // 0x02
    tmpbuf[2] = 0x60;                  // 0x60
    tmpbuf[3] = 0x01;                  // 0x01
    tmpbuf[4] = 0xE0;                  // 0xE0
    if (BSP_EinkWriteData(tmpbuf, 5, SPI_AUTOMATIC_CS) != 0) {
        return;
    }

    tmpbuf[0] = EinkVCM_DCSetting; // 0x82
    tmpbuf[1] = 0x30;
    if (BSP_EinkWriteData(tmpbuf, 2, SPI_AUTOMATIC_CS) != 0) {
        return;
    }
}

EinkStatus_e EinkResetAndInitialize()
{
    // Initialize the synchronization resources, SPI and GPIOs for the Eink BSP
    BSP_EinkInit(NULL);
    // Reset the display
    BSP_EinkResetDisplayController();
    // Set the initial configuration of the eink registers after reset
    s_EinkSetInitialConfig();

    // After the reset the display is powered off
    s_einkIsPoweredOn = false;
    return EinkOK;
}

EinkStatus_e EinkUpdateWaveform(const EinkWaveformSettings_t *settings)
{
    /// LUTD
    if (BSP_EinkWriteData(settings->LUTDData, settings->LUTDSize, SPI_AUTOMATIC_CS) != 0) {
        EinkResetAndInitialize();
        return EinkSPIErr;
    }

    /// LUTC
    if (BSP_EinkWriteData(settings->LUTCData, settings->LUTCSize + 1, SPI_AUTOMATIC_CS) != 0) {
        EinkResetAndInitialize();
        return EinkSPIErr;
    }

    s_einkConfiguredWaveform = settings->mode;

    return EinkOK;
}

static EinkStatus_e s_EinkReadFlagsRegister(uint16_t *flags)
{
    uint8_t cmd = EinkFLG;

    BSP_EinkWriteCS(BSP_Eink_CS_Clr);

    if (BSP_EinkWriteData(&cmd, sizeof(cmd), SPI_MANUAL_CS) != 0) {
        BSP_EinkWriteCS(BSP_Eink_CS_Set);
        EinkResetAndInitialize();
        return EinkSPIErr;
    }

    if (BSP_EinkReadData(flags, sizeof(uint16_t), SPI_MANUAL_CS) != 0) {
        BSP_EinkWriteCS(BSP_Eink_CS_Set);
        EinkResetAndInitialize();
        return EinkSPIErr;
    }

    BSP_EinkWriteCS(BSP_Eink_CS_Set);

    return EinkOK;
}

EinkStatus_e EinkWaitTillPipelineBusy()
{
    uint16_t flags = 0;

    s_EinkReadFlagsRegister(&flags);

    while ((flags & EINK_FLAG_PIPELINE_BUSY)) {
        vTaskDelay(pdMS_TO_TICKS(1));
        s_EinkReadFlagsRegister(&flags);
    }

    return EinkOK;
}

EinkStatus_e EinkDitherDisplay()
{
    uint8_t cmdWithArgs[2] = {EinkDPC, EINK_DITHER_4BPP_MODE | EINK_DITHER_START};

    if (BSP_EinkWriteData(cmdWithArgs, sizeof(cmdWithArgs), SPI_AUTOMATIC_CS) != 0) {
        EinkResetAndInitialize();
        return EinkSPIErr;
    }

    uint16_t flags = 0;

    s_EinkReadFlagsRegister(&flags);

    // Wait for the dither operation finish
    while (flags & EINK_FLAG_DITHER_IN_PROGRESS) {
        vTaskDelay(pdMS_TO_TICKS(1));
        s_EinkReadFlagsRegister(&flags);
    }

    return EinkOK;
}

EinkStatus_e EinkUpdateFrame(
    uint16_t X, uint16_t Y, uint16_t W, uint16_t H, uint8_t *buffer, EinkBpp_e bpp, EinkDisplayColorMode_e invertColors)
{
    uint8_t buf[10];
    uint8_t pixelsInByte = 8 / bpp;

    s_einkServiceRotatedBuf[0] = EinkDataStartTransmission1;
    s_einkServiceRotatedBuf[1] = bpp - 1; //  0 - 1Bpp, 1 - 2Bpp, 2 - 3Bpp, 3 - 4Bpp

    if ((s_einkConfiguredWaveform == EinkWaveformA2) || (s_einkConfiguredWaveform == EinkWaveformDU2)) {
        switch (bpp) {
        case Eink1Bpp: {
            s_EinkTransformAnimationFrameCoordinateSystem_1Bpp(buffer, W, H, s_einkServiceRotatedBuf + 2, invertColors);
        } break;
        case Eink2Bpp: {
            s_EinkTransformAnimationFrameCoordinateSystem_2Bpp(buffer, W, H, s_einkServiceRotatedBuf + 2, invertColors);
        } break;
        case Eink3Bpp: {
            s_EinkTransformAnimationFrameCoordinateSystem_3Bpp(buffer, W, H, s_einkServiceRotatedBuf + 2, invertColors);
        } break;
        case Eink4Bpp: {
#if defined(EINK_ROTATE_90_CLOCKWISE)
            s_EinkTransformFrameCoordinateSystem_4Bpp(buffer, W, H, s_einkServiceRotatedBuf + 2, invertColors);
#else
            s_EinkTransformFrameCoordinateSystemNoRotation_4Bpp(
                buffer, W, H, s_einkServiceRotatedBuf + 2, invertColors);
#endif
        } break;
        }
    }
    else {
        switch (bpp) {
        case Eink1Bpp: {
            s_EinkTransformFrameCoordinateSystem_1Bpp(buffer, W, H, s_einkServiceRotatedBuf + 2, invertColors);
        } break;
        case Eink2Bpp: {
            s_EinkTransformFrameCoordinateSystem_2Bpp(buffer, W, H, s_einkServiceRotatedBuf + 2, invertColors);
        } break;
        case Eink3Bpp: {
            s_EinkTransformFrameCoordinateSystem_3Bpp(buffer, W, H, s_einkServiceRotatedBuf + 2, invertColors);
        } break;
        case Eink4Bpp: {
#if defined(EINK_ROTATE_90_CLOCKWISE)
            s_EinkTransformFrameCoordinateSystem_4Bpp(buffer, W, H, s_einkServiceRotatedBuf + 2, invertColors);
#else
            s_EinkTransformFrameCoordinateSystemNoRotation_4Bpp(
                buffer, W, H, s_einkServiceRotatedBuf + 2, invertColors);
#endif
        } break;
        }
    }

    buf[0] = EinkDataStartTransmissionWindow;           // set display window
    buf[1] = (uint8_t)(EINK_DISPLAY_X_AXIS >> 8);       // MSB of the X axis in the EPD display. Value converted
                                                        // from the standard GUI coords system to the ED028TC1 one
    buf[2] = (uint8_t)EINK_DISPLAY_X_AXIS;              // LSB of the X axis in the EPD display. Value converted from
                                                        // the standard GUI coords system to the ED028TC1 one
    buf[3] = (uint8_t)(EINK_DISPLAY_Y_AXIS >> 8);       // MSB of the Y axis in the EPD display. Value converted
                                                        // from the standard GUI coords system to the ED028TC1 one
    buf[4] = (uint8_t)EINK_DISPLAY_Y_AXIS;              // LSB of the Y axis in the EPD display. Value converted from
                                                        // the standard GUI coords system to the ED028TC1 one
    buf[5] = (uint8_t)(EINK_DISPLAY_WINDOW_WIDTH >> 8); // MSB of the window height in the EPD display. Value converted
                                                        // from the standard GUI coords system to the ED028TC1 one
    buf[6] = (uint8_t)EINK_DISPLAY_WINDOW_WIDTH; // LSB of the window height in the EPD display. Value converted from
                                                 // the standard GUI coords system to the ED028TC1 one
    buf[7] = (uint8_t)(EINK_DISPLAY_WINDOW_HEIGHT >> 8); // MSB of the window width in the EPD display. Value converted
                                                         // from the standard GUI coords system to the ED028TC1 one
    buf[8] = (uint8_t)EINK_DISPLAY_WINDOW_HEIGHT; // LSB of the window width in the EPD display. Value converted from
                                                  // the standard GUI coords system to the ED028TC1 one

    if (BSP_EinkWriteData(buf, 9, SPI_AUTOMATIC_CS) != 0) {
        EinkResetAndInitialize();
        return EinkSPIErr;
    }

    uint32_t msgSize = 2 + ((uint32_t)W * (uint32_t)H /
                            pixelsInByte); // command (1 byte) + bpp (1 byte) + dataSize(W*H/pixelsInByte bytes)
    // Send the part of the image to the display memory

    if (BSP_EinkWriteData(s_einkServiceRotatedBuf, msgSize, SPI_AUTOMATIC_CS) != 0) {
        EinkResetAndInitialize();
        return EinkSPIErr;
    }

    return (EinkOK);
}

EinkStatus_e EinkFillScreenWithColor(EinkDisplayColorFilling_e colorFill)
{
    uint8_t buf[10];

    // Set the window to the entire screen
    buf[0] = EinkDataStartTransmissionWindow; // 0x83
    buf[1] = 0x00;
    buf[2] = 0x00;
    buf[3] = 0x00;
    buf[4] = 0x00;
    buf[5] = (EINK_DISPLAY_RES_X & 0xFF00) >> 8;
    buf[6] = EINK_DISPLAY_RES_X & 0x00FF;
    buf[7] = (EINK_DISPLAY_RES_Y & 0xFF00) >> 8;
    buf[8] = EINK_DISPLAY_RES_Y & 0x00FF;

    if (BSP_EinkWriteData(buf, 9, SPI_AUTOMATIC_CS) != 0) {
        EinkResetAndInitialize();
        return EinkSPIErr;
    }

    BSP_EinkWriteCS(BSP_Eink_CS_Clr);

    buf[0] = EinkDataStartTransmission1; // 0x10
    buf[1] = Eink1Bpp - 1;
    if (BSP_EinkWriteData(buf, 2, SPI_MANUAL_CS) != 0) {
        BSP_EinkWriteCS(BSP_Eink_CS_Set);
        EinkResetAndInitialize();
        return EinkSPIErr;
    }

    uint8_t background = colorFill;

    std::unique_ptr<char[]> bg;
    try {
        bg = std::unique_ptr<char[]>(new char[BOARD_EINK_DISPLAY_RES_Y * BOARD_EINK_DISPLAY_RES_X / 8]);
    }
    catch (const std::bad_alloc &exception) {
        LOG_ERROR("Could not create the buffer for the background plane");
        return EinkNoMem;
    }

    memset(bg.get(), background, BOARD_EINK_DISPLAY_RES_Y * BOARD_EINK_DISPLAY_RES_X / 8);

    if (BSP_EinkWriteData(bg.get(), BOARD_EINK_DISPLAY_RES_Y * BOARD_EINK_DISPLAY_RES_X / 8, SPI_MANUAL_CS) != 0) {
        BSP_EinkWriteCS(BSP_Eink_CS_Set);
        EinkResetAndInitialize();
        return EinkSPIErr;
    }

    BSP_EinkWriteCS(BSP_Eink_CS_Set);

    EinkRefreshImage(0, 0, BOARD_EINK_DISPLAY_RES_X, BOARD_EINK_DISPLAY_RES_Y, EinkDisplayTimingsDeepCleanMode);

    return EinkOK;
}

EinkStatus_e EinkRefreshImage(
    uint16_t X, uint16_t Y, uint16_t W, uint16_t H, EinkDisplayTimingsMode_e refreshTimingsMode)
{
    EinkChangeDisplayUpdateTimings(refreshTimingsMode);

    s_EinkSetGateOrder();

    uint8_t buf[10];

    buf[0] = EinkDisplayRefresh;
    buf[1] = UPD_CPY_TO_PRE;

    buf[2] = (uint8_t)(EINK_DISPLAY_X_AXIS >> 8);       // MSB of the X axis in the EPD display. Value converted
                                                        // from the standard GUI coords system to the ED028TC1 one
    buf[3] = (uint8_t)EINK_DISPLAY_X_AXIS;              // LSB of the X axis in the EPD display. Value converted from
                                                        // the standard GUI coords system to the ED028TC1 one
    buf[4] = (uint8_t)(EINK_DISPLAY_Y_AXIS >> 8);       // MSB of the Y axis in the EPD display. Value converted
                                                        // from the standard GUI coords system to the ED028TC1 one
    buf[5] = (uint8_t)EINK_DISPLAY_Y_AXIS;              // LSB of the Y axis in the EPD display. Value converted from
                                                        // the standard GUI coords system to the ED028TC1 one
    buf[6] = (uint8_t)(EINK_DISPLAY_WINDOW_WIDTH >> 8); // MSB of the window height in the EPD display. Value converted
                                                        // from the standard GUI coords system to the ED028TC1 one
    buf[7] = (uint8_t)EINK_DISPLAY_WINDOW_WIDTH; // LSB of the window height in the EPD display. Value converted from
                                                 // the standard GUI coords system to the ED028TC1 one
    buf[8] = (uint8_t)(EINK_DISPLAY_WINDOW_HEIGHT >> 8); // MSB of the window width in the EPD display. Value converted
                                                         // from the standard GUI coords system to the ED028TC1 one
    buf[9] = (uint8_t)EINK_DISPLAY_WINDOW_HEIGHT; // LSB of the window width in the EPD display. Value converted from
                                                  // the standard GUI coords system to the ED028TC1 one

    if (BSP_EinkWriteData(buf, sizeof(buf), SPI_AUTOMATIC_CS) != 0) {
        EinkResetAndInitialize();
        return EinkSPIErr;
    }

    BSP_EinkWaitUntilDisplayBusy(pdMS_TO_TICKS(BSP_EinkBusyTimeout));

    return EinkOK;
}

__attribute__((optimize("O3"))) void EinkARGBToLuminance(uint8_t *dataIn,
                                                         uint8_t *dataOut,
                                                         uint32_t displayWidth,
                                                         uint32_t displayHeight)
{
    //    uint32_t i, j;
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

__attribute__((optimize("O1"))) static uint8_t *s_EinkTransformFrameCoordinateSystem_1Bpp(
    uint8_t *dataIn,
    uint16_t windowWidthPx,
    uint16_t windowHeightPx,
    uint8_t *dataOut,
    EinkDisplayColorMode_e invertColors)
{
    // In 1bpp mode there are 8 pixels in the byte
    const uint8_t pixelsInByte = 8;

    uint8_t pixels    = 0;
    uint8_t *outArray = dataOut;

    for (int32_t inputCol = windowWidthPx - 1; inputCol >= 0; --inputCol) {
        for (int32_t inputRow = windowHeightPx - 1; inputRow >= 7; inputRow -= pixelsInByte) {
            // HACK: Did not create the loop for accessing pixels and merging them in the single byte for better
            // performance.
            //       Wanted to avoid unneeded loop count increasing and jump operations which for large amount of data
            //       take considerable amount of time.
            uint32_t index = inputRow * BOARD_EINK_DISPLAY_RES_X + inputCol;

            // Use the LUT to convert the input pixel from 4bpp to 1bpp
            uint8_t firstPixel   = s_einkMaskLut_1Bpp[dataIn[index - 0 * BOARD_EINK_DISPLAY_RES_X]];
            uint8_t secondPixel  = s_einkMaskLut_1Bpp[dataIn[index - 1 * BOARD_EINK_DISPLAY_RES_X]];
            uint8_t thirdPixel   = s_einkMaskLut_1Bpp[dataIn[index - 2 * BOARD_EINK_DISPLAY_RES_X]];
            uint8_t fourthPixel  = s_einkMaskLut_1Bpp[dataIn[index - 3 * BOARD_EINK_DISPLAY_RES_X]];
            uint8_t fifthPixel   = s_einkMaskLut_1Bpp[dataIn[index - 4 * BOARD_EINK_DISPLAY_RES_X]];
            uint8_t sixthPixel   = s_einkMaskLut_1Bpp[dataIn[index - 5 * BOARD_EINK_DISPLAY_RES_X]];
            uint8_t seventhPixel = s_einkMaskLut_1Bpp[dataIn[index - 6 * BOARD_EINK_DISPLAY_RES_X]];
            uint8_t eightPixel   = s_einkMaskLut_1Bpp[dataIn[index - 7 * BOARD_EINK_DISPLAY_RES_X]];

            // Put the pixels in order: Most left positioned pixel at the most significant side of byte
            pixels = (firstPixel << 7) | (secondPixel << 6) | (thirdPixel << 5) | (fourthPixel << 4) |
                     (fifthPixel << 3) | (sixthPixel << 2) | (seventhPixel << 1) | (eightPixel << 0);

            if (invertColors) {
                pixels = ~pixels;
            }

            *outArray = pixels;
            ++outArray;
        }
    }

    return dataOut;
}

__attribute__((optimize("O1"))) static uint8_t *s_EinkTransformFrameCoordinateSystem_2Bpp(
    uint8_t *dataIn,
    uint16_t windowWidthPx,
    uint16_t windowHeightPx,
    uint8_t *dataOut,
    EinkDisplayColorMode_e invertColors)
{
    // In 2bpp mode there are 4 pixels in the byte
    const uint8_t pixelsInByte = 8;
    uint16_t pixels            = 0;
    uint16_t *outArray         = (uint16_t *)dataOut;
    uint8_t temp               = 0;

    for (int32_t inputCol = windowWidthPx - 1; inputCol >= 0; --inputCol) {
        for (int32_t inputRow = windowHeightPx - 1; inputRow >= 7; inputRow -= pixelsInByte) {
            // HACK: Did not create the loop for accessing pixels and merging them in the single byte for better
            // performance.
            //       Wanted to avoid unneeded loop count increasing and jump operations which for large amount of data
            //       take considerable amount of time.
            uint32_t index = inputRow * BOARD_EINK_DISPLAY_RES_X + inputCol;

            // Use the LUT to convert the input pixel from 4bpp to 2bpp and put 4 pixels in single byte
            temp = (s_einkMaskLut_2Bpp[dataIn[index - 0 * BOARD_EINK_DISPLAY_RES_X]] << 6);
            temp |= (s_einkMaskLut_2Bpp[dataIn[index - 1 * BOARD_EINK_DISPLAY_RES_X]] << 4);
            temp |= (s_einkMaskLut_2Bpp[dataIn[index - 2 * BOARD_EINK_DISPLAY_RES_X]] << 2);
            temp |= (s_einkMaskLut_2Bpp[dataIn[index - 3 * BOARD_EINK_DISPLAY_RES_X]] << 0);

            // Push the 4 pixels into the proper place in the uint16_t
            pixels = temp << 0;

            // Use the LUT to convert the input pixel from 4bpp to 2bpp and put 4 pixels in single byte
            temp = (s_einkMaskLut_2Bpp[dataIn[index - 4 * BOARD_EINK_DISPLAY_RES_X]] << 6);
            temp |= (s_einkMaskLut_2Bpp[dataIn[index - 5 * BOARD_EINK_DISPLAY_RES_X]] << 4);
            temp |= (s_einkMaskLut_2Bpp[dataIn[index - 6 * BOARD_EINK_DISPLAY_RES_X]] << 2);
            temp |= (s_einkMaskLut_2Bpp[dataIn[index - 7 * BOARD_EINK_DISPLAY_RES_X]] << 0);

            // Push the 4 pixels into the proper place in the uint16_t
            pixels |= temp << 8;

            if (invertColors) {
                pixels = ~pixels;
            }

            *outArray = pixels;
            ++outArray;
        }
    }

    return dataOut;
}

__attribute__((optimize("O1"))) static uint8_t *s_EinkTransformFrameCoordinateSystem_3Bpp(
    uint8_t *dataIn,
    uint16_t windowWidthPx,
    uint16_t windowHeightPx,
    uint8_t *dataOut,
    EinkDisplayColorMode_e invertColors)
{
    // The 4bpp is coded the same way as the 3bpp
    return s_EinkTransformFrameCoordinateSystem_4Bpp(dataIn, windowWidthPx, windowHeightPx, dataOut, invertColors);
}

__attribute__((optimize("O1"))) static uint8_t *s_EinkTransformAnimationFrameCoordinateSystem_1Bpp(
    uint8_t *dataIn,
    uint16_t windowWidthPx,
    uint16_t windowHeightPx,
    uint8_t *dataOut,
    EinkDisplayColorMode_e invertColors)
{
    // In 1bpp mode there are 8 pixels in the byte
    const uint8_t pixelsInByte = 8;
    uint8_t pixels             = 0;
    uint8_t *outArray          = dataOut;

    for (int32_t inputCol = windowWidthPx - 1; inputCol >= 0; --inputCol) {
        for (int32_t inputRow = windowHeightPx - 1; inputRow >= 7; inputRow -= pixelsInByte) {
            // HACK: Did not create the loop for accessing pixels and merging them in the single byte for better
            // performance.
            //       Wanted to avoid unneeded loop count increasing and jump operations which for large amount of data
            //       take considerable amount of time.

            uint32_t index = inputRow * BOARD_EINK_DISPLAY_RES_X + inputCol;

            // Use the LUT to convert the input pixel from 4bpp to 1bpp
            uint8_t firstPixel   = s_einkMaskLut_1Bpp[dataIn[index - 0 * BOARD_EINK_DISPLAY_RES_X]];
            uint8_t secondPixel  = s_einkMaskLut_1Bpp[dataIn[index - 1 * BOARD_EINK_DISPLAY_RES_X]];
            uint8_t thirdPixel   = s_einkMaskLut_1Bpp[dataIn[index - 2 * BOARD_EINK_DISPLAY_RES_X]];
            uint8_t fourthPixel  = s_einkMaskLut_1Bpp[dataIn[index - 3 * BOARD_EINK_DISPLAY_RES_X]];
            uint8_t fifthPixel   = s_einkMaskLut_1Bpp[dataIn[index - 4 * BOARD_EINK_DISPLAY_RES_X]];
            uint8_t sixthPixel   = s_einkMaskLut_1Bpp[dataIn[index - 5 * BOARD_EINK_DISPLAY_RES_X]];
            uint8_t seventhPixel = s_einkMaskLut_1Bpp[dataIn[index - 6 * BOARD_EINK_DISPLAY_RES_X]];
            uint8_t eightPixel   = s_einkMaskLut_1Bpp[dataIn[index - 7 * BOARD_EINK_DISPLAY_RES_X]];

            // Put the pixels in order: Most left positioned pixel at the most significant side of byte
            pixels = (firstPixel << 7) | (secondPixel << 6) | (thirdPixel << 5) | (fourthPixel << 4) |
                     (fifthPixel << 3) | (sixthPixel << 2) | (seventhPixel << 1) | (eightPixel << 0);

            if (invertColors) {
                pixels = ~pixels;
            }

            *outArray = pixels;
            ++outArray;
        }
    }

    return dataOut;
}

__attribute__((optimize("O1"))) static uint8_t *s_EinkTransformAnimationFrameCoordinateSystem_2Bpp(
    uint8_t *dataIn,
    uint16_t windowWidthPx,
    uint16_t windowHeightPx,
    uint8_t *dataOut,
    EinkDisplayColorMode_e invertColors)
{
    // In 2bpp mode there are 4 pixels in the byte
    const uint8_t pixelsInByte = 8;
    uint16_t pixels            = 0;
    uint16_t *outArray         = (uint16_t *)dataOut;
    uint8_t temp               = 0;
    for (int32_t inputCol = windowWidthPx - 1; inputCol >= 0; --inputCol) {
        for (int32_t inputRow = windowHeightPx - 1; inputRow >= 7; inputRow -= pixelsInByte) {
            // HACK: Did not create the loop for accessing pixels and merging them in the single byte for better
            // performance.
            //       Wanted to avoid unneeded loop count increasing and jump operations which for large amount of data
            //       take considerable amount of time.
            uint32_t index = inputRow * BOARD_EINK_DISPLAY_RES_X + inputCol;

            // Use the LUT to convert the input pixel from 4bpp to 2bpp and put 4 pixels in single byte
            temp = (s_einkMaskLut_2Bpp[dataIn[index - 0 * BOARD_EINK_DISPLAY_RES_X]] << 6);
            temp |= (s_einkMaskLut_2Bpp[dataIn[index - 1 * BOARD_EINK_DISPLAY_RES_X]] << 4);
            temp |= (s_einkMaskLut_2Bpp[dataIn[index - 2 * BOARD_EINK_DISPLAY_RES_X]] << 2);
            temp |= (s_einkMaskLut_2Bpp[dataIn[index - 3 * BOARD_EINK_DISPLAY_RES_X]] << 0);

            // Use the LUT to binarize the 2bpp pixels value and push them into the proper place in the uint16_t
            pixels = einkBinarizationLUT_2bpp[temp] << 0;

            // Use the LUT to convert the input pixel from 4bpp to 2bpp and put 4 pixels in single byte
            temp = (s_einkMaskLut_2Bpp[dataIn[index - 4 * BOARD_EINK_DISPLAY_RES_X]] << 6);
            temp |= (s_einkMaskLut_2Bpp[dataIn[index - 5 * BOARD_EINK_DISPLAY_RES_X]] << 4);
            temp |= (s_einkMaskLut_2Bpp[dataIn[index - 6 * BOARD_EINK_DISPLAY_RES_X]] << 2);
            temp |= (s_einkMaskLut_2Bpp[dataIn[index - 7 * BOARD_EINK_DISPLAY_RES_X]] << 0);

            // Use the LUT to binarize the 2bpp pixels value and push them into the proper place in the uint16_t
            pixels |= einkBinarizationLUT_2bpp[temp] << 8;
            if (invertColors) {
                pixels = ~pixels;
            }

            *outArray = pixels;
            ++outArray;
        }
    }

    return dataOut;
}

__attribute__((optimize("O3"))) static uint8_t *s_EinkTransformAnimationFrameCoordinateSystem_3Bpp(
    uint8_t *dataIn,
    uint16_t windowWidthPx,
    uint16_t windowHeightPx,
    uint8_t *dataOut,
    EinkDisplayColorMode_e invertColors)
{
    // The 4bpp is coded the same way as the 3bpp
    return s_EinkTransformFrameCoordinateSystem_4Bpp(dataIn, windowWidthPx, windowHeightPx, dataOut, invertColors);
}

__attribute__((optimize("O1"))) static uint8_t *s_EinkTransformFrameCoordinateSystem_4Bpp(
    uint8_t *dataIn,
    uint16_t windowWidthPx,
    uint16_t windowHeightPx,
    uint8_t *dataOut,
    EinkDisplayColorMode_e invertColors)
{
    // In 3bpp and 4bpp modes there are 2 pixels in the byte. Using 8bpp to process the whole uint32_t at once for
    // faster execution
    const uint8_t pixelsInByte = 8;

    uint32_t pixels    = 0;
    uint32_t *outArray = (uint32_t *)dataOut;

    for (int32_t inputCol = windowWidthPx - 1; inputCol >= 0; --inputCol) {
        for (int32_t inputRow = windowHeightPx - 1; inputRow >= 7; inputRow -= pixelsInByte) {
            // HACK: Did not create the loop for accessing pixels and merging them in the single byte for better
            // performance.
            //       Wanted to avoid unneeded loop count increasing and jump operations which for large amount of data
            //       take considerable amount of time. Using 8 pixels at a time for better performance
            uint32_t index = inputRow * BOARD_EINK_DISPLAY_RES_X + inputCol;

            uint8_t firstPixelPair =
                (dataIn[index - 0 * BOARD_EINK_DISPLAY_RES_X] << 4) | dataIn[index - 1 * BOARD_EINK_DISPLAY_RES_X];
            uint8_t secondPixelPair =
                (dataIn[index - 2 * BOARD_EINK_DISPLAY_RES_X] << 4) | dataIn[index - 3 * BOARD_EINK_DISPLAY_RES_X];
            uint8_t thirdPixelPair =
                (dataIn[index - 4 * BOARD_EINK_DISPLAY_RES_X] << 4) | dataIn[index - 5 * BOARD_EINK_DISPLAY_RES_X];
            uint8_t fourthPixelPair =
                (dataIn[index - 6 * BOARD_EINK_DISPLAY_RES_X] << 4) | dataIn[index - 7 * BOARD_EINK_DISPLAY_RES_X];

            // Put the pixels in the uint32_t for faster processing
            pixels = firstPixelPair | (secondPixelPair << 8) | (thirdPixelPair << 16) | (fourthPixelPair << 24);

            if (invertColors) {
                pixels = ~pixels;
            }

            // Put the pixels in order: Most left positioned pixel at the most significant side of byte
            *outArray = pixels;
            ++outArray;
        }
    }

    return dataOut;
}

__attribute__((optimize("O1"))) static uint8_t *s_EinkTransformFrameCoordinateSystemNoRotation_4Bpp(
    uint8_t *dataIn,
    uint16_t windowWidthPx,
    uint16_t windowHeightPx,
    uint8_t *dataOut,
    EinkDisplayColorMode_e invertColors)
{
    // In 3bpp and 4bpp modes there are 2 pixels in the byte. Using 8bpp to process the whole uint32_t at once for
    // faster execution
    const uint8_t pixelsInByte = 8;

    uint32_t pixels    = 0;
    uint32_t *outArray = (uint32_t *)dataOut;
    int32_t inputRow   = 0;
    int32_t inputCol   = 0;

    for (inputRow = windowHeightPx - 1; inputRow >= 0; --inputRow) {
        for (inputCol = 0; inputCol < windowWidthPx - 7; inputCol += pixelsInByte) {
            // HACK: Did not create the loop for accessing pixels and merging them in the single byte for better
            // performance.
            //       Wanted to avoid unneeded loop count increasing and jump operations which for large amount of data
            //       take considerable amount of time. Using 8 pixels at a time for better performance
            uint32_t index = inputRow * BOARD_EINK_DISPLAY_RES_X + inputCol;

            // Get 4x 2 adjacent pixels to process them as uint32_t for better execution timings
            uint8_t firstPixelPair  = (dataIn[index] << 4) | dataIn[index + 1];
            uint8_t secondPixelPair = (dataIn[index + 2] << 4) | dataIn[index + 3];
            uint8_t thirdPixelPair  = (dataIn[index + 4] << 4) | dataIn[index + 5];
            uint8_t fourthPixelPair = (dataIn[index + 6] << 4) | dataIn[index + 7];

            // Put the pixels in the uint32_t for faster processing
            pixels = (firstPixelPair) | (secondPixelPair << 8) | (thirdPixelPair << 16) | (fourthPixelPair << 24);

            if (invertColors) {
                pixels = ~pixels;
            }

            // Put the pixels in order: Most left positioned pixel at the most significant side of byte
            *outArray = pixels;
            ++outArray;
        }
    }

    return dataOut;
}
