// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#pragma once

#include <array>
#include <Units.hpp>

namespace bsp::devices::power::CW2015Regs
{
    constexpr inline auto ADDRESS = 0x62;

    struct Config
    {
        constexpr static auto ADDRESS = 0x08;

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
        constexpr static auto ADDRESS = 0x0A;

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
        constexpr static auto ADDRESS_H = 0x06;
        constexpr static auto ADDRESS_L = 0x07;

        RRT_ALERT(const std::uint8_t lsb, const std::uint8_t msb) : reg{static_cast<uint16_t>(lsb | (msb << 8))}
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
        constexpr static auto ADDRESS_H = 0x02;
        constexpr static auto ADDRESS_L = 0x03;

        VCELL(const std::uint8_t lsb, const std::uint8_t msb) : reg{static_cast<uint16_t>(lsb | (msb << 8))}
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
        constexpr static auto ADDRESS_H = 0x04;
        constexpr static auto ADDRESS_L = 0x05;

        SOC(const std::uint8_t lsb, const std::uint8_t msb) : reg{static_cast<uint16_t>(lsb | (msb << 8))}
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
        constexpr static auto BATTERY_INFO_SIZE = 64;
        using Type                              = const std::array<std::uint8_t, BATTERY_INFO_SIZE>;

        /* got from ODM init code */
        constexpr static Type config_info = {
            // profile_DEM50X_2nd_20211012
            0x15, 0x15, 0x6E, 0x67, 0x65, 0x62, 0x60, 0x60, 0x5F, 0x5E, 0x5B, 0x59, 0x55, 0x50, 0x41, 0x33,
            0x2A, 0x26, 0x24, 0x27, 0x31, 0x46, 0x55, 0x5B, 0x47, 0x4A, 0x0A, 0x3E, 0x38, 0x58, 0x59, 0x63,
            0x67, 0x63, 0x62, 0x64, 0x3D, 0x1B, 0x6F, 0x15, 0x07, 0x21, 0x54, 0x85, 0x8F, 0x90, 0x90, 0x44,
            0x63, 0x86, 0x94, 0x99, 0x80, 0x89, 0xBC, 0xCB, 0x2F, 0x00, 0x64, 0xA5, 0xB5, 0xC1, 0x46, 0xAE};

      public:
        constexpr static auto ADDRESS = 0x10;

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
