// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <hal/eink/AbstractEinkDisplay.hpp>

namespace hal::eink
{

    class LinuxEinkDisplay : public AbstractEinkDisplay
    {
      public:
        explicit LinuxEinkDisplay(FrameSize size);

      private:
        void setMode(const EinkDisplayColorMode mode) noexcept override;
        EinkStatus showImage(std::uint8_t *frameBuffer, const EinkRefreshMode refreshMode) override;
        void prepareEarlyRequest(const EinkRefreshMode refreshMode, const WaveformTemperature behaviour) override;

        void dither() override;
        void powerOn() override;
        void powerOff() override;
        void shutdown() override;
        void wipeOut() override;
        EinkStatus resetAndInit() override;
        [[nodiscard]] std::shared_ptr<devices::Device> getDevice() const noexcept override;

        FrameSize size;
        EinkDisplayColorMode displayColorMode{EinkDisplayColorMode::EinkDisplayColorModeStandard};
    };
} // namespace hal::eink
