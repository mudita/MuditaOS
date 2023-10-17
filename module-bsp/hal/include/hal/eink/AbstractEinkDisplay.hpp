// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory>
#include <vector>

#include <devices/Device.hpp>

namespace hal::eink
{

    enum class EinkDisplayColorMode
    {
        EinkDisplayColorModeStandard,
        EinkDisplayColorModeInverted
    };

    enum class WaveformTemperature
    {
        KEEP_CURRENT,
        MEASURE_NEW,
    };

    enum class EinkRefreshMode
    {
        REFRESH_NONE,
        REFRESH_FAST = 1,
        REFRESH_DEEP
    };

    enum class EinkStatus
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

        EinkUnknown,
    };

    struct FrameSize
    {
        uint16_t width;
        uint16_t height;
    };

    struct EinkFrame
    {
        uint16_t pos_x;
        uint16_t pos_y;
        FrameSize size;
    };

    class AbstractEinkDisplay
    {
      public:
        struct Factory
        {
            static std::unique_ptr<AbstractEinkDisplay> create(FrameSize size);
        };

        virtual ~AbstractEinkDisplay() = default;

        virtual void setMode(const EinkDisplayColorMode mode) noexcept                                        = 0;
        virtual EinkDisplayColorMode getMode() const noexcept                                                 = 0;
        virtual EinkStatus showImageUpdate(const std::vector<EinkFrame> &updateFrames,
                                           const std::uint8_t *frameBuffer)                                   = 0;
        virtual EinkStatus showImageRefresh(const EinkFrame &refreshFrame, const EinkRefreshMode refreshMode) = 0;
        virtual EinkStatus showImage(const std::vector<EinkFrame> &updateFrames,
                                     const EinkFrame &refreshFrame,
                                     const std::uint8_t *frameBuffer,
                                     const EinkRefreshMode refreshMode)                                       = 0;
        virtual void prepareEarlyRequest(EinkRefreshMode refreshMode, const WaveformTemperature behaviour)    = 0;

        virtual EinkStatus dither()                                         = 0;
        virtual EinkStatus powerOn()                                        = 0;
        virtual EinkStatus powerOff()                                       = 0;
        virtual EinkStatus shutdown()                                       = 0;
        virtual EinkStatus wipeOut()                                        = 0;
        virtual EinkStatus resetAndInit()                                   = 0;
        virtual std::shared_ptr<devices::Device> getDevice() const noexcept = 0;
        virtual EinkStatus reinitAndPowerOn()                               = 0;
    };
} // namespace hal::eink
