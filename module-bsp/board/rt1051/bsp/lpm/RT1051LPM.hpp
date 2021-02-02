// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#ifndef PUREPHONE_RT1051LPM_HPP
#define PUREPHONE_RT1051LPM_HPP

#include "bsp/lpm/bsp_lpm.hpp"
#include "drivers/gpio/DriverGPIO.hpp"
#include "CpuFreqLPM.hpp"

namespace bsp
{
    inline constexpr uint8_t OscillatorReadyCounterValue{127};

    class RT1051LPM : public LowPowerMode
    {
      public:
        RT1051LPM();
        int32_t PowerOff() override final;
        int32_t Reboot() override final;
        void SetCpuFrequency(CpuFrequencyHz freq) final;
        [[nodiscard]] uint32_t GetCpuFrequency() const noexcept final;
        void SwitchOscillatorSource(OscillatorSource source) final;

      private:
        std::shared_ptr<drivers::DriverGPIO> gpio;
        std::unique_ptr<bsp::CpuFreqLPM> CpuFreq;
    };

} // namespace bsp

#endif // PUREPHONE_RT1051LPM_HPP
