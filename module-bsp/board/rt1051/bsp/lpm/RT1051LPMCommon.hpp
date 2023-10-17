// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "bsp/lpm/bsp_lpm.hpp"
#include "drivers/gpio/DriverGPIO.hpp"
#include "CpuFreqLPM.hpp"

namespace bsp
{
    class RT1051LPMCommon : public LowPowerMode
    {
      public:
        RT1051LPMCommon();
        std::int32_t PowerOff() final;
        std::int32_t Reboot(RebootType reason) final;
        void SetCpuFrequency(CpuFrequencyMHz freq) final;
        [[nodiscard]] std::uint32_t GetCpuFrequency() const noexcept final;
        void SwitchOscillatorSource(OscillatorSource source) final;

      private:
        void onChangeUp(CpuFrequencyMHz freq, CpuFrequencyMHz newFrequency);
        void onChangeDown(CpuFrequencyMHz freq);

        std::unique_ptr<bsp::CpuFreqLPM> CpuFreq;
        std::shared_ptr<drivers::DriverSEMC> driverSEMC;
    };
} // namespace bsp
