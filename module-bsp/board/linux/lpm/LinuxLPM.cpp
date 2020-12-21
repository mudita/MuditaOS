// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
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

    int32_t LinuxLPM::Reboot()
    {
        return 0;
    }

    void LinuxLPM::SetCpuFrequency(bsp::LowPowerMode::CpuFrequency freq)
    {
        currentFrequency = freq;
    }

    void LinuxLPM::SwitchOscillatorSource(bsp::LowPowerMode::OscillatorSource source)
    {
        currentOscSource = source;
    }
} // namespace bsp
