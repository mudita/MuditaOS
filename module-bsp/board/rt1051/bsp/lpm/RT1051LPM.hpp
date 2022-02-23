// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
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
        int32_t Reboot(RebootType reason) override final;
        void SetCpuFrequency(CpuFrequencyMHz freq) final;
        void SetHighestCoreVoltage() final;
        [[nodiscard]] uint32_t GetCpuFrequency() const noexcept final;
        void SwitchOscillatorSource(OscillatorSource source) final;
        void SetBootSuccess() override;

        void EnableDcdcPowerSaveMode() final;
        void DisableDcdcPowerSaveMode() final;

        void DisconnectInternalLoadResistor() final;
        void ConnectInternalLoadResistor() final;

        void SwitchToRegularModeLDO() final;
        void SwitchToLowPowerModeLDO() final;

      private:
        CpuFrequencyMHz onChangeUp(CpuFrequencyMHz freq, CpuFrequencyMHz newFrequency);
        void onChangeDown(bsp::CpuFrequencyMHz freq);
        std::shared_ptr<drivers::DriverGPIO> gpio_1;
        std::shared_ptr<drivers::DriverGPIO> gpio_2;
        std::unique_ptr<bsp::CpuFreqLPM> CpuFreq;
        std::shared_ptr<drivers::DriverSEMC> driverSEMC;
    };

} // namespace bsp

#endif // PUREPHONE_RT1051LPM_HPP
