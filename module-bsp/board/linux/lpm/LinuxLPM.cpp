// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "LinuxLPM.h"

namespace bsp
{
    std::int32_t LinuxLPM::PowerOff()
    {
        return 0;
    }

    std::int32_t LinuxLPM::Reboot([[maybe_unused]] RebootType reason)
    {
        return 0;
    }

    void LinuxLPM::SetCpuFrequency(CpuFrequencyMHz freq)
    {
        currentFrequency = freq;
    }

    std::uint32_t LinuxLPM::GetCpuFrequency() const noexcept
    {
        return 0;
    }

    void LinuxLPM::SwitchOscillatorSource(LowPowerMode::OscillatorSource source)
    {}

    void LinuxLPM::EnableDcdcPowerSaveMode()
    {}

    void LinuxLPM::DisableDcdcPowerSaveMode()
    {}

    void LinuxLPM::AllowEnteringWfiMode()
    {}

    void LinuxLPM::BlockEnteringWfiMode()
    {}

    std::uint32_t LinuxLPM::EnterWfiModeIfAllowed()
    {
        return 0;
    }

    std::uint32_t LinuxLPM::GetLastTimeSpentInWfi()
    {
        return 0;
    }

    void LinuxLPM::DisableSysTick()
    {}

    void LinuxLPM::EnableSysTick()
    {}

    std::uint32_t LinuxLPM::DisableInterrupts()
    {
        return 0;
    }

    void LinuxLPM::EnableInterrupts([[maybe_unused]] std::uint32_t primask)
    {}
} // namespace bsp
