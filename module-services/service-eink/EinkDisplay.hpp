// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <EinkIncludes.hpp>
#include <gui/Common.hpp>
#include "Common.hpp"

#include <cstdint>
#include <memory>
#include "drivers/lpspi/DriverLPSPI.hpp"
#include "EinkSentinel.hpp"

namespace service::eink
{
    /**
     * Specifies the Eink display.
     * Responsible for handling low-level Eink display operations, e.g. switching power modes, updating, refreshing,
     * etc.
     */
    class EinkDisplay
    {
      public:
        explicit EinkDisplay(::gui::Size screenSize);
        ~EinkDisplay() noexcept;

        EinkStatus_e resetAndInit();
        EinkStatus_e update(std::uint8_t *displayBuffer);
        EinkStatus_e refresh(EinkDisplayTimingsMode_e refreshMode);
        void dither();
        void powerOn();
        void powerOff();
        void shutdown();
        void wipeOut();

        EinkStatus_e setWaveform(EinkWaveforms_e mode, std::int32_t temperature);
        void setMode(EinkDisplayColorMode_e mode) noexcept;

        std::int32_t getLastTemperature() const noexcept;
        ::gui::Size getSize() const noexcept;

        [[nodiscard]] auto getDevice() const noexcept -> std::shared_ptr<devices::Device>;
        void setEinkSentinel(std::shared_ptr<EinkSentinel> sentinel);

      private:
        static unsigned int toWaveformTemperatureOffset(std::int32_t temperature) noexcept;
        static unsigned int toWaveformOffset(unsigned short LUTbank, unsigned int temperatureOffset) noexcept;
        static unsigned int toWaveformOffset(EinkWaveforms_e mode, unsigned int temperatureOffset);
        bool isNewWaveformNeeded(EinkWaveforms_e newMode, int32_t newTemperature) const;
        void resetWaveformSettings();

        EinkBpp_e getCurrentBitsPerPixelFormat() const noexcept;
        static constexpr ::gui::Point pointTopLeft{0, 0};
        const ::gui::Size size;
        EinkWaveformSettings_t currentWaveform;
        EinkDisplayColorMode_e displayMode;

        std::shared_ptr<drivers::DriverLPSPI> driverLPSPI;
        std::shared_ptr<EinkSentinel> eInkSentinel;
    };
} // namespace service::eink
