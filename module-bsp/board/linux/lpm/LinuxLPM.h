// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "bsp/lpm/bsp_lpm.hpp"

namespace bsp
{
    class LinuxLPM : public LowPowerMode
    {
      public:
        std::int32_t PowerOff() override final;
        std::int32_t Reboot(RebootType reason) override final;
        void SetCpuFrequency(CpuFrequencyMHz freq) final;
        [[nodiscard]] std::uint32_t GetCpuFrequency() const noexcept final;
        void SwitchOscillatorSource(OscillatorSource source) final;

        void EnableDcdcPowerSaveMode() final;
        void DisableDcdcPowerSaveMode() final;

        void AllowEnteringWfiMode() final;
        void BlockEnteringWfiMode() final;
        std::uint32_t EnterWfiModeIfAllowed() final;
        std::uint32_t GetLastTimeSpentInWfi() final;

        void DisableSysTick() final;
        void EnableSysTick() final;

        std::uint32_t DisableInterrupts() final;
        void EnableInterrupts(std::uint32_t primask) final;
    };
} // namespace bsp
