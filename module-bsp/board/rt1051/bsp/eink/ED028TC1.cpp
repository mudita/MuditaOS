// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ED028TC1.h"
#include "macros.h"
#include "bsp_eink.h"
#include "eink_dimensions.hpp"
#include "eink_binarization_luts.h"

#include <math.h>

#include <log/log.hpp>
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

#define EINK_BLACK_PIXEL_MASK      0x00 // This is the mask for the black pixel value
#define EINK_1BPP_WHITE_PIXEL_MASK 0x01 // This is the mask for the white pixel in 1bpp mode
#define EINK_2BPP_WHITE_PIXEL_MASK 0x03 // This is the mask for the white pixel in 2bpp mode
#define EINK_4BPP_WHITE_PIXEL_MASK 0x0F // This is the mask for the white pixel in 4bpp mode

/* Internal variable definitions */
static bool s_einkIsPoweredOn = false; //  Variable which contains the state of the power of the EPD display

static EinkWaveforms_e s_einkConfiguredWaveform =
    EinkWaveformGC16; //  This variable contains the current waveform set in the display

static CACHEABLE_SECTION_SDRAM(uint8_t s_einkServiceRotatedBuf[BOARD_EINK_DISPLAY_RES_X * BOARD_EINK_DISPLAY_RES_Y / 2 +
                                                               2]); // Plus 2 for the EPD command and BPP config

/**
 * @brief This lut is used for convertion of the 4bp input grayscale pixel to the 1bpp output pixel
 */
static std::uint8_t s_einkMaskLut_1Bpp[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1};

/**
 * @brief This lut is used for convertion of the 4bp input grayscale pixel to the 2bpp output pixel
 */
static std::uint8_t s_einkMaskLut_2Bpp[16] = {0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3};

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
static std::uint8_t *s_EinkTransformFrameCoordinateSystem_1Bpp(const std::uint8_t *dataIn,
                                                               std::uint16_t windowWidthPx,
                                                               std::uint16_t windowHeightPx,
                                                               std::uint8_t *dataOut,
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
static std::uint8_t *s_EinkTransformFrameCoordinateSystem_2Bpp(const std::uint8_t *dataIn,
                                                               std::uint16_t windowWidthPx,
                                                               std::uint16_t windowHeightPx,
                                                               std::uint8_t *dataOut,
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
static std::uint8_t *s_EinkTransformFrameCoordinateSystem_3Bpp(const std::uint8_t *dataIn,
                                                               std::uint16_t windowWidthPx,
                                                               std::uint16_t windowHeightPx,
                                                               std::uint8_t *dataOut,
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
static std::uint8_t *s_EinkTransformFrameCoordinateSystem_4Bpp(const std::uint8_t *dataIn,
                                                               std::uint16_t windowWidthPx,
                                                               std::uint16_t windowHeightPx,
                                                               std::uint8_t *dataOut,
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
static std::uint8_t *s_EinkTransformAnimationFrameCoordinateSystem_1Bpp(const std::uint8_t *dataIn,
                                                                        std::uint16_t windowWidthPx,
                                                                        std::uint16_t windowHeightPx,
                                                                        std::uint8_t *dataOut,
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
static std::uint8_t *s_EinkTransformAnimationFrameCoordinateSystem_2Bpp(const std::uint8_t *dataIn,
                                                                        std::uint16_t windowWidthPx,
                                                                        std::uint16_t windowHeightPx,
                                                                        std::uint8_t *dataOut,
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
static std::uint8_t *s_EinkTransformAnimationFrameCoordinateSystem_3Bpp(const std::uint8_t *dataIn,
                                                                        std::uint16_t windowWidthPx,
                                                                        std::uint16_t windowHeightPx,
                                                                        std::uint8_t *dataOut,
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

static std::uint8_t *s_EinkTransformFrameCoordinateSystemNoRotation_4Bpp(const std::uint8_t *dataIn,
                                                                         std::uint16_t windowWidthPx,
                                                                         std::uint16_t windowHeightPx,
                                                                         std::uint8_t *dataOut,
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

    if (BSP_EinkWriteData(tmpbuf, sizeof(tmpbuf), SPI_AUTOMATIC_CS) != 0) {
        return;
    }
}

bool EinkIsPoweredOn()
{
    return s_einkIsPoweredOn;
}

EinkStatus_e EinkPowerOn()
{
    if (s_einkIsPoweredOn) {
        return EinkOK;
    }

    std::uint8_t cmd = EinkPowerON; // 0x04
    if (BSP_EinkWriteData(&cmd, sizeof(cmd), SPI_AUTOMATIC_CS) != 0) {
        EinkPowerDown();
        return EinkDMAErr;
    }

    s_einkIsPoweredOn = true;

    if (BSP_EinkWaitUntilDisplayBusy(pdMS_TO_TICKS(BSP_EinkBusyTimeout)) == 0) {
        return EinkTimeout;
    }

    return EinkOK;
}

EinkStatus_e EinkPowerOff()
{
    if (!s_einkIsPoweredOn) {
        return EinkOK;
    }

    std::uint8_t cmd = EinkPowerOFF; // 0x02
    if (BSP_EinkWriteData(&cmd, sizeof(cmd), SPI_AUTOMATIC_CS) != 0) {
        return EinkDMAErr;
    }

    const auto ret = BSP_EinkWaitUntilDisplayBusy(pdMS_TO_TICKS(BSP_EinkBusyTimeout)) == 1 ? EinkOK : EinkTimeout;

    // continue procedure regardless result
    BSP_EinkLogicPowerOff();

    s_einkIsPoweredOn = false;
    return ret;
}

EinkStatus_e EinkPowerDown(void)
{
    const auto powerOffStatus = EinkPowerOff();
    BSP_EinkDeinit();
    return powerOffStatus;
}

std::int16_t EinkGetTemperatureInternal()
{
    std::uint8_t cmd;
    std::int8_t temp[2] = {0, 0};

    cmd = EinkTemperatureSensorCalibration;

    BSP_EinkWriteCS(BSP_Eink_CS_Clr);

    if (BSP_EinkWriteData(&cmd, sizeof(cmd), SPI_MANUAL_CS) != 0) {
        BSP_EinkWriteCS(BSP_Eink_CS_Set);
        EinkPowerDown();
        return -1;
    }

    if (BSP_EinkWaitUntilDisplayBusy(pdMS_TO_TICKS(BSP_EinkBusyTimeout)) == 0) {
        return -1;
    }

    if (BSP_EinkReadData(temp, sizeof(temp), SPI_MANUAL_CS) != 0) {
        BSP_EinkWriteCS(BSP_Eink_CS_Set);
        EinkPowerDown();
        return -1;
    }

    BSP_EinkWriteCS(BSP_Eink_CS_Set);

    // First byte of the temp describes the integer part of the temperature in degrees Celsius
    const std::int8_t temperatureInteger = temp[0];
    // The MSB bit of the second byte describes the fraction of the temperature. Bit value of 1 means .5 degree Celsius,
    // bit value of 0 means .0 degree Celsius
    // int8_t temperatureFraction =    ((temp[1] & 0x80) >> 7);

    return temperatureInteger;
}

static void s_EinkSetGateOrder()
{
    std::uint8_t buf[3];

    // Set the order of gate refreshing
    buf[0] = EinkGDOrderSetting;
    buf[1] = 0x02; // Magic value required by the ED028TC1 display manufacturer
    buf[2] = 0x00;
    if (BSP_EinkWriteData(buf, sizeof(buf), SPI_AUTOMATIC_CS) != 0) {
        EinkPowerDown();
        return;
    }
}

static EinkStatus_e s_EinkSetInitialConfig()
{
    // send initialization data
    unsigned char tmpbuf[10];

    tmpbuf[0] = EinkPowerSetting; // 0x01
    tmpbuf[1] = 0x03;
    tmpbuf[2] = 0x04;
    tmpbuf[3] = 0x00; // 0x06
    tmpbuf[4] = 0x00;
    if (BSP_EinkWriteData(tmpbuf, 5, SPI_AUTOMATIC_CS) != 0) {
        EinkPowerDown();
        return EinkDMAErr;
    }

    tmpbuf[0] = EinkPanelSetting;      // 0x00
    tmpbuf[1] = LUT_SEL | SHL | RST_N; // 0x25 -> _XON _RES0 LUT_SEL _DM - SHL _SPIWM RST_N
                                       // If 0x35 (DM - 1 is used (2bpp)) the SPI speed can be 25MHz
    tmpbuf[2] = 0x00;
    if (BSP_EinkWriteData(tmpbuf, 3, SPI_AUTOMATIC_CS) != 0) {
        EinkPowerDown();
        return EinkDMAErr;
    }

    tmpbuf[0] = EinkPowerSaving; // 0x26
    tmpbuf[1] = 0x82;            // B2
    if (BSP_EinkWriteData(tmpbuf, 2, SPI_AUTOMATIC_CS) != 0) {
        EinkPowerDown();
        return EinkDMAErr;
    }

    tmpbuf[0] = EinkPowerOFFSequenceSetting; // 0x03
    tmpbuf[1] = 0x01;                        // 0x00;//0x03;
    if (BSP_EinkWriteData(tmpbuf, 2, SPI_AUTOMATIC_CS) != 0) {
        EinkPowerDown();
        return EinkDMAErr;
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
        EinkPowerDown();
        return EinkDMAErr;
    }

    tmpbuf[0] = EinkPLLControl; // 0x30
    tmpbuf[1] = 0x0E;
    if (BSP_EinkWriteData(tmpbuf, 2, SPI_AUTOMATIC_CS) != 0) {
        EinkPowerDown();
        return EinkDMAErr;
    }

    tmpbuf[0] = EinkTemperatureSensorSelection;              // Temp. sensor setting TSE
    tmpbuf[1] = EINK_TEMPERATURE_SENSOR_USE_INTERNAL | 0x00; // Temperature offset
    tmpbuf[2] = 0x00;                                        // Host forced temperature value
    if (BSP_EinkWriteData(tmpbuf, 3, SPI_AUTOMATIC_CS) != 0) {
        EinkPowerDown();
        return EinkDMAErr;
    }

    tmpbuf[0] = EinkVcomAndDataIntervalSetting; // 0x50
    tmpbuf[1] = DDX;                            // 0x01;   // 0x0D
    tmpbuf[2] = 0x00;                           // 0x22;
    if (BSP_EinkWriteData(tmpbuf, 3, SPI_AUTOMATIC_CS) != 0) {
        EinkPowerDown();
        return EinkDMAErr;
    }

    tmpbuf[0] = EinkTCONSetting; // 0x60
    tmpbuf[1] = 0x3F;
    tmpbuf[2] = 0x09;
    tmpbuf[3] = 0x2D;
    if (BSP_EinkWriteData(tmpbuf, 4, SPI_AUTOMATIC_CS) != 0) {
        EinkPowerDown();
        return EinkDMAErr;
    }

    tmpbuf[0] = EinkResolutionSetting; // 0x61
    tmpbuf[1] = 0x02;                  // 0x02
    tmpbuf[2] = 0x60;                  // 0x60
    tmpbuf[3] = 0x01;                  // 0x01
    tmpbuf[4] = 0xE0;                  // 0xE0
    if (BSP_EinkWriteData(tmpbuf, 5, SPI_AUTOMATIC_CS) != 0) {
        EinkPowerDown();
        return EinkDMAErr;
    }

    tmpbuf[0] = EinkVCM_DCSetting; // 0x82
    tmpbuf[1] = 0x30;
    if (BSP_EinkWriteData(tmpbuf, 2, SPI_AUTOMATIC_CS) != 0) {
        EinkPowerDown();
        return EinkDMAErr;
    }
    return EinkOK;
}

EinkStatus_e EinkResetAndInitialize()
{
    BSP_EinkLogicPowerOn();

    // Initialize the synchronization resources, SPI and GPIOs for the Eink BSP
    if (BSP_EinkInit() != 0) {
        EinkPowerDown();
        return EinkInitErr;
    }
    // Reset the display
    BSP_EinkResetDisplayController();
    // Set the initial configuration of the eink registers after reset
    if (s_EinkSetInitialConfig() != EinkOK) {
        EinkPowerDown();
        return EinkInitErr;
    }

    // After the reset the display is powered off
    s_einkIsPoweredOn = false;
    return EinkOK;
}

EinkStatus_e EinkUpdateWaveform(const EinkWaveformSettings_t *settings)
{
    /// LUTD
    if (BSP_EinkWriteData(settings->LUTDData, settings->LUTDSize, SPI_AUTOMATIC_CS) != 0) {
        EinkPowerDown();
        return EinkDMAErr;
    }

    /// LUTC
    if (BSP_EinkWriteData(settings->LUTCData, settings->LUTCSize + 1, SPI_AUTOMATIC_CS) != 0) {
        EinkPowerDown();
        return EinkDMAErr;
    }

    s_einkConfiguredWaveform = settings->mode;

    return EinkOK;
}

static EinkStatus_e s_EinkReadFlagsRegister(std::uint16_t *flags)
{
    std::uint8_t cmd = EinkFLG;

    BSP_EinkWriteCS(BSP_Eink_CS_Clr);

    if (BSP_EinkWriteData(&cmd, sizeof(cmd), SPI_MANUAL_CS) != 0) {
        BSP_EinkWriteCS(BSP_Eink_CS_Set);
        EinkPowerDown();
        return EinkDMAErr;
    }

    if (BSP_EinkReadData(flags, sizeof(std::uint16_t), SPI_MANUAL_CS) != 0) {
        BSP_EinkWriteCS(BSP_Eink_CS_Set);
        EinkPowerDown();
        return EinkDMAErr;
    }

    BSP_EinkWriteCS(BSP_Eink_CS_Set);

    return EinkOK;
}

EinkStatus_e EinkWaitTillPipelineBusy()
{
    std::uint16_t flags = 0;

    s_EinkReadFlagsRegister(&flags);

    while ((flags & EINK_FLAG_PIPELINE_BUSY)) {
        vTaskDelay(pdMS_TO_TICKS(1));
        s_EinkReadFlagsRegister(&flags);
    }

    return EinkOK;
}

EinkStatus_e EinkDitherDisplay()
{
    std::uint8_t cmdWithArgs[2] = {EinkDPC, EINK_DITHER_4BPP_MODE | EINK_DITHER_START};

    if (BSP_EinkWriteData(cmdWithArgs, sizeof(cmdWithArgs), SPI_AUTOMATIC_CS) != 0) {
        EinkPowerDown();
        return EinkDMAErr;
    }

    std::uint16_t flags = 0;

    s_EinkReadFlagsRegister(&flags);

    if ((flags & EINK_FLAG_DITHER_IN_PROGRESS)) {
        return EinkSPIErr;
    }

    return EinkOK;
}

EinkStatus_e EinkUpdateFrame(EinkFrame_t frame,
                             const std::uint8_t *buffer,
                             EinkBpp_e bpp,
                             EinkDisplayColorMode_e invertColors)
{
    std::uint8_t buf[10];
    std::uint8_t pixelsInByte = 8 / bpp;

    s_einkServiceRotatedBuf[0] = EinkDataStartTransmission1;
    s_einkServiceRotatedBuf[1] = bpp - 1; //  0 - 1Bpp, 1 - 2Bpp, 2 - 3Bpp, 3 - 4Bpp

    if ((s_einkConfiguredWaveform == EinkWaveformA2) || (s_einkConfiguredWaveform == EinkWaveformDU2)) {
        switch (bpp) {
        case Eink1Bpp: {
            s_EinkTransformAnimationFrameCoordinateSystem_1Bpp(
                buffer, frame.width, frame.height, s_einkServiceRotatedBuf + 2, invertColors);
        } break;
        case Eink2Bpp: {
            s_EinkTransformAnimationFrameCoordinateSystem_2Bpp(
                buffer, frame.width, frame.height, s_einkServiceRotatedBuf + 2, invertColors);
        } break;
        case Eink3Bpp: {
            s_EinkTransformAnimationFrameCoordinateSystem_3Bpp(
                buffer, frame.width, frame.height, s_einkServiceRotatedBuf + 2, invertColors);
        } break;
        case Eink4Bpp: {
#if defined(EINK_ROTATE_90_CLOCKWISE)
            s_EinkTransformFrameCoordinateSystem_4Bpp(
                buffer, frame.width, frame.height, s_einkServiceRotatedBuf + 2, invertColors);
#else
            s_EinkTransformFrameCoordinateSystemNoRotation_4Bpp(
                buffer, frame.width, frame.height, s_einkServiceRotatedBuf + 2, invertColors);
#endif
        } break;
        }
    }
    else {
        switch (bpp) {
        case Eink1Bpp: {
            s_EinkTransformFrameCoordinateSystem_1Bpp(
                buffer, frame.width, frame.height, s_einkServiceRotatedBuf + 2, invertColors);
        } break;
        case Eink2Bpp: {
            s_EinkTransformFrameCoordinateSystem_2Bpp(
                buffer, frame.width, frame.height, s_einkServiceRotatedBuf + 2, invertColors);
        } break;
        case Eink3Bpp: {
            s_EinkTransformFrameCoordinateSystem_3Bpp(
                buffer, frame.width, frame.height, s_einkServiceRotatedBuf + 2, invertColors);
        } break;
        case Eink4Bpp: {
#if defined(EINK_ROTATE_90_CLOCKWISE)
            s_EinkTransformFrameCoordinateSystem_4Bpp(
                buffer, frame.width, frame.height, s_einkServiceRotatedBuf + 2, invertColors);
#else
            s_EinkTransformFrameCoordinateSystemNoRotation_4Bpp(
                buffer, frame.width, frame.height, s_einkServiceRotatedBuf + 2, invertColors);
#endif
        } break;
        }
    }

    buf[0] = EinkDataStartTransmissionWindow; // set display window
    buf[1] = static_cast<std::uint8_t>(hal::eink::getDisplayXAxis(frame) >>
                                       8); // MSB of the X axis in the EPD display. Value converted
                                           // from the standard GUI coords system to the ED028TC1 one
    buf[2] = static_cast<std::uint8_t>(
        hal::eink::getDisplayXAxis(frame)); // LSB of the X axis in the EPD display. Value converted from
                                            // the standard GUI coords system to the ED028TC1 one
    buf[3] = static_cast<std::uint8_t>(hal::eink::getDisplayYAxis(frame) >>
                                       8); // MSB of the Y axis in the EPD display. Value converted
                                           // from the standard GUI coords system to the ED028TC1 one
    buf[4] = static_cast<std::uint8_t>(
        hal::eink::getDisplayYAxis(frame)); // LSB of the Y axis in the EPD display. Value converted from
                                            // the standard GUI coords system to the ED028TC1 one
    buf[5] = static_cast<std::uint8_t>(hal::eink::getDisplayWindowWidth(frame) >>
                                       8); // MSB of the window height in the EPD display. Value converted
                                           // from the standard GUI coords system to the ED028TC1 one
    buf[6] = static_cast<std::uint8_t>(
        hal::eink::getDisplayWindowWidth(frame)); // LSB of the window height in the EPD display. Value converted from
                                                  // the standard GUI coords system to the ED028TC1 one
    buf[7] = static_cast<std::uint8_t>(hal::eink::getDisplayWindowHeight(frame) >>
                                       8); // MSB of the window width in the EPD display. Value converted
                                           // from the standard GUI coords system to the ED028TC1 one
    buf[8] = static_cast<std::uint8_t>(
        hal::eink::getDisplayWindowHeight(frame)); // LSB of the window width in the EPD display. Value converted from
    // the standard GUI coords system to the ED028TC1 one

    if (BSP_EinkWriteData(buf, 9, SPI_AUTOMATIC_CS) != 0) {
        EinkPowerDown();
        return EinkDMAErr;
    }

    std::uint32_t msgSize = 2 + (static_cast<std::uint32_t>(frame.width) * static_cast<std::uint32_t>(frame.height) /
                                 pixelsInByte); // command (1 byte) + bpp (1 byte) + dataSize(W*H/pixelsInByte bytes)
    // Send the part of the image to the display memory

    if (BSP_EinkWriteData(s_einkServiceRotatedBuf, msgSize, SPI_AUTOMATIC_CS) != 0) {
        EinkPowerDown();
        return EinkDMAErr;
    }

    return (EinkOK);
}

EinkStatus_e EinkFillScreenWithColor(EinkDisplayColorFilling_e colorFill)
{
    std::uint8_t buf[10];

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
        EinkPowerDown();
        return EinkDMAErr;
    }

    BSP_EinkWriteCS(BSP_Eink_CS_Clr);

    buf[0] = EinkDataStartTransmission1; // 0x10
    buf[1] = Eink1Bpp - 1;
    if (BSP_EinkWriteData(buf, 2, SPI_MANUAL_CS) != 0) {
        BSP_EinkWriteCS(BSP_Eink_CS_Set);
        EinkPowerDown();
        return EinkDMAErr;
    }

    std::uint8_t background = colorFill;

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
        EinkPowerDown();
        return EinkDMAErr;
    }

    BSP_EinkWriteCS(BSP_Eink_CS_Set);

    return EinkRefreshImage(EinkFrame_t{0, 0, BOARD_EINK_DISPLAY_RES_X, BOARD_EINK_DISPLAY_RES_Y},
                            EinkDisplayTimingsDeepCleanMode);
}

EinkStatus_e EinkRefreshImage(EinkFrame_t frame, EinkDisplayTimingsMode_e refreshTimingsMode)
{
    EinkChangeDisplayUpdateTimings(refreshTimingsMode);

    s_EinkSetGateOrder();

    std::uint8_t buf[10];

    buf[0] = EinkDisplayRefresh;
    buf[1] = UPD_CPY_TO_PRE;

    buf[2] = static_cast<std::uint8_t>(hal::eink::getDisplayXAxis(frame) >>
                                       8); // MSB of the X axis in the EPD display. Value converted
                                           // from the standard GUI coords system to the ED028TC1 one
    buf[3] = static_cast<std::uint8_t>(
        hal::eink::getDisplayXAxis(frame)); // LSB of the X axis in the EPD display. Value converted from
                                            // the standard GUI coords system to the ED028TC1 one
    buf[4] = static_cast<std::uint8_t>(hal::eink::getDisplayYAxis(frame) >>
                                       8); // MSB of the Y axis in the EPD display. Value converted
                                           // from the standard GUI coords system to the ED028TC1 one
    buf[5] = static_cast<std::uint8_t>(
        hal::eink::getDisplayYAxis(frame)); // LSB of the Y axis in the EPD display. Value converted from
                                            // the standard GUI coords system to the ED028TC1 one
    buf[6] = static_cast<std::uint8_t>(hal::eink::getDisplayWindowWidth(frame) >>
                                       8); // MSB of the window height in the EPD display. Value converted
                                           // from the standard GUI coords system to the ED028TC1 one
    buf[7] = static_cast<std::uint8_t>(
        hal::eink::getDisplayWindowWidth(frame)); // LSB of the window height in the EPD display. Value converted from
                                                  // the standard GUI coords system to the ED028TC1 one
    buf[8] = static_cast<std::uint8_t>(hal::eink::getDisplayWindowHeight(frame) >>
                                       8); // MSB of the window width in the EPD display. Value converted
                                           // from the standard GUI coords system to the ED028TC1 one
    buf[9] = static_cast<std::uint8_t>(
        hal::eink::getDisplayWindowHeight(frame)); // LSB of the window width in the EPD display. Value converted from
    // the standard GUI coords system to the ED028TC1 one

    if (BSP_EinkWriteData(buf, sizeof(buf), SPI_AUTOMATIC_CS) != 0) {
        EinkPowerDown();
        return EinkDMAErr;
    }

    if (BSP_EinkWaitUntilDisplayBusy(pdMS_TO_TICKS(BSP_EinkBusyTimeout)) == 0) {
        return EinkTimeout;
    }

    return EinkOK;
}

__attribute__((optimize("O3"))) void EinkARGBToLuminance(const std::uint8_t *dataIn,
                                                         std::uint8_t *dataOut,
                                                         std::uint32_t displayWidth,
                                                         std::uint32_t displayHeight)
{
    std::uint8_t r, g, b;
    float fi;
    std::uint32_t *src;
    std::uint8_t *dst;

    src = (std::uint32_t *)dataIn;
    dst = (std::uint8_t *)dataOut;

    for (std::uint32_t i = 0; i < (displayWidth * displayHeight);
         i += 2) // increase by 8 pixels - 32bit word is 8 pixels in 4BPP
    {
        *dst = 0x00000000;
        for (std::uint8_t j = 0; j < 8; j += 4) {
            r  = (std::uint8_t)((*(src)) >> 16);
            g  = (std::uint8_t)((*(src)) >> 8);
            b  = (std::uint8_t) * (src);
            fi = (r + g + b) / 3;

            *dst |= ((std::uint32_t)(floor(fi / 16))) << (4 - j);

            src++;
        }
        dst++;
    }
}

__attribute__((optimize("O1"))) static std::uint8_t *s_EinkTransformFrameCoordinateSystem_1Bpp(
    const std::uint8_t *dataIn,
    std::uint16_t windowWidthPx,
    std::uint16_t windowHeightPx,
    std::uint8_t *dataOut,
    EinkDisplayColorMode_e invertColors)
{
    // In 1bpp mode there are 8 pixels in the byte
    const std::uint8_t pixelsInByte = 8;

    std::uint8_t pixels    = 0;
    std::uint8_t *outArray = dataOut;

    for (std::int32_t inputCol = windowWidthPx - 1; inputCol >= 0; --inputCol) {
        for (std::int32_t inputRow = windowHeightPx - 1; inputRow >= 7; inputRow -= pixelsInByte) {
            // HACK: Did not create the loop for accessing pixels and merging them in the single byte for better
            // performance.
            //       Wanted to avoid unneeded loop count increasing and jump operations which for large amount of data
            //       take considerable amount of time.
            std::uint32_t index = inputRow * BOARD_EINK_DISPLAY_RES_X + inputCol;

            // Use the LUT to convert the input pixel from 4bpp to 1bpp
            std::uint8_t firstPixel   = s_einkMaskLut_1Bpp[dataIn[index - 0 * BOARD_EINK_DISPLAY_RES_X]];
            std::uint8_t secondPixel  = s_einkMaskLut_1Bpp[dataIn[index - 1 * BOARD_EINK_DISPLAY_RES_X]];
            std::uint8_t thirdPixel   = s_einkMaskLut_1Bpp[dataIn[index - 2 * BOARD_EINK_DISPLAY_RES_X]];
            std::uint8_t fourthPixel  = s_einkMaskLut_1Bpp[dataIn[index - 3 * BOARD_EINK_DISPLAY_RES_X]];
            std::uint8_t fifthPixel   = s_einkMaskLut_1Bpp[dataIn[index - 4 * BOARD_EINK_DISPLAY_RES_X]];
            std::uint8_t sixthPixel   = s_einkMaskLut_1Bpp[dataIn[index - 5 * BOARD_EINK_DISPLAY_RES_X]];
            std::uint8_t seventhPixel = s_einkMaskLut_1Bpp[dataIn[index - 6 * BOARD_EINK_DISPLAY_RES_X]];
            std::uint8_t eightPixel   = s_einkMaskLut_1Bpp[dataIn[index - 7 * BOARD_EINK_DISPLAY_RES_X]];

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

__attribute__((optimize("O1"))) static std::uint8_t *s_EinkTransformFrameCoordinateSystem_2Bpp(
    const std::uint8_t *dataIn,
    std::uint16_t windowWidthPx,
    std::uint16_t windowHeightPx,
    std::uint8_t *dataOut,
    EinkDisplayColorMode_e invertColors)
{
    // In 2bpp mode there are 4 pixels in the byte
    const std::uint8_t pixelsInByte = 8;
    std::uint16_t pixels            = 0;
    std::uint16_t *outArray         = (std::uint16_t *)dataOut;
    std::uint8_t temp               = 0;

    for (std::int32_t inputCol = windowWidthPx - 1; inputCol >= 0; --inputCol) {
        for (std::int32_t inputRow = windowHeightPx - 1; inputRow >= 7; inputRow -= pixelsInByte) {
            // HACK: Did not create the loop for accessing pixels and merging them in the single byte for better
            // performance.
            //       Wanted to avoid unneeded loop count increasing and jump operations which for large amount of data
            //       take considerable amount of time.
            std::uint32_t index = inputRow * BOARD_EINK_DISPLAY_RES_X + inputCol;

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

__attribute__((optimize("O1"))) static std::uint8_t *s_EinkTransformFrameCoordinateSystem_3Bpp(
    const std::uint8_t *dataIn,
    std::uint16_t windowWidthPx,
    std::uint16_t windowHeightPx,
    std::uint8_t *dataOut,
    EinkDisplayColorMode_e invertColors)
{
    // The 4bpp is coded the same way as the 3bpp
    return s_EinkTransformFrameCoordinateSystem_4Bpp(dataIn, windowWidthPx, windowHeightPx, dataOut, invertColors);
}

__attribute__((optimize("O1"))) static std::uint8_t *s_EinkTransformAnimationFrameCoordinateSystem_1Bpp(
    const std::uint8_t *dataIn,
    std::uint16_t windowWidthPx,
    std::uint16_t windowHeightPx,
    std::uint8_t *dataOut,
    EinkDisplayColorMode_e invertColors)
{
    // In 1bpp mode there are 8 pixels in the byte
    const std::uint8_t pixelsInByte = 8;
    std::uint8_t pixels             = 0;
    std::uint8_t *outArray          = dataOut;

    for (std::int32_t inputCol = windowWidthPx - 1; inputCol >= 0; --inputCol) {
        for (std::int32_t inputRow = windowHeightPx - 1; inputRow >= 7; inputRow -= pixelsInByte) {
            // HACK: Did not create the loop for accessing pixels and merging them in the single byte for better
            // performance.
            //       Wanted to avoid unneeded loop count increasing and jump operations which for large amount of data
            //       take considerable amount of time.

            std::uint32_t index = inputRow * BOARD_EINK_DISPLAY_RES_X + inputCol;

            // Use the LUT to convert the input pixel from 4bpp to 1bpp
            std::uint8_t firstPixel   = s_einkMaskLut_1Bpp[dataIn[index - 0 * BOARD_EINK_DISPLAY_RES_X]];
            std::uint8_t secondPixel  = s_einkMaskLut_1Bpp[dataIn[index - 1 * BOARD_EINK_DISPLAY_RES_X]];
            std::uint8_t thirdPixel   = s_einkMaskLut_1Bpp[dataIn[index - 2 * BOARD_EINK_DISPLAY_RES_X]];
            std::uint8_t fourthPixel  = s_einkMaskLut_1Bpp[dataIn[index - 3 * BOARD_EINK_DISPLAY_RES_X]];
            std::uint8_t fifthPixel   = s_einkMaskLut_1Bpp[dataIn[index - 4 * BOARD_EINK_DISPLAY_RES_X]];
            std::uint8_t sixthPixel   = s_einkMaskLut_1Bpp[dataIn[index - 5 * BOARD_EINK_DISPLAY_RES_X]];
            std::uint8_t seventhPixel = s_einkMaskLut_1Bpp[dataIn[index - 6 * BOARD_EINK_DISPLAY_RES_X]];
            std::uint8_t eightPixel   = s_einkMaskLut_1Bpp[dataIn[index - 7 * BOARD_EINK_DISPLAY_RES_X]];

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

__attribute__((optimize("O1"))) static std::uint8_t *s_EinkTransformAnimationFrameCoordinateSystem_2Bpp(
    const std::uint8_t *dataIn,
    std::uint16_t windowWidthPx,
    std::uint16_t windowHeightPx,
    std::uint8_t *dataOut,
    EinkDisplayColorMode_e invertColors)
{
    // In 2bpp mode there are 4 pixels in the byte
    const std::uint8_t pixelsInByte = 8;
    std::uint16_t pixels            = 0;
    std::uint16_t *outArray         = (std::uint16_t *)dataOut;
    std::uint8_t temp               = 0;
    for (std::int32_t inputCol = windowWidthPx - 1; inputCol >= 0; --inputCol) {
        for (std::int32_t inputRow = windowHeightPx - 1; inputRow >= 7; inputRow -= pixelsInByte) {
            // HACK: Did not create the loop for accessing pixels and merging them in the single byte for better
            // performance.
            //       Wanted to avoid unneeded loop count increasing and jump operations which for large amount of data
            //       take considerable amount of time.
            std::uint32_t index = inputRow * BOARD_EINK_DISPLAY_RES_X + inputCol;

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

__attribute__((optimize("O3"))) static std::uint8_t *s_EinkTransformAnimationFrameCoordinateSystem_3Bpp(
    const std::uint8_t *dataIn,
    std::uint16_t windowWidthPx,
    std::uint16_t windowHeightPx,
    std::uint8_t *dataOut,
    EinkDisplayColorMode_e invertColors)
{
    // The 4bpp is coded the same way as the 3bpp
    return s_EinkTransformFrameCoordinateSystem_4Bpp(dataIn, windowWidthPx, windowHeightPx, dataOut, invertColors);
}

__attribute__((optimize("O1"))) static std::uint8_t *s_EinkTransformFrameCoordinateSystem_4Bpp(
    const std::uint8_t *dataIn,
    std::uint16_t windowWidthPx,
    std::uint16_t windowHeightPx,
    std::uint8_t *dataOut,
    EinkDisplayColorMode_e invertColors)
{
    // In 3bpp and 4bpp modes there are 2 pixels in the byte. Using 8bpp to process the whole uint32_t at once for
    // faster execution
    const std::uint8_t pixelsInByte = 8;

    std::uint32_t pixels    = 0;
    std::uint32_t *outArray = (std::uint32_t *)dataOut;

    for (std::int32_t inputCol = windowWidthPx - 1; inputCol >= 0; --inputCol) {
        for (std::int32_t inputRow = windowHeightPx - 1; inputRow >= 7; inputRow -= pixelsInByte) {
            // HACK: Did not create the loop for accessing pixels and merging them in the single byte for better
            // performance.
            //       Wanted to avoid unneeded loop count increasing and jump operations which for large amount of data
            //       take considerable amount of time. Using 8 pixels at a time for better performance
            std::uint32_t index = inputRow * BOARD_EINK_DISPLAY_RES_X + inputCol;

            std::uint8_t firstPixelPair =
                (dataIn[index - 0 * BOARD_EINK_DISPLAY_RES_X] << 4) | dataIn[index - 1 * BOARD_EINK_DISPLAY_RES_X];
            std::uint8_t secondPixelPair =
                (dataIn[index - 2 * BOARD_EINK_DISPLAY_RES_X] << 4) | dataIn[index - 3 * BOARD_EINK_DISPLAY_RES_X];
            std::uint8_t thirdPixelPair =
                (dataIn[index - 4 * BOARD_EINK_DISPLAY_RES_X] << 4) | dataIn[index - 5 * BOARD_EINK_DISPLAY_RES_X];
            std::uint8_t fourthPixelPair =
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

__attribute__((optimize("O1"))) static std::uint8_t *s_EinkTransformFrameCoordinateSystemNoRotation_4Bpp(
    const std::uint8_t *dataIn,
    std::uint16_t windowWidthPx,
    std::uint16_t windowHeightPx,
    std::uint8_t *dataOut,
    EinkDisplayColorMode_e invertColors)
{
    // In 3bpp and 4bpp modes there are 2 pixels in the byte. Using 8bpp to process the whole uint32_t at once for
    // faster execution
    const std::uint8_t pixelsInByte = 8;

    std::uint32_t pixels    = 0;
    std::uint32_t *outArray = (std::uint32_t *)dataOut;
    std::int32_t inputRow   = 0;
    std::int32_t inputCol   = 0;

    for (inputRow = 0; inputRow < windowHeightPx; ++inputRow) {
        for (inputCol = windowWidthPx - 7; inputCol >= 0; inputCol -= pixelsInByte) {
            // HACK: Did not create the loop for accessing pixels and merging them in the single byte for better
            // performance.
            //       Wanted to avoid unneeded loop count increasing and jump operations which for large amount of data
            //       take considerable amount of time. Using 8 pixels at a time for better performance
            std::uint32_t index = inputRow * BOARD_EINK_DISPLAY_RES_X + inputCol;

            // Get 4x 2 adjacent pixels to process them as uint32_t for better execution timings
            std::uint8_t firstPixelPair  = (dataIn[index]) | (dataIn[index + 1] << 4);
            std::uint8_t secondPixelPair = (dataIn[index + 2]) | (dataIn[index + 3] << 4);
            std::uint8_t thirdPixelPair  = (dataIn[index + 4]) | (dataIn[index + 5] << 4);
            std::uint8_t fourthPixelPair = (dataIn[index + 6]) | (dataIn[index + 7] << 4);

            // Put the pixels in the uint32_t for faster processing
            pixels = (firstPixelPair << 24) | (secondPixelPair << 16) | (thirdPixelPair << 8) | (fourthPixelPair);

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
