// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

/**
 * @file ED028TC1.h
 * @author Lukasz Skrzypczak (l.skrzypczak@mudita.com)
 * @date Sep 6, 2017
 * @brief Header for EInk ED028TC1 electronic paper display driver
 * @copyright Copyright (C) 2017 mudita.com.
 * @details This is hardware specific electronic paper display ED028TC1 driver.
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ED028TC1_H
#define __ED028TC1_H

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdbool.h>
#include <functional>
#include "FreeRTOS.h"
#include "semphr.h"

#if defined(__cplusplus)
extern "C"
{
#endif /* __cplusplus */

/* Exported macro ------------------------------------------------------------*/
/**
 * @brief ED028TC1 register definitions
 */
#define EinkPanelSetting            (0x00U)
#define EinkPowerSetting            (0x01U)
#define EinkPowerOFF                (0x02U)
#define EinkPowerOFFSequenceSetting (0x03U)
#define EinkPowerON                 (0x04U)
#define EinkDPC                     (0x05U)
#define EinkDSLP                    (0x06U)
#define EinkBoosterSoftStart        (0x07U)

#define EinkDataStartTransmission1 (0x10U)
#define EinkDisplayRefresh         (0x12U)
#define EinkDTM2                   (0x13U)
#define EinkAWM1                   (0x15U)
#define EinkAWM2                   (0x16U)

#define EinkLUTC        (0x20U)
#define EinkLUTD        (0x21U)
#define EinkLUTR        (0x22U)
#define EinkPowerSaving (0x26U)
#define EinkMISCS       (0x27U)

#define EinkPLLControl (0x30U)

#define EinkTemperatureSensorCalibration (0x40U)
#define EinkTemperatureSensorSelection   (0x41U)

#define EinkVcomAndDataIntervalSetting (0x50U)
#define EinkLPD                        (0x51U)

#define EinkTCONSetting       (0x60U)
#define EinkResolutionSetting (0x61U)
#define EinkDAM               (0x65U)

#define EinkREV          (0x70U)
#define EinkFLG          (0x71U)
#define EinkLUT_COL_FLG  (0x72U)
#define EinkLUT_BUSY_FLG (0x73U)

#define EinkAutoMeasurementVcom         (0x80U)
#define EinkReadVcomValue               (0x81U)
#define EinkVCM_DCSetting               (0x82U)
#define EinkDataStartTransmissionWindow (0x83U)
#define EinkEDS                         (0x84U)
#define EinkXONS                        (0x85U)

#define EinkLEDDS (0x90U)
#define EinkPBC   (0x91U)
#define EinkPBCS  (0x92U)
#define EinkEXTRS (0x93U)
#define EinkNTRS  (0x94U)

#define EinkGDOrderSetting (0xE0U)

/**
 * @brief ED028TC1 register bit definitions
 */
// PanelSetting
#define XON     (1 << 7)
#define RES0    (1 << 6)
#define LUT_SEL (1 << 5)
#define DM      (1 << 4)
#define SHL     (1 << 2)
#define SPIWM   (1 << 1)
#define RST_N   (1 << 0)
#define SFT1PX  (0 << 0)
#define SFT2PX  (1 << 0)
#define SFT3PX  (2 << 0)
#define SFT4PX  (3 << 0)

// PowerSetting
#define VSource_EN (1 << 0)
#define VGate_EN   (1 << 1)
#define VG_LVL17V  (0 << 0)
#define VG_LVL18V  (1 << 0)
#define VG_LVL19V  (2 << 0)
#define VG_LVL20V  (3 << 0)
#define VG_LVL21V  (4 << 0)
#define VSLV_LVL   (0) // this is: 7-bit 00h=2.4V, 7Fh=15.0V
#define VSL_LVL15V (0 << 2)
#define VSL_LVL14V (1 << 2)
#define VSL_LVL13V (2 << 2)
#define VSL_LVL12V (3 << 2)
#define VSH_LVL15V (0 << 0)
#define VSH_LVL14V (1 << 0)
#define VSH_LVL13V (2 << 0)
#define VSH_LVL12V (3 << 0)

// PowerOFFSequenceSetting
#define T_VDS_OFF_1F (0 << 4)
#define T_VDS_OFF_2F (1 << 4)
#define T_VDS_OFF_3F (2 << 4)
#define T_VDS_OFF_4F (3 << 4)

// BoosterSoftStart
#define BTPHx_SSP_10ms (0 << 6) // soft start period of phase A,B
#define BTPHx_SSP_20ms (1 << 6)
#define BTPHx_SSP_30ms (2 << 6)
#define BTPHx_SSP_40ms (3 << 6)
#define BTPHx_DS_1     (0 << 3) // driving strength of phase A,B,C
#define BTPHx_DS_2     (1 << 3)
#define BTPHx_DS_3     (2 << 3)
#define BTPHx_DS_4     (3 << 3)
#define BTPHx_DS_5     (4 << 3)
#define BTPHx_DS_6     (5 << 3)
#define BTPHx_DS_7     (6 << 3)
#define BTPHx_DS_8     (7 << 3)
#define BTPHx_OT_027   (0 << 0) // minimum OFF time setting of phase A,B,C
#define BTPHx_OT_034   (1 << 0)
#define BTPHx_OT_040   (2 << 0)
#define BTPHx_OT_054   (3 << 0)
#define BTPHx_OT_080   (4 << 0)
#define BTPHx_OT_154   (5 << 0)
#define BTPHx_OT_334   (6 << 0)
#define BTPHx_OT_658   (7 << 0)

#define EINK_TEMPERATURE_SENSOR_USE_INTERNAL (0 << 6)
#define EINK_TEMPERATURE_SENSOR_USE_EXTERNAL (2 << 6)

// DataStartTransmission1
#define Cur_BPP1 (0 << 0)
#define Cur_BPP2 (1 << 0)
#define Cur_BPP3 (2 << 0)
#define Cur_BPP4 (3 << 0)

// DisplayRefresh
#define AC_DCVCOM      (1 << 7)
#define WFMode0        (0 << 4)
#define WFMode1        (1 << 4)
#define WFMode2        (2 << 4)
#define WFMode3        (3 << 4)
#define WFMode4        (4 << 4)
#define UPD_CPY_TO_PRE (1 << 3)
#define DN_EN          (1 << 2)
#define Regal_EN_DIS   (0 << 0)
#define Regal_EN_K     (1 << 0)
#define Regal_EN_W     (2 << 0)
#define Regal_EN_KW    (3 << 0)

// PLLControl
#define OSC_RATE_SEL2_5 (0 << 1)
#define OSC_RATE_SEL5   (1 << 1)
#define OSC_RATE_SEL8   (2 << 1)
#define OSC_RATE_SEL10  (3 << 1)
#define OSC_RATE_SEL16  (4 << 1)
#define OSC_RATE_SEL18  (5 << 1)
#define OSC_RATE_SEL19  (6 << 1)
#define OSC_RATE_SEL20  (7 << 1)

// VcomAndDataIntervalSetting
#define VBD_CON        (1 << 3)
#define VBD_OT_G0_G0   (0 << 1)
#define VBD_OT_G0_G15  (2 << 1)
#define VBD_OT_G15_G0  (3 << 1)
#define VBD_OT_G15_G15 (4 << 1)
#define DDX            (1 << 0)
#define CDI            (4) // Vcom data interval: 0h=2hsync -> Fh=32hsync, step=2
#define DCI            (0) // Data to Vcom interval: 0h=1hsync -> Fh=16hsync, step=1

// AutoMeasurementVcom
#define VCM_EN  (1 << 6)
#define AMVT3s  (0 << 4)
#define AMVT5s  (1 << 4)
#define AMVT6s  (2 << 4)
#define AMVT10s (3 << 4)
#define AMVX    (1 << 3)
#define AMVS    (1 << 2)
#define AMV     (1 << 1)
#define AMVE    (1 << 0)

// GDOrderSetting
#define VBD_EN_SEL (1 << 3)
#define GDOS_M0    (0 << 0)
#define GDOS_M1    (1 << 0)
#define GDOS_M2    (2 << 0)
#define GDOS_M3    (3 << 0)
#define GDOS_M4    (4 << 0)
#define GDOS_M5    (5 << 0)
#define GDOS_M6    (6 << 0)
#define GDOS_M7    (7 << 0)
#define VBD_FN     (0) // VBorder frame number seting: 0-VBD disabled, 1-VBD=8, ... 1Fh-VBD=248

// Dither
#define EINK_DITHER_START     0x01
#define EINK_DITHER_4BPP_MODE (0 << 1)
#define EINK_DITHER_2BPP_MODE (1 << 1)

#define EINK_FLAG_BUSY_N 0x0001 ///< This flag informs that the driver is busy. 0 - busy, 1 - idle
#define EINK_FLAG_POWER_OFF_IN_PROGRESS                                                                                \
    0x0002 ///< This flag informs that the Power Off sequence is in progress. 1 - in progress
#define EINK_FLAG_POWER_ON_IN_PROGRESS                                                                                 \
    0x0004 ///< This flag informs that the Power On sequence is in progress. 1 - in progress
#define EINK_FLAG_ENTIRE_FRAME_RECEIVED                                                                                \
    0x0008 ///< This flag informs that the driver received data for entire frame defined in the \ref
           ///< EinkDataStartTransmissionWindow command
#define EINK_FLAG_I2C_BUSY           0x0010 ///< This flag informs that the I2C master periph is busy. Active low
#define EINK_FLAG_I2C_ERROR          0x0020 ///< This flag informs that the I2C master acquired an error
#define EINK_FLAG_PIPELINE_COLLISION 0x0040 ///< This flag informs that two frames sent in the pipelin overlap
#define EINK_FLAG_PIPELINE_BUSY      0x0080 ///< This flag informs that pipeline insertion is in progress
#define EINK_FLAG_REAGL_BUSY         0x0100 ///< This flag informs that the REAGLS function processing is in progress
#define EINK_FLAG_DITHER_IN_PROGRESS 0x0200 ///< This flag informs that the Dither process is in progress
#define EINK_FLAG_DISP_REFRESH_IN_PROGRESS                                                                             \
    0x0400 ///< This flag informs that the Display Refreshing process is in progress
#define EINK_FLAG_AUTO_MEASURE_VCOM_IN_PROGRESS                                                                        \
    0x0800 ///< This flag informs that the auto measurement of the VCOM is in progress
#define EINK_FLAG_EPD_DISCHARGE_IN_PROGRESS 0x1000 ///< This flag informs that the EPD display discharge is in progress
#define EINK_FLAG_BOOST_VOLTAGE_READY       0x4000 ///< This flag informs that the Boost voltage is ready
#define EINK_FLAG_RAM_TEST_FLAG             0x8000 ///< This flag is for internal SRAM memory testing

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

    /**
     * @enum EinkBpp_e
     */
    typedef enum
    {
        Eink1Bpp = 1, //!< Eink1Bpp
        Eink2Bpp,     //!< Eink2Bpp
        Eink3Bpp,     //!< Eink3Bpp
        Eink4Bpp      //!< Eink4Bpp
    } EinkBpp_e;

    typedef enum
    {
        EinkWaveformINIT,  ///< Clears deeply the display
        EinkWaveformA2,    ///< Fastest, direct update, no flashing. Severe ghosting effect
        EinkWaveformDU2,   ///< Fast, direct update, no flashing. Medium ghosting effect
        EinkWaveformGLD16, ///< Slow, little flashing. Light ghosting mode
        EinkWaveformGC16,  ///< Slow, strong flashing. Next to none ghosting
    } EinkWaveforms_e;

    typedef enum
    {
        EinkDisplayTimingsDeepCleanMode,
        EinkDisplayTimingsHighContrastMode,
        EinkDisplayTimingsFastRefreshMode
    } EinkDisplayTimingsMode_e;

    typedef enum
    {
        EinkDisplayColorBlack = 0,
        EinkDisplayColorWhite = 0xFF
    } EinkDisplayColorFilling_e;

    typedef enum
    {
        EinkDisplayColorModeStandard,
        EinkDisplayColorModeInverted
    } EinkDisplayColorMode_e;

    typedef struct
    {
        // type of eink's waveform
        EinkWaveforms_e mode;
        // temperature of surrounding
        int32_t temperature;
        // counts usage of this waveform (display refreshes)
        uint32_t useCounter;
        // pointer to lookup table for lut c
        uint8_t *LUTCData;
        // sizeo of lutc data
        uint32_t LUTCSize;
        // pointer to lookup table for lut d
        uint8_t *LUTDData;
        // size of lutd data
        uint32_t LUTDSize;
    } EinkWaveformSettings_t;

    /* Exported constants --------------------------------------------------------*/

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
     * This function measures the ambient temperature using the ED028TC1 display internal temperature sensor.
     * @note The display needs to be powered on
     *
     * @return Ambient temperature in the degrees of Celsius
     */
    int16_t EinkGetTemperatureInternal();

    /**
     * @brief This function resets the eink display and setups the initial configuration
     */
    EinkStatus_e EinkResetAndInitialize();

    /**
     * TODO: Fill The doxy when got info what does it do
     * @return
     */
    EinkStatus_e EinkWaitTillPipelineBusy();

    /**
     * TODO: Fill The doxy when got info what does it do
     * @return
     */
    EinkStatus_e EinkDitherDisplay();

    /**
     * @brief This function sends the part of image from the given buffer to the internal memory of the display. It
     * makes not screen to update.
     * @param X [in] - image start position X in pixels
     * @param Y [in] - image start position Y in pixels
     * @param W [in] - image width in pixels
     * @param H [in] - image height in pixels
     * @param buffer [in] -  pointer to image encoded according to \ref bpp set in initialization
     * @param bpp [in] - The format of the \ref buffer (number of the bits per pixel)
     * @param invertColors[in] - true if colors of the image are to be inverted, false otherwise
     *
     * @return  EinkNoMem - Could not allocate the temporary buffer
     *          EinkOK - Part of image send successfully
     */
    EinkStatus_e EinkUpdateFrame(uint16_t X,
                                 uint16_t Y,
                                 uint16_t W,
                                 uint16_t H,
                                 uint8_t *buffer,
                                 EinkBpp_e bpp,
                                 EinkDisplayColorMode_e invertColors);

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
     * @param X refresh window position X in pixels
     * @param Y refresh window position Y in pixels
     * @param W refresh window width in pixels
     * @param H refresh window height in pixels
     * @param refreshTimingsMode [in] - EinkDisplayTimingsDeepCleanMode - if image is to be cleared precisely
     *                                  EinkDisplayTimingsHighContrastMode - if image is displayed in the high contrast
     * mode EinkDisplayTimingsFastRefreshMode - if image is to be displayed fast
     *
     * @return EinkOK
     */
    EinkStatus_e EinkRefreshImage(
        uint16_t X, uint16_t Y, uint16_t W, uint16_t H, EinkDisplayTimingsMode_e refreshTimingsMode);

    /**
     * @brief This function sends the proper waveform consisting from the LUTC and LUTD data,
     *          based on the requested waveform (see \ref Mode) and the given temperature (see \ref temperature)
     * @param Mode [in] - type of the waveform
     * @param temperature [in] - current ambient temperature in Celsius degrees
     * @param LUTCData [in] - Data
     * @return
     */
    EinkStatus_e EinkUpdateWaveform(const EinkWaveformSettings_t *settings);

    /**
     * This function converts the ARGB image to the L4 format
     * @param dataIn  [in]  - input image
     * @param dataOut [out] - output image
     * @param displayWidth    [in] - display width in pixels
     * @param displayHeight   [in] - display height in pixels
     */
    void EinkARGBToLuminance(uint8_t *dataIn, uint8_t *dataOut, uint32_t displayWidth, uint32_t displayHeight);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* __ED028TC1_H */

/******************* (C) COPYRIGHT 2017 mudita *****END OF FILE****/
