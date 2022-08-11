// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CT7117.hpp"

#include <array>

namespace
{
    enum class Registers
    {
        Temp          = 0x00,
        Config        = 0x01,
        Low_Temp_Set  = 0x02,
        High_Temp_Set = 0x03,
        ID            = 0x07
    };

    enum class ConfigReg
    {
        OTS  = (1 << 15),
        F1   = (1 << 12),
        F0   = (1 << 11),
        ALTM = (1 << 9),
        SD   = (1 << 8),
        EM   = (1 << 7),
        RES1 = (1 << 6),
        RES0 = (1 << 5),
        TO   = (1 << 4),
        PEC  = (1 << 3),
        CR1  = (1 << 2),
        CR0  = (1 << 1),
        OS   = (1 << 0)
    };

    using namespace bsp::devices::temperature::CT7117;

    std::array<std::uint8_t, 2U> to_bytes(const std::uint16_t value)
    {
        return {static_cast<std::uint8_t>(value & 0xFF00 >> 8U), static_cast<std::uint8_t>(value & 0xFF)};
    }

    std::uint16_t from_bytes(std::array<std::uint8_t, 2U> bytes)
    {
        return bytes[0] << 8U | bytes[1];
    }

    ssize_t write_register16(const std::uint8_t dev_id,
                             drivers::DriverI2C &i2c,
                             const Registers address,
                             const std::uint16_t reg)
    {
        const drivers::I2CAddress addr = {
            .deviceAddress = dev_id, .subAddress = static_cast<std::uint32_t>(address), .subAddressSize = 1};

        return i2c.Write(addr, &to_bytes(reg)[0], sizeof(reg));
    }

    std::optional<std::uint16_t> read_register16(const std::uint8_t dev_id,
                                                 drivers::DriverI2C &i2c,
                                                 const Registers address)
    {
        const drivers::I2CAddress addr = {
            .deviceAddress = dev_id, .subAddress = static_cast<std::uint32_t>(address), .subAddressSize = 1};
        std::array<std::uint8_t, 2U> ret_value{};

        if (const auto result = i2c.Read(addr, &ret_value[0], ret_value.size()); result == ret_value.size()) {
            return from_bytes(ret_value);
        }
        return std::nullopt;
    }

    units::Temperature from_raw(const std::uint16_t raw)
    {
        auto is_sign_present = [](const std::uint16_t raw) { return ((raw & 0x8000) != 0); };

        if (is_sign_present(raw)) {
            const auto integer =
                static_cast<std::uint16_t>(((~raw + 1) & 0x7FFF) >> 7); // remove sign bit and shift to lower byte
            const auto fractional = static_cast<std::uint16_t>(((~raw + 1) & 0x7F) * 0.78125);
            return -1 * (static_cast<float>(integer) + (static_cast<float>(fractional) / 100.0));
        }
        else {
            const auto integer =
                static_cast<std::uint16_t>((raw & 0x7FFF) >> 7); // remove sign bit and shift to lower byte
            const auto fractional = static_cast<std::uint16_t>((raw & 0x7F) * 0.78125);
            return static_cast<float>(integer) + (static_cast<float>(fractional) / 100.0);
        }
    }

    std::uint16_t adjust_resolution(const std::uint16_t value)
    {
        /// Resolution = 0.25°C
        return value & 0xFFE0;
    }
} // namespace

namespace bsp::devices::temperature::CT7117
{

    CT7117::CT7117(const std::uint8_t id, drivers::DriverI2C &i2c) : device_id{id}, i2c{i2c}
    {}

    CT7117::~CT7117()
    {
        standby();
    }

    bool CT7117::standby()
    {
        auto reg = read_register16(device_id, i2c, Registers::Config);
        if (not reg) {
            return false;
        }
        *reg |= static_cast<std::uint16_t>(ConfigReg::SD);
        return write_register16(device_id, i2c, Registers::Config, *reg) == sizeof(*reg);
    }

    bool CT7117::wakeup()
    {
        auto reg = read_register16(device_id, i2c, Registers::Config);
        if (not reg) {
            return false;
        }
        *reg &= ~(static_cast<std::uint16_t>(ConfigReg::SD));
        return write_register16(device_id, i2c, Registers::Config, *reg) == sizeof(*reg);
    }

    std::optional<units::Temperature> CT7117::get_temperature() const
    {
        auto reg = read_register16(device_id, i2c, Registers::Temp);
        if (not reg) {
            return std::nullopt;
        }

        return from_raw(adjust_resolution(*reg));
    }

    bool CT7117::poll() const
    {
        constexpr auto DEVICE_ID      = 0x59;
        const drivers::I2CAddress reg = {
            .deviceAddress = device_id, .subAddress = static_cast<std::uint32_t>(Registers::ID), .subAddressSize = 1};

        std::uint8_t id{};
        const auto id_size = sizeof id;

        return i2c.Read(reg, &id, id_size) == id_size and id == DEVICE_ID;
    }
} // namespace bsp::devices::temperature::CT7117
