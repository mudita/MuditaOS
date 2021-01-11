// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <gui/Common.hpp>

#include <EinkIncludes.hpp>
#include "Common.hpp"

#include <cstdint>
#include <memory>

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
        explicit EinkDisplay(::gui::Size screenSize = {DefaultScreenWidth, DefaultScreenHeight});
        ~EinkDisplay() noexcept;

        EinkStatus_e resetAndInit();
        EinkStatus_e update(std::uint8_t *displayBuffer);
        EinkStatus_e refresh(EinkDisplayTimingsMode_e refreshMode);
        void dither();
        void powerOn();
        void powerOff();
        void shutdown();

        bool changeWaveform(EinkWaveforms_e mode, std::int32_t temperature);
        void setMode(EinkDisplayColorMode_e mode) noexcept;

        ::gui::Size getSize() const noexcept;

      private:
        unsigned int calculateWaveFormSegment(std::int32_t temperature) const;
        unsigned int calculateWaveFormOffset(EinkWaveforms_e mode, unsigned int segment) const;
        void resetWaveFormSettings();
        EinkBpp_e getCurrentBitsPerPixelFormat() const noexcept;

        static constexpr ::gui::Point pointTopLeft{0, 0};
        const ::gui::Size size;
        EinkWaveFormSettings_t waveformSettings;
        EinkDisplayColorMode_e displayMode;
    };
} // namespace service::eink
