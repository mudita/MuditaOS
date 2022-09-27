// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

/**
 * @brief Header for EInk ED028TC1 electronic paper display driver
 * @details This is hardware specific electronic paper display ED028TC1 driver.
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ED028TC1_H
#define __ED028TC1_H

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdbool.h>
#include "FreeRTOS.h"
#include "semphr.h"
#include "fsl_edma.h"

#if defined(__cplusplus)
extern "C"
{
#endif /* __cplusplus */

    /* Exported types ------------------------------------------------------------*/
    /**
     * @enum EinkStatus_e
     */
    typedef enum
    {
        EinkOK, //!< EinkOK
        EinkError,
        EinkSPIErr,                //!< EinkSPIErr
        EinkSPINotInitializedErr,  //!< EinkSPINotInitializedErr
        EinkDMAErr,                //!< EinkDMAErr
        EinkInitErr,               //!< EinkInitErr
        EinkTimeout,               //!< Timeout occured while waiting for not busy signal from EINK
        EinkNoMem,                 //!< Could not allocate memory
        EinkWaveformsFileOpenFail, //!< Could not open the file with the waveforms for EPD display
    } EinkStatus_e;

    typedef enum
    {
        EinkDisplayColorBlack = 0,
        EinkDisplayColorWhite = 0xFF
    } EinkDisplayColorFilling_e;

    typedef struct
    {
        uint16_t pos_x;
        uint16_t pos_y;
        uint16_t width;
        uint16_t height;
    } EinkFrame_t;

    /* Exported functions ------------------------------------------------------- */

    /**
     * This function returns the state of the EPD siplay powe
     * @return 1 if is currently powered on, 0 otherwise
     */
    uint8_t EinkIsPoweredOn();

    /**
     * This function powers on the display. Needed for refreshing, measuring the temperature
     */
    void EinkPowerOn();

    /**
     * This functions powers off the display
     */
    void EinkPowerOff();

    /**
     * @brief This function is responsible for turning eink of and releasing all resources.
     */
    void EinkPowerDown(void);

    /**
     * @brief This function resets the eink display and setups the initial configuration
     */
    EinkStatus_e EinkResetAndInitialize();

    /**
     * @brief This function sends the part of image from the given buffer to the internal memory of the display. It
     * makes not screen to update.
     * @param frame [in] - draw buffer on specified part of screen
     * @param buffer [in] -  pointer to image encoded according to \ref bpp set in initialization
     *
     * @return  EinkNoMem - Could not allocate the temporary buffer
     *          EinkOK - Part of image send successfully
     */
    EinkStatus_e EinkUpdateFrame(EinkFrame_t frame, const uint8_t *buffer);

    /**
     * @brief This function sets the waveform to the \ref EinkWaveformINIT to make the display clearing more deep and
     * sends the white background
     * @param temperature [in] - current ambient temperature
     * @return EinkOK
     */
    // EinkStatus_e EinkClearScreenDeep (int8_t temperature);

    /**
     * @brief This function just displays the color (black or white) on the entire display. Used to clear the display
     * fast.
     * @param colorFill [in] - color which is used to fill the display
     * @return EinkOK
     */
    EinkStatus_e EinkFillScreenWithColor(EinkDisplayColorFilling_e colorFill);

    /**
     * @brief Refresh window on the screen. E-paper display tends to loose contrast over time. To Keep the image sharp
     * refresh is needed.
     * @param frame refresh specified part of screen
     *
     * @return EinkOK
     */
    EinkStatus_e EinkRefreshImage(EinkFrame_t frame);

    /**
     * This function converts the ARGB image to the L4 format
     * @param dataIn  [in]  - input image
     * @param dataOut [out] - output image
     * @param displayWidth    [in] - display width in pixels
     * @param displayHeight   [in] - display height in pixels
     */
    void EinkARGBToLuminance(uint8_t *dataIn, uint8_t *dataOut, uint32_t displayWidth, uint32_t displayHeight);

    EinkStatus_e EinkMemcpyDmaInit(edma_callback memcpyCallback);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* __ED028TC1_H */

/******************* (C) COPYRIGHT 2017 mudita *****END OF FILE****/
