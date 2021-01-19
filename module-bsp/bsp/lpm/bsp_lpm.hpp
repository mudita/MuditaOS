#ifndef PUREPHONE_BSP_LPM_HPP
#define PUREPHONE_BSP_LPM_HPP

#include <optional>
#include <memory>

namespace bsp {

    class LowPowerMode
    {
      public:
        enum class CpuFrequency
        {
            Level_1, // 12 MHz
            Level_2, // 24 MHz
            Level_3, // 66 MHz
            Level_4, // 132 MHz
            Level_5, // 264 MHz
            Level_6  // 528 MHz
        };
        enum class OscillatorSource
        {
            External,
            Internal
        };
        enum class Pll2State
        {
            Enable,
            Disable
        };

        LowPowerMode()          = default;
        virtual ~LowPowerMode() = default;

        static std::optional<std::unique_ptr<LowPowerMode>> Create();

        virtual int32_t PowerOff() = 0;
        virtual int32_t Reboot() = 0;

        virtual void SetCpuFrequency(CpuFrequency freq) = 0;
        [[nodiscard]] CpuFrequency GetCurrentFrequency() const noexcept;

        virtual void SwitchOscillatorSource(OscillatorSource source) = 0;
        [[nodiscard]] OscillatorSource GetCurrentOscillatorSource() const noexcept;

        virtual void SwitchPll2State(Pll2State state) = 0;
        [[nodiscard]] Pll2State GetCurrentPll2State() const noexcept;

    protected:
        CpuFrequency currentFrequency = CpuFrequency::Level_6;
        OscillatorSource currentOscSource = OscillatorSource::External;
        Pll2State currentPll2State = Pll2State::Enable;
    };
} // namespace bsp

#endif //PUREPHONE_BSP_LPM_HPP
