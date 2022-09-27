// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <board.h>

#include <log/log.hpp>

#include <cstring>
#include <memory>

#include "LinuxEinkDisplay.hpp"
#include "ED028TC1.h"

namespace hal::eink
{
    EinkStatus translateStatus(const EinkStatus_e status_e)
    {
        switch (status_e) {
        case EinkOK:
            return EinkStatus::EinkOK;
        case EinkError:
            return EinkStatus::EinkError;
        case EinkSPIErr:
            return EinkStatus::EinkSPIErr;
        case EinkSPINotInitializedErr:
            return EinkStatus::EinkSPINotInitializedErr;
        case EinkDMAErr:
            return EinkStatus::EinkDMAErr;
        case EinkInitErr:
            return EinkStatus::EinkInitErr;
        case EinkTimeout:
            return EinkStatus::EinkTimeout;
        case EinkNoMem:
            return EinkStatus::EinkNoMem;
        case EinkWaveformsFileOpenFail:
            return EinkStatus::EinkWaveformsFileOpenFail;
        default:
            return EinkStatus::EinkUnknown;
        }
    }

    LinuxEinkDisplay::LinuxEinkDisplay(FrameSize size) : size{size}
    {}

    void LinuxEinkDisplay::setMode(const EinkDisplayColorMode mode) noexcept
    {
        displayColorMode = mode;
    }

    EinkStatus LinuxEinkDisplay::showImage(std::uint8_t *frameBuffer, const EinkRefreshMode refreshMode)
    {
        return translateStatus(EinkUpdateFrame(EinkFrame_t{0, 0, size.width, size.height}, frameBuffer));
    }

    void LinuxEinkDisplay::prepareEarlyRequest([[maybe_unused]] const EinkRefreshMode refreshMode,
                                               [[maybe_unused]] const WaveformTemperature behaviour)
    {}

    void LinuxEinkDisplay::dither()
    {}

    void LinuxEinkDisplay::powerOn()
    {
        EinkPowerOn();
    }

    void LinuxEinkDisplay::powerOff()
    {
        EinkPowerOff();
    }

    void LinuxEinkDisplay::shutdown()
    {}

    void LinuxEinkDisplay::wipeOut()
    {
        EinkFillScreenWithColor(EinkDisplayColorFilling_e::EinkDisplayColorWhite);
    }

    EinkStatus LinuxEinkDisplay::resetAndInit()
    {
        EinkResetAndInitialize();
        return EinkStatus::EinkOK;
    }

    [[nodiscard]] auto LinuxEinkDisplay::getDevice() const noexcept -> std::shared_ptr<devices::Device>
    {
        return {};
    }

    std::unique_ptr<AbstractEinkDisplay> AbstractEinkDisplay::Factory::create(FrameSize size)
    {
        return std::make_unique<LinuxEinkDisplay>(size);
    }
} // namespace hal::eink
