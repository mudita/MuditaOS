// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#ifndef PUREPHONE_RT1051LPM_HPP
#define PUREPHONE_RT1051LPM_HPP

#include "bsp/lpm/bsp_lpm.hpp"
#include "drivers/gpio/DriverGPIO.hpp"
#include "CpuFreqLPM.hpp"

namespace bsp
{

    class RT1051LPM : public LowPowerMode
    {
      public:
        RT1051LPM();
        int32_t PowerOff() override final;
        int32_t Reboot() override final;
        void SetCpuFrequency(CpuFrequency freq) final;

      private:
        std::shared_ptr<drivers::DriverGPIO> gpio;
        std::unique_ptr<bsp::CpuFreqLPM> CpuFreq;
    };

} // namespace bsp

#endif // PUREPHONE_RT1051LPM_HPP
