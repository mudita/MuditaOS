// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <gui/Common.hpp>

#include <EinkIncludes.hpp>

#include <cstdint>
#include <memory>

namespace eink
{
    class EinkScreen
    {
      public:
        explicit EinkScreen(gui::Size screenSize = {480, 600});
        ~EinkScreen() noexcept;

        EinkStatus_e resetAndInit();
        EinkStatus_e update();
        EinkStatus_e refresh(EinkDisplayTimingsMode_e refreshMode);
        void dither();
        void powerOn();
        void powerOff();
        void shutdown();

        bool deepClear(std::int32_t temperature);
        bool changeWaveform(EinkWaveforms_e mode, std::int32_t temperature);

        void setScreenBuffer(const std::uint8_t *buffer, std::uint32_t bufferSize);
        void setDisplayMode(EinkDisplayColorMode_e mode) noexcept;

      private:
        void fillScreen(std::uint8_t colorIntensity);
        void setScreenBuffer(std::uint8_t value, std::uint32_t bufferSize);
        unsigned int calculateWaveFormSegment(std::int32_t temperature) const;
        unsigned int calculateWaveFormOffset(EinkWaveforms_e mode, unsigned int segment) const;
        void resetWaveFormSettings();

        static constexpr gui::Point pointTopLeft{0, 0};

        const gui::Size size;
        std::unique_ptr<std::uint8_t[]> screenBuffer;
        EinkWaveFormSettings_t waveformSettings;
        EinkDisplayColorMode_e displayMode;
    };
} // namespace eink
