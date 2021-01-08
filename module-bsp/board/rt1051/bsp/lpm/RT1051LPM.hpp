// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#ifndef PUREPHONE_RT1051LPM_HPP
#define PUREPHONE_RT1051LPM_HPP

#include "bsp/lpm/bsp_lpm.hpp"
#include "drivers/gpio/DriverGPIO.hpp"
#include "CpuFreqLPM.hpp"
#include <fsl_clock.h>

namespace bsp
{
    inline constexpr uint8_t OscillatorReadyCounterValue{127};
    inline constexpr uint8_t CCM_TupleShift{8};
    inline constexpr uint8_t CCM_TupleMask{0x1F};
    inline constexpr uint32_t ClockNeededRunWaitMode{kCLOCK_ClockNeededRunWait};

    class RT1051LPM : public LowPowerMode
    {
      public:
        RT1051LPM();
        int32_t PowerOff() override final;
        int32_t Reboot() override final;
        void SetCpuFrequency(CpuFrequency freq) final;
        void SwitchOscillatorSource(OscillatorSource source) final;
        void SwitchPll2State(Pll2State state) final;

      private:
        [[nodiscard]] bool IsClockEnabled(clock_ip_name_t name) const noexcept;

        std::shared_ptr<drivers::DriverGPIO> gpio;
        std::unique_ptr<bsp::CpuFreqLPM> CpuFreq;
    };

} // namespace bsp

#endif // PUREPHONE_RT1051LPM_HPP
