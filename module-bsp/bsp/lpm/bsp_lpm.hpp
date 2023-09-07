// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <optional>
#include <memory>
#include <bsp/common.hpp>

namespace drivers
{
    class DriverSEMC;
}

namespace bsp
{
    class LowPowerMode
    {
      public:
        enum class OscillatorSource
        {
            External,
            Internal
        };
        
        enum class RebootType
        {
            NormalRestart,
            GoToRecoveryUpdate,       //! Goto recovery into the update mode
            GoToRecoveryFactoryReset, //! GOto recovery into the factory reset mode
            GoToRecoveryRecovery,     //! Goto to recovery into recovery mode
            GoToRecoveryBackup,       //! Goto to recovery into backup mode
            GoToRecoveryRestore       //! Goto to recovery into restore mode
        };

        LowPowerMode()          = default;
        virtual ~LowPowerMode() = default;

        static std::optional<std::unique_ptr<LowPowerMode>> Create();

        virtual std::int32_t PowerOff()                = 0;
        virtual std::int32_t Reboot(RebootType reason) = 0;

        virtual void SetCpuFrequency(CpuFrequencyMHz freq) = 0;
        [[nodiscard]] CpuFrequencyMHz GetCurrentFrequencyLevel() const noexcept;
        [[nodiscard]] virtual std::uint32_t GetCpuFrequency() const noexcept = 0;

        virtual void SwitchOscillatorSource(OscillatorSource source) = 0;

        virtual void EnableDcdcPowerSaveMode()  = 0;
        virtual void DisableDcdcPowerSaveMode() = 0;

      protected:
        CpuFrequencyMHz currentFrequency = CpuFrequencyMHz::Level_6;
    };
} // namespace bsp
