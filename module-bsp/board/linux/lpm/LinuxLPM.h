﻿// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#ifndef PUREPHONE_LINUXLPM_H
#define PUREPHONE_LINUXLPM_H

#include "bsp/lpm/bsp_lpm.hpp"

namespace bsp
{

    class LinuxLPM : public LowPowerMode
    {
      public:
        int32_t PowerOff() override final;
        int32_t Reboot(RebootType reason) override final;
        void SetCpuFrequency(CpuFrequencyMHz freq) final;
        [[nodiscard]] uint32_t GetCpuFrequency() const noexcept final;
        void SwitchOscillatorSource(OscillatorSource source) final;

        void EnableDcdcPowerSaveMode() final;
        void DisableDcdcPowerSaveMode() final;

        void SwitchToRegularModeLDO() final;
        void SwitchToLowPowerModeLDO() final;
    };

} // namespace bsp

#endif // PUREPHONE_LINUXLPM_H
