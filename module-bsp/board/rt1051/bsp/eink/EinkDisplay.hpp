// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint>
#include <memory>

#include <hal/eink/AbstractEinkDisplay.hpp>

#include "drivers/lpspi/DriverLPSPI.hpp"
#include "ED028TC1.h"

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
        EinkDisplay(FrameSize size);

        ~EinkDisplay() noexcept;

        void setMode(EinkDisplayColorMode mode) noexcept override;
        EinkStatus showImage(std::uint8_t *frameBuffer, const EinkRefreshMode refreshMode) override;
        void prepareEarlyRequest(EinkRefreshMode refreshMode, const WaveformTemperature behaviour) override;

        EinkStatus resetAndInit() override;
        void dither() override;
        void powerOn() override;
        void powerOff() override;
        void shutdown() override;
        void wipeOut() override;

        [[nodiscard]] auto getDevice() const noexcept -> std::shared_ptr<devices::Device> override;

      private:
        bool isNewWaveformNeeded(const EinkWaveforms_e newMode, const int32_t newTemperature) const;
        void resetWaveformSettings();

        EinkBpp_e getCurrentBitsPerPixelFormat() const noexcept;

        EinkStatus setWaveform(const EinkWaveforms_e mode, const std::int32_t temperature);

        std::int32_t getLastTemperature() const noexcept;

        EinkStatus update(std::uint8_t *displayBuffer);
        EinkStatus refresh(const EinkDisplayTimingsMode_e refreshMode);

        EinkStatus updateDisplay(std::uint8_t *frameBuffer, const EinkRefreshMode refreshMode);
        EinkStatus refreshDisplay(const EinkRefreshMode refreshMode);
        EinkStatus prepareDisplay(const EinkRefreshMode refreshMode, const WaveformTemperature behaviour);

        FrameSize size;

        EinkWaveformSettings_t currentWaveform;
        EinkDisplayColorMode displayMode;

        std::shared_ptr<drivers::DriverLPSPI> driverLPSPI;
    };
} // namespace hal::eink
