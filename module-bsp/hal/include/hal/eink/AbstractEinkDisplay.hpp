// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

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
        KeepCurrent,
        MeasureNew
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
        EinkUnknown
    };

    struct FrameSize
    {
        std::uint16_t width;
        std::uint16_t height;
    };

    struct EinkFrame
    {
        std::uint16_t pos_x;
        std::uint16_t pos_y;
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

        virtual auto setMode(EinkDisplayColorMode mode) noexcept -> void            = 0;
        [[nodiscard]] virtual auto getMode() const noexcept -> EinkDisplayColorMode = 0;
        virtual auto showImageUpdate(const std::vector<EinkFrame> &updateFrames, const std::uint8_t *frameBuffer)
            -> EinkStatus                                                                                       = 0;
        virtual auto showImageRefresh(const EinkFrame &refreshFrame, EinkRefreshMode refreshMode) -> EinkStatus = 0;
        virtual auto showImage(const std::vector<EinkFrame> &updateFrames,
                               const EinkFrame &refreshFrame,
                               const std::uint8_t *frameBuffer,
                               EinkRefreshMode refreshMode) -> EinkStatus                                       = 0;
        virtual auto prepareEarlyRequest(EinkRefreshMode refreshMode, WaveformTemperature behaviour) -> void    = 0;

        virtual auto dither() -> EinkStatus                                                       = 0;
        virtual auto powerOn() -> EinkStatus                                                      = 0;
        virtual auto powerOff() -> EinkStatus                                                     = 0;
        virtual auto shutdown() -> EinkStatus                                                     = 0;
        virtual auto wipeOut() -> EinkStatus                                                      = 0;
        virtual auto resetAndInit() -> EinkStatus                                                 = 0;
        [[nodiscard]] virtual auto getDevice() const noexcept -> std::shared_ptr<devices::Device> = 0;
        virtual auto reinitAndPowerOn() -> EinkStatus                                             = 0;
    };
} // namespace hal::eink
