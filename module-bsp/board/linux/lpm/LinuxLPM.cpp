// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

//
// Created by mati on 09.09.2019.
//

#include "LinuxLPM.h"

namespace bsp
{

    int32_t LinuxLPM::PowerOff()
    {
        return 0;
    }

    int32_t LinuxLPM::Reboot(RebootType)
    {
        return 0;
    }

    void LinuxLPM::SetCpuFrequency(bsp::CpuFrequencyMHz freq)
    {
        currentFrequency = freq;
    }

    void LinuxLPM::SetHighestCoreVoltage()
    {}

    uint32_t LinuxLPM::GetCpuFrequency() const noexcept
    {
        return 0;
    }

    void LinuxLPM::SwitchOscillatorSource(bsp::LowPowerMode::OscillatorSource source)
    {}

    void LinuxLPM::EnableDcdcPowerSaveMode()
    {}

    void LinuxLPM::DisableDcdcPowerSaveMode()
    {}

    void LinuxLPM::DisconnectInternalLoadResistor()
    {}

    void LinuxLPM::ConnectInternalLoadResistor()
    {}

    void LinuxLPM::SwitchToRegularModeLDO()
    {}

    void LinuxLPM::SwitchToLowPowerModeLDO()
    {}

} // namespace bsp
