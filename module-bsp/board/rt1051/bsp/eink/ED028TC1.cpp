// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "ED028TC1.hpp"
#include "EinkDimensions.hpp"
#include "macros.h"
#include "BspEink.hpp"
#include "EinkBufferTransformation.hpp"

#include <task.h>
#include <cstring>
#include <memory>
#include <log/log.hpp>

namespace bsp::eink
{
    namespace
    {
        constexpr std::uint8_t tconSettings[] = {0x3F, 0x09, 0x2D}; // Magic numbers we got from EInk
        constexpr std::size_t imageBufferSize =
            EINK_IMAGE_CONFIG_SIZE +
            ((BOARD_EINK_DISPLAY_RES_X * BOARD_EINK_DISPLAY_RES_Y) / (EINK_BITS_IN_BYTE / EINK_MAX_BPP));

        CACHEABLE_SECTION_SDRAM(std::uint8_t einkRotatedBuf[imageBufferSize]);

        auto poweredOn                   = false;              // Power state of EPD display
        auto currentlyConfiguredWaveform = EinkWaveform::GC16; // Current waveform set in the display

        void setGateOrder()
        {
            std::uint8_t buf[3];

            /* Set the order of gate refreshing */
            buf[0] = EinkGDOrderSetting;
            buf[1] = 0x02; // Magic value required by the ED028TC1 display manufacturer
            buf[2] = 0x00;
            if (writeData(buf, sizeof(buf), SPICSConfig::Automatic) != kStatus_Success) {
                powerDown();
                return;
            }
        }

        EinkStatus setInitialConfig()
        {
            std::uint8_t buf[10];

            buf[0] = EinkPowerSetting; // 0x01
            buf[1] = 0x03;
            buf[2] = 0x04;
            buf[3] = 0x00; // 0x06
            buf[4] = 0x00;
            if (writeData(buf, 5, SPICSConfig::Automatic) != kStatus_Success) {
                powerDown();
                return EinkStatus::DMAErr;
            }

            buf[0] = EinkPanelSetting;      // 0x00
            buf[1] = LUT_SEL | SHL | RST_N; // 0x25 -> _XON _RES0 LUT_SEL _DM - SHL _SPIWM RST_N
                                            // If 0x35 (DM - 1 is used (2bpp)) the SPI speed can be 25MHz
            buf[2] = 0x00;
            if (writeData(buf, 3, SPICSConfig::Automatic) != kStatus_Success) {
                powerDown();
                return EinkStatus::DMAErr;
            }

            buf[0] = EinkPowerSaving; // 0x26
            buf[1] = 0x82;            // B2
            if (writeData(buf, 2, SPICSConfig::Automatic) != kStatus_Success) {
                powerDown();
                return EinkStatus::DMAErr;
            }

            buf[0] = EinkPowerOFFSequenceSetting; // 0x03
            buf[1] = 0x01;                        // 0x00;//0x03;
            if (writeData(buf, 2, SPICSConfig::Automatic) != kStatus_Success) {
                powerDown();
                return EinkStatus::DMAErr;
            }

            buf[0] = EinkBoosterSoftStart; // 0x07
            buf[1] = (EPD_BOOSTER_OFF_TIME_GDR_6_58uS << EPD_BOOSTER_OFF_TIME_GDR_POS) |
                     (EPD_BOOSTER_DRIVING_STRENGTH_6 << EPD_BOOSTER_DRIVING_STRENGTH_POS) |
                     (EPD_BOOSTER_START_PERIOD_10MS << EPD_BOOSTER_START_PERIOD_POS);

            buf[2] = (EPD_BOOSTER_OFF_TIME_GDR_6_58uS << EPD_BOOSTER_OFF_TIME_GDR_POS) |
                     (EPD_BOOSTER_DRIVING_STRENGTH_6 << EPD_BOOSTER_DRIVING_STRENGTH_POS) |
                     (EPD_BOOSTER_START_PERIOD_10MS << EPD_BOOSTER_START_PERIOD_POS);

            buf[3] = (EPD_BOOSTER_OFF_TIME_GDR_0_40uS << EPD_BOOSTER_OFF_TIME_GDR_POS) |
                     (EPD_BOOSTER_DRIVING_STRENGTH_7 << EPD_BOOSTER_DRIVING_STRENGTH_POS) |
                     (EPD_BOOSTER_START_PERIOD_10MS << EPD_BOOSTER_START_PERIOD_POS);
            if (writeData(buf, 4, SPICSConfig::Automatic) != kStatus_Success) {
                powerDown();
                return EinkStatus::DMAErr;
            }

            buf[0] = EinkPLLControl; // 0x30
            buf[1] = 0x0E;
            if (writeData(buf, 2, SPICSConfig::Automatic) != kStatus_Success) {
                powerDown();
                return EinkStatus::DMAErr;
            }

            buf[0] = EinkTemperatureSensorSelection;       // Temp. sensor setting TSE
            buf[1] = EINK_TEMPERATURE_SENSOR_USE_INTERNAL; // Temperature offset
            buf[2] = 0x00;                                 // Host forced temperature value
            if (writeData(buf, 3, SPICSConfig::Automatic) != kStatus_Success) {
                powerDown();
                return EinkStatus::DMAErr;
            }

            buf[0] = EinkVcomAndDataIntervalSetting; // 0x50
            buf[1] = DDX;                            // 0x01;   // 0x0D
            buf[2] = 0x00;                           // 0x22;
            if (writeData(buf, 3, SPICSConfig::Automatic) != kStatus_Success) {
                powerDown();
                return EinkStatus::DMAErr;
            }

            buf[0] = EinkTCONSetting; // 0x60
            std::memcpy(&buf[1], tconSettings, sizeof(tconSettings));
            if (writeData(buf, 4, SPICSConfig::Automatic) != kStatus_Success) {
                powerDown();
                return EinkStatus::DMAErr;
            }

            buf[0] = EinkResolutionSetting; // 0x61
            buf[1] = 0x02;                  // 0x02
            buf[2] = 0x60;                  // 0x60
            buf[3] = 0x01;                  // 0x01
            buf[4] = 0xE0;                  // 0xE0
            if (writeData(buf, 5, SPICSConfig::Automatic) != kStatus_Success) {
                powerDown();
                return EinkStatus::DMAErr;
            }

            buf[0] = EinkVCM_DCSetting; // 0x82
            buf[1] = 0x30;
            if (writeData(buf, 2, SPICSConfig::Automatic) != kStatus_Success) {
                powerDown();
                return EinkStatus::DMAErr;
            }
            return EinkStatus::OK;
        }

        EinkStatus readFlagsRegister(std::uint16_t *flags)
        {
            std::uint8_t cmd = EinkFLG;

            writeCS(SPICSState::Clear);

            if (writeData(&cmd, sizeof(cmd), SPICSConfig::Manual) != kStatus_Success) {
                writeCS(SPICSState::Set);
                powerDown();
                return EinkStatus::DMAErr;
            }

            if (readData(flags, sizeof(*flags), SPICSConfig::Manual) != kStatus_Success) {
                writeCS(SPICSState::Set);
                powerDown();
                return EinkStatus::DMAErr;
            }

            writeCS(SPICSState::Set);

            return EinkStatus::OK;
        }
    } // namespace

    void setDisplayRefreshTimings()
    {
        std::uint8_t buf[4];

        buf[0] = EinkTCONSetting; // 0x60
        std::memcpy(&buf[1], tconSettings, sizeof(tconSettings));

        if (writeData(buf, sizeof(buf), SPICSConfig::Automatic) != kStatus_Success) {
            return;
        }
    }

    bool isPoweredOn()
    {
        return poweredOn;
    }

    EinkStatus powerOn()
    {
        if (poweredOn) {
            return EinkStatus::OK;
        }

        std::uint8_t cmd = EinkPowerON; // 0x04
        if (writeData(&cmd, sizeof(cmd), SPICSConfig::Automatic) != kStatus_Success) {
            powerDown();
            return EinkStatus::DMAErr;
        }

        poweredOn = true;

        if (!waitUntilDisplayBusy(pdMS_TO_TICKS(busyTimeoutMs))) {
            return EinkStatus::Timeout;
        }

        return EinkStatus::OK;
    }

    EinkStatus powerOff()
    {
        if (!poweredOn) {
            return EinkStatus::OK;
        }

        std::uint8_t cmd = EinkPowerOFF; // 0x02
        if (writeData(&cmd, sizeof(cmd), SPICSConfig::Automatic) != kStatus_Success) {
            return EinkStatus::DMAErr;
        }

        const auto ret = waitUntilDisplayBusy(pdMS_TO_TICKS(busyTimeoutMs)) ? EinkStatus::OK : EinkStatus::Timeout;

        /* Continue procedure regardless result */
        logicPowerOff();
        poweredOn = false;

        return ret;
    }

    EinkStatus powerDown()
    {
        const auto powerOffStatus = powerOff();
        deinit();
        return powerOffStatus;
    }

    std::int16_t getTemperatureInternal()
    {
        std::uint8_t cmd    = EinkTemperatureSensorCalibration;
        std::int8_t temp[2] = {0, 0};

        writeCS(SPICSState::Clear);

        if (writeData(&cmd, sizeof(cmd), SPICSConfig::Manual) != kStatus_Success) {
            writeCS(SPICSState::Set);
            powerDown();
            return -1;
        }

        if (!waitUntilDisplayBusy(pdMS_TO_TICKS(busyTimeoutMs))) {
            return -1;
        }

        if (readData(temp, sizeof(temp), SPICSConfig::Manual) != kStatus_Success) {
            writeCS(SPICSState::Set);
            powerDown();
            return -1;
        }

        writeCS(SPICSState::Set);

        /* First byte of the temp describes the integer part of the temperature in degrees Celsius */
        const std::int8_t temperatureInteger = temp[0];

        /* The MSB bit of the second byte describes the fraction of the temperature. Bit value of 1 means .5 degree
         * Celsius, bit value of 0 means .0 degree Celsius */
        // const std::int8_t temperatureFraction = ((temp[1] & 0x80) >> 7);

        return temperatureInteger;
    }

    EinkStatus resetAndInitialize()
    {
        logicPowerOn();

        /* Initialize the synchronization resources, SPI and GPIOs for the Eink BSP */
        if (init() != 0) {
            powerDown();
            return EinkStatus::InitErr;
        }

        /* Reset the display */
        resetDisplayController();

        /* Set the initial configuration of the eink registers after reset */
        if (setInitialConfig() != EinkStatus::OK) {
            powerDown();
            return EinkStatus::InitErr;
        }

        /* After the reset the display is powered off */
        poweredOn = false;
        return EinkStatus::OK;
    }

    EinkStatus updateWaveform(const EinkWaveformSettings *settings)
    {
        /* LUTD */
        if (writeData(settings->LUTDData, settings->LUTDSize, SPICSConfig::Automatic) != kStatus_Success) {
            powerDown();
            return EinkStatus::DMAErr;
        }

        /* LUTC */
        if (writeData(settings->LUTCData, settings->LUTCSize + 1, SPICSConfig::Automatic) != kStatus_Success) {
            powerDown();
            return EinkStatus::DMAErr;
        }

        currentlyConfiguredWaveform = settings->mode;

        return EinkStatus::OK;
    }

    EinkStatus waitTillPipelineBusy()
    {
        std::uint16_t flags = 0;

        readFlagsRegister(&flags);
        while ((flags & EINK_FLAG_PIPELINE_BUSY)) {
            vTaskDelay(pdMS_TO_TICKS(1));
            readFlagsRegister(&flags);
        }

        return EinkStatus::OK;
    }

    EinkStatus ditherDisplay()
    {
        std::uint8_t cmdWithArgs[2] = {EinkDPC, EINK_DITHER_4BPP_MODE | EINK_DITHER_START};

        if (writeData(cmdWithArgs, sizeof(cmdWithArgs), SPICSConfig::Automatic) != kStatus_Success) {
            powerDown();
            return EinkStatus::DMAErr;
        }

        std::uint16_t flags = 0;
        readFlagsRegister(&flags);
        if ((flags & EINK_FLAG_DITHER_IN_PROGRESS)) {
            return EinkStatus::SPIErr;
        }

        return EinkStatus::OK;
    }

    EinkStatus updateFrame(EinkFrame frame, const std::uint8_t *buffer, EinkBpp bpp, EinkDisplayColorMode invertColors)
    {
        const std::uint8_t pixelsInByte = 8 / static_cast<unsigned>(bpp);
        std::uint8_t buf[10];

        einkRotatedBuf[0] = EinkDataStartTransmission1;
        einkRotatedBuf[1] = static_cast<unsigned>(bpp) - 1; //  0 - 1Bpp, 1 - 2Bpp, 2 - 3Bpp, 3 - 4Bpp

        if ((currentlyConfiguredWaveform == EinkWaveform::A2) || (currentlyConfiguredWaveform == EinkWaveform::DU2)) {
            switch (bpp) {
            case EinkBpp::Eink1Bpp:
                transformAnimationFrameCoordinateSystem1Bpp(
                    buffer, frame.width, frame.height, &einkRotatedBuf[EINK_IMAGE_CONFIG_SIZE], invertColors);
                break;

            case EinkBpp::Eink2Bpp:
                transformAnimationFrameCoordinateSystem2Bpp(
                    buffer, frame.width, frame.height, &einkRotatedBuf[EINK_IMAGE_CONFIG_SIZE], invertColors);
                break;

            case EinkBpp::Eink3Bpp:
                transformAnimationFrameCoordinateSystem3Bpp(
                    buffer, frame.width, frame.height, &einkRotatedBuf[EINK_IMAGE_CONFIG_SIZE], invertColors);
                break;

            case EinkBpp::Eink4Bpp:
#if defined(EINK_ROTATE_90_CLOCKWISE)
                transformFrameCoordinateSystem4Bpp(
                    buffer, frame.width, frame.height, &einkRotatedBuf[EINK_IMAGE_CONFIG_SIZE], invertColors);
#else
                transformFrameCoordinateSystemNoRotation4Bpp(
                    buffer, frame.width, frame.height, &einkRotatedBuf[EINK_IMAGE_CONFIG_SIZE], invertColors);
#endif
                break;
            }
        }
        else {
            switch (bpp) {
            case EinkBpp::Eink1Bpp:
                transformFrameCoordinateSystem1Bpp(
                    buffer, frame.width, frame.height, &einkRotatedBuf[EINK_IMAGE_CONFIG_SIZE], invertColors);
                break;

            case EinkBpp::Eink2Bpp:
                transformFrameCoordinateSystem2Bpp(
                    buffer, frame.width, frame.height, &einkRotatedBuf[EINK_IMAGE_CONFIG_SIZE], invertColors);
                break;

            case EinkBpp::Eink3Bpp:
                transformFrameCoordinateSystem3Bpp(
                    buffer, frame.width, frame.height, &einkRotatedBuf[EINK_IMAGE_CONFIG_SIZE], invertColors);
                break;

            case EinkBpp::Eink4Bpp:
#if defined(EINK_ROTATE_90_CLOCKWISE)
                transformFrameCoordinateSystem4Bpp(
                    buffer, frame.width, frame.height, &einkRotatedBuf[EINK_IMAGE_CONFIG_SIZE], invertColors);
#else
                transformFrameCoordinateSystemNoRotation4Bpp(
                    buffer, frame.width, frame.height, &einkRotatedBuf[EINK_IMAGE_CONFIG_SIZE], invertColors);
#endif
                break;
            }
        }

        /* Set display window */
        buf[0] = EinkDataStartTransmissionWindow;
        /* MSB of X axis of EPD display. Value converted from the standard GUI coords system to ED028TC1 one */
        buf[1] = static_cast<std::uint8_t>(getDisplayXAxis(frame) >> 8);
        /* LSB of X axis of EPD display. Value converted from the standard GUI coords system to ED028TC1 one */
        buf[2] = static_cast<std::uint8_t>(getDisplayXAxis(frame));
        /* MSB of Y axis of EPD display. Value converted from the standard GUI coords system to ED028TC1 one */
        buf[3] = static_cast<std::uint8_t>(getDisplayYAxis(frame) >> 8);
        /* LSB of Y axis of EPD display. Value converted from the standard GUI coords system to ED028TC1 one */
        buf[4] = static_cast<std::uint8_t>(getDisplayYAxis(frame));
        /* MSB of window height of EPD display. Value converted from the standard GUI coords system to ED028TC1 one */
        buf[5] = static_cast<std::uint8_t>(getDisplayWindowWidth(frame) >> 8);
        /* LSB of window height of EPD display. Value converted from the standard GUI coords system to ED028TC1 one */
        buf[6] = static_cast<std::uint8_t>(getDisplayWindowWidth(frame));
        /* MSB of window width of EPD display. Value converted from the standard GUI coords system to ED028TC1 one */
        buf[7] = static_cast<std::uint8_t>(getDisplayWindowHeight(frame) >> 8);
        /* LSB of window width of EPD display. Value converted from the standard GUI coords system to ED028TC1 one */
        buf[8] = static_cast<std::uint8_t>(getDisplayWindowHeight(frame));

        if (writeData(buf, 9, SPICSConfig::Automatic) != kStatus_Success) {
            powerDown();
            return EinkStatus::DMAErr;
        }

        /* Config (2 bytes) + dataSize (W * H / pixelsInByte bytes) */
        const auto msgSize = EINK_IMAGE_CONFIG_SIZE + (static_cast<std::uint32_t>(frame.width) *
                                                       static_cast<std::uint32_t>(frame.height) / pixelsInByte);

        /* Send the part of the image to the display memory */
        if (writeData(einkRotatedBuf, msgSize, SPICSConfig::Automatic) != kStatus_Success) {
            powerDown();
            return EinkStatus::DMAErr;
        }

        return EinkStatus::OK;
    }

    EinkStatus fillScreenWithColor(EinkDisplayColorFilling colorFill)
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

        if (writeData(buf, 9, SPICSConfig::Automatic) != kStatus_Success) {
            powerDown();
            return EinkStatus::DMAErr;
        }

        writeCS(SPICSState::Clear);

        buf[0] = EinkDataStartTransmission1; // 0x10
        buf[1] = static_cast<unsigned>(EinkBpp::Eink1Bpp) - 1;
        if (writeData(buf, 2, SPICSConfig::Manual) != kStatus_Success) {
            writeCS(SPICSState::Set);
            powerDown();
            return EinkStatus::DMAErr;
        }

        constexpr auto pixelsPerByte1Bpp    = 8;
        constexpr auto backgroundBufferSize = BOARD_EINK_DISPLAY_RES_Y * BOARD_EINK_DISPLAY_RES_X / pixelsPerByte1Bpp;
        const auto backgroundColor          = static_cast<std::uint8_t>(colorFill);

        std::unique_ptr<std::uint8_t[]> backgroundBuffer;
        try {
            backgroundBuffer = std::make_unique<std::uint8_t[]>(backgroundBufferSize);
        }
        catch (const std::bad_alloc &exception) {
            LOG_ERROR("Could not create the buffer for the background plane");
            return EinkStatus::NoMem;
        }

        std::memset(backgroundBuffer.get(), backgroundColor, backgroundBufferSize);

        if (writeData(backgroundBuffer.get(), backgroundBufferSize, SPICSConfig::Manual) != kStatus_Success) {
            writeCS(SPICSState::Set);
            powerDown();
            return EinkStatus::DMAErr;
        }

        writeCS(SPICSState::Set);

        return refreshImage(EinkFrame{0, 0, BOARD_EINK_DISPLAY_RES_X, BOARD_EINK_DISPLAY_RES_Y});
    }

    EinkStatus refreshImage(EinkFrame frame)
    {
        setDisplayRefreshTimings();
        setGateOrder();

        std::uint8_t buf[10];

        buf[0] = EinkDisplayRefresh;
        buf[1] = UPD_CPY_TO_PRE;

        /* MSB of X axis of EPD display. Value converted from the standard GUI coords system to ED028TC1 one */
        buf[2] = static_cast<std::uint8_t>(getDisplayXAxis(frame) >> 8);
        /* LSB of X axis of EPD display. Value converted from the standard GUI coords system to ED028TC1 one */
        buf[3] = static_cast<std::uint8_t>(getDisplayXAxis(frame));
        /* MSB of Y axis of EPD display. Value converted from the standard GUI coords system to ED028TC1 one */
        buf[4] = static_cast<std::uint8_t>(getDisplayYAxis(frame) >> 8);
        /* LSB of Y axis of EPD display. Value converted from the standard GUI coords system to ED028TC1 one */
        buf[5] = static_cast<std::uint8_t>(getDisplayYAxis(frame));
        // MSB of window height of EPD display. Value converted from the standard GUI coords system to ED028TC1 one */
        buf[6] = static_cast<std::uint8_t>(getDisplayWindowWidth(frame) >> 8);
        /* LSB of window height of EPD display. Value converted from the standard GUI coords system to ED028TC1 one */
        buf[7] = static_cast<std::uint8_t>(getDisplayWindowWidth(frame));
        /* MSB of window width of EPD display. Value converted from the standard GUI coords system to ED028TC1 one */
        buf[8] = static_cast<std::uint8_t>(getDisplayWindowHeight(frame) >> 8);
        /* LSB of window width of EPD display. Value converted from the standard GUI coords system to ED028TC1 one */
        buf[9] = static_cast<std::uint8_t>(getDisplayWindowHeight(frame));

        if (writeData(buf, sizeof(buf), SPICSConfig::Automatic) != kStatus_Success) {
            powerDown();
            return EinkStatus::DMAErr;
        }

        if (!waitUntilDisplayBusy(pdMS_TO_TICKS(busyTimeoutMs))) {
            return EinkStatus::Timeout;
        }

        return EinkStatus::OK;
    }
}
