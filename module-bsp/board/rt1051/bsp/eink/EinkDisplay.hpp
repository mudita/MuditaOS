// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "drivers/lpspi/DriverLPSPI.hpp"
#include "ED028TC1.hpp"

#include <hal/eink/AbstractEinkDisplay.hpp>

#include <cstdint>
#include <memory>

namespace hal::eink
{
    /**
     * Specifies the Eink display.
     * Responsible for handling low-level Eink display operations, e.g. switching power modes, updating, refreshing,
     * etc.
     */
    class EinkDisplay : public AbstractEinkDisplay
    {
      public:
        explicit EinkDisplay(FrameSize size);

        ~EinkDisplay() noexcept override;

        auto setMode(EinkDisplayColorMode mode) noexcept -> void override;
        [[nodiscard]] auto getMode() const noexcept -> EinkDisplayColorMode override;

        auto showImageUpdate(const std::vector<EinkFrame> &updateFrames, const std::uint8_t *frameBuffer)
            -> EinkStatus override;
        auto showImageRefresh(const EinkFrame &refreshFrame, EinkRefreshMode refreshMode) -> EinkStatus override;
        auto showImage(const std::vector<EinkFrame> &updateFrames,
                       const EinkFrame &refreshFrame,
                       const std::uint8_t *frameBuffer,
                       EinkRefreshMode refreshMode) -> EinkStatus override;
        auto prepareEarlyRequest(EinkRefreshMode refreshMode, WaveformTemperature behaviour) -> void override;

        [[nodiscard]] auto resetAndInit() -> EinkStatus override;
        [[nodiscard]] auto dither() -> EinkStatus override;
        [[nodiscard]] auto powerOn() -> EinkStatus override;
        [[nodiscard]] auto powerOff() -> EinkStatus override;
        [[nodiscard]] auto shutdown() -> EinkStatus override;
        [[nodiscard]] auto wipeOut() -> EinkStatus override;
        [[nodiscard]] auto reinitAndPowerOn() -> EinkStatus override;

        [[nodiscard]] auto getDevice() const noexcept -> std::shared_ptr<devices::Device> override;

      private:
        [[nodiscard]] auto isNewWaveformNeeded(bsp::eink::EinkWaveform newMode, std::int32_t newTemperature) const
            -> bool;
        [[nodiscard]] auto setWaveform(bsp::eink::EinkWaveform mode, std::int32_t temperature) -> EinkStatus;
        auto resetWaveformSettings() -> void;

        [[nodiscard]] auto getCurrentBitsPerPixelFormat() const noexcept -> bsp::eink::EinkBpp;

        [[nodiscard]] auto getLastTemperature() const noexcept -> std::int32_t;

        auto updateDisplay(EinkFrame frame, const std::uint8_t *frameBuffer) -> EinkStatus;
        auto refreshDisplay(EinkFrame frame) -> EinkStatus;
        auto prepareDisplay(EinkRefreshMode refreshMode, WaveformTemperature behaviour) -> EinkStatus;
        auto tryReinitAndPowerOn() -> EinkStatus;

        FrameSize size;

        bsp::eink::EinkWaveformSettings currentWaveform;
        EinkDisplayColorMode displayMode;

        std::shared_ptr<drivers::DriverLPSPI> driverLPSPI;
    };
} // namespace hal::eink
