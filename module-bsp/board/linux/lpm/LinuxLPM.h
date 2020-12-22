//
// Created by mati on 09.09.2019.
//

#ifndef PUREPHONE_LINUXLPM_H
#define PUREPHONE_LINUXLPM_H

#include "bsp/lpm/bsp_lpm.hpp"

namespace bsp
{

    class LinuxLPM : public LowPowerMode
    {
      public:
        int32_t PowerOff() override final;
        int32_t Reboot() override final;
        void SetCpuFrequency(CpuFrequency freq) final;
    };

} // namespace bsp

#endif // PUREPHONE_LINUXLPM_H
