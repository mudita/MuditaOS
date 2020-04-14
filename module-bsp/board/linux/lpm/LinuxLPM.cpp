//
// Created by mati on 09.09.2019.
//

#include "LinuxLPM.h"

namespace bsp
{

    int32_t LinuxLPM::Switch(const bsp::LowPowerMode::Mode mode)
    {
        currentMode = mode;
        return 0;
    }

    int32_t LinuxLPM::PowerOff()
    {
        return 0;
    }

    int32_t LinuxLPM::Reboot()
    {
        return 0;
    }
} // namespace bsp
