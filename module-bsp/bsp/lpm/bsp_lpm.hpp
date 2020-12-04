#ifndef PUREPHONE_BSP_LPM_HPP
#define PUREPHONE_BSP_LPM_HPP

#include <optional>
#include <memory>

namespace bsp {

    class LowPowerMode
    {
      private:
      public:
        enum class Mode
        {
            FullSpeed,
            LowPowerRun,
            LowPowerIdle,
            Suspend

        };
        enum class CpuFrequency
        {
            Level_1, // 12 MHz
            Level_2, // 24 MHz
            Level_3, // 66 MHz
            Level_4, // 132 MHz
            Level_5, // 264 MHz
            Level_6  // 528 MHz
        };

        LowPowerMode()          = default;
        virtual ~LowPowerMode() = default;

        static std::optional<std::unique_ptr<LowPowerMode>> Create();

        virtual int32_t Switch(const Mode mode) = 0;
        Mode GetCurrentMode(){return currentMode;}

        virtual int32_t PowerOff() = 0;
        virtual int32_t Reboot() = 0;
        virtual void SetCpuFrequency(CpuFrequency freq) = 0;

    protected:
      Mode currentMode = Mode::FullSpeed;
    };
} // namespace bsp

#endif //PUREPHONE_BSP_LPM_HPP
