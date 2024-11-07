// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md
#pragma once

#include <array>
#include <Units.hpp>

namespace bsp::devices::power::CW2015Regs
{
    inline constexpr auto ADDRESS = 0x62;

    struct Config
    {
        static constexpr auto ADDRESS = 0x08;

        explicit Config(const std::uint8_t raw) : reg{raw}
        {}

        units::SOC get_alert_threshold() const
        {
            return (reg & alert_mask) >> alert_shift;
        }

        Config &set_threshold(const units::SOC soc)
        {
            reg &= ~alert_mask;
            reg |= soc << alert_shift;
            return *this;
        }

        bool is_ufg_set() const
        {
            return (reg & ufg_mask) != 0;
        }

        Config &set_ufg(bool val)
        {
            reg &= ~ufg_mask;
            reg |= val << ufg_shift;
            return *this;
        }

        std::uint8_t get_raw() const
        {
            return reg;
        }

      private:
        std::uint8_t reg{};

        static constexpr std::uint8_t alert_mask  = 0xF8; // low level threshold (0% - 31%)
        static constexpr std::uint8_t alert_shift = 3;

        static constexpr std::uint8_t ufg_shift = 1;
        static constexpr std::uint8_t ufg_mask  = 1 << ufg_shift;
    };

    struct Mode
    {
        static constexpr auto ADDRESS = 0x0A;

        Mode() = default;
        explicit Mode(const std::uint8_t raw) : reg{raw}
        {}

        bool is_sleep() const
        {
            return (reg & sleep_mask) == sleep_mask;
        }

        Mode &set_wake_up()
        {
            reg &= ~sleep_mask;
            return *this;
        }

        Mode &set_sleep()
        {
            reg |= sleep_mask;
            return *this;
        }

        Mode &set_quick_start()
        {
            reg |= qstrt_mask;
            return *this;
        }

        Mode &set_power_on_reset()
        {
            reg |= por_mask;
            return *this;
        }

        std::uint8_t get_raw() const
        {
            return reg;
        }

      private:
        std::uint8_t reg{};
        static constexpr std::uint8_t sleep_shift = 6;
        static constexpr std::uint8_t sleep_mask  = 0b11 << sleep_shift;

        static constexpr std::uint8_t qstrt_shift = 4;
        static constexpr std::uint8_t qstrt_mask  = 0b11 << qstrt_shift;

        static constexpr std::uint8_t por_shift = 0;
        static constexpr std::uint8_t por_mask  = 0b1111 << por_shift;
    };

    struct RRT_ALERT
    {
        static constexpr auto ADDRESS_H = 0x06;
        static constexpr auto ADDRESS_L = 0x07;

        RRT_ALERT(const std::uint8_t lsb, const std::uint8_t msb) : reg{static_cast<std::uint16_t>(lsb | (msb << 8))}
        {}
        explicit RRT_ALERT(const std::uint16_t reg) : reg{reg}
        {}

        bool is_alert_triggered() const
        {
            return (reg & 0x8000) == 0;
        }

        RRT_ALERT &clear_alert()
        {
            reg |= 0x8000;
            return *this;
        }

        std::uint16_t rrt() const
        {
            return reg & 0x1FFF;
        }

        std::uint8_t get_raw() const
        {
            return reg;
        }

      private:
        std::uint16_t reg{};
    };

    struct VCELL
    {
        static constexpr auto ADDRESS_H = 0x02;
        static constexpr auto ADDRESS_L = 0x03;

        VCELL(const std::uint8_t lsb, const std::uint8_t msb) : reg{static_cast<std::uint16_t>(lsb | (msb << 8))}
        {}
        explicit VCELL(const std::uint16_t reg) : reg{reg}
        {}

        units::Voltage get_voltage() const
        {
            return reg & 0x3FFF;
        }

        std::uint8_t get_raw() const
        {
            return reg;
        }

      private:
        const std::uint16_t reg{};
    };

    struct SOC
    {
        static constexpr auto ADDRESS_H = 0x04;
        static constexpr auto ADDRESS_L = 0x05;

        SOC(const std::uint8_t lsb, const std::uint8_t msb) : reg{static_cast<std::uint16_t>(lsb | (msb << 8))}
        {}
        explicit SOC(const std::uint16_t reg) : reg{reg}
        {}

        units::Voltage get_soc() const
        {
            return (reg & 0xFF00) >> 8U;
        }

      private:
        const std::uint16_t reg{};
    };

    class BATTINFO
    {
        static constexpr auto BATTERY_INFO_SIZE = 64;
        using Type                              = const std::array<std::uint8_t, BATTERY_INFO_SIZE>;

        /* got from ODM init code */
        static constexpr Type config_info = {
            // BH21P_3400mAh_DM140X_PROFILE_20241106
            0x14, 0x97, 0x39, 0x1B, 0x14, 0x27, 0x4A, 0x5C, 0x64, 0x5E, 0x4B, 0x53, 0x4D, 0x4E, 0x52, 0x56,
            0x5B, 0x5C, 0x5A, 0x5B, 0x4F, 0x67, 0x6B, 0x74, 0x71, 0x62, 0x0A, 0x3E, 0x45, 0x69, 0x83, 0x8F,
            0x8E, 0x89, 0x88, 0x73, 0x45, 0x20, 0xFF, 0x48, 0x09, 0x3B, 0x59, 0x85, 0x8F, 0x90, 0x90, 0x38,
            0x53, 0x84, 0x92, 0x92, 0x80, 0xBF, 0xDE, 0xCB, 0x2F, 0x00, 0x64, 0xA5, 0xB5, 0xC1, 0x46, 0xAE};

      public:
        static constexpr auto ADDRESS = 0x10;

        Type::const_iterator begin() const noexcept
        {
            return config_info.begin();
        }

        Type::const_iterator end() const noexcept
        {
            return config_info.end();
        }

        Type::const_iterator cbegin()
        {
            return config_info.cbegin();
        }

        Type::const_iterator cend()
        {
            return config_info.cend();
        }
    };
} // namespace bsp::devices::power::CW2015Regs
