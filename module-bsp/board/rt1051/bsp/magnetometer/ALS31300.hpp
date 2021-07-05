// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <array>
#include <cstdint>
#include <cstring>
#include <chrono>

// ALS31300 magnetometer driver
namespace drivers::als31300
{

    // Note: this device returns 32-bit register values in MSB order

    using whole_reg_t = uint32_t; // ALS31300 always talks 4 bytes

    constexpr auto I2C_ADDRESS = 0x64;

    // base ALS31300 register struct.
    struct base_reg
    {
        virtual operator whole_reg_t() const = 0;
    };

    // REGISTER DEFINITIONS

    // there is no 0x01 register
    // --------
    constexpr auto CONF_REG = 0x02;
    struct conf_reg : base_reg
    {
        conf_reg(whole_reg_t whole_reg);
        operator whole_reg_t() const override;
        uint8_t user_eeprom : 5;
        bool int_latch_enable : 1;
        bool channel_X_en : 1;
        bool channel_Y_en : 1;
        bool channel_Z_en : 1;
        bool I2C_threshold : 1;
        uint8_t slave_addr : 7;
        bool disable_slave_ADC : 1;
        bool I2C_CRC_en : 1;
        uint8_t hall_mode : 2;
        uint8_t bandwidth : 3;
        uint8_t RESERVED : 8;
    };

    constexpr auto CONF_REG_LATCH_disabled = 0b0;
    constexpr auto CONF_REG_LATCH_enabled  = 0b1;

    constexpr auto CONF_REG_CHANNEL_disabled = 0b0;
    constexpr auto CONF_REG_CHANNEL_enabled  = 0b1;

    constexpr auto CONF_REG_I2C_THRES_3v0 = 0b0;
    constexpr auto CONF_REG_I2C_THRES_1v8 = 0b1;

    // --------
    constexpr auto INT_REG = 0x03;

    struct int_reg : base_reg
    {
        int_reg(whole_reg_t whole_reg);
        operator whole_reg_t() const override;
        uint8_t int_X_threshold : 6;
        uint8_t int_Y_threshold : 6;
        uint8_t int_Z_threshold : 6;
        bool int_X_en : 1;
        bool int_Y_en : 1;
        bool int_Z_en : 1;
        bool int_eeprom_en : 1;
        bool int_eeprom_status : 1;
        bool int_mode : 1;
        bool int_threshold_signed : 1;
        uint8_t RESERVED : 7;
    };

    constexpr auto INT_REG_INT_CHANNEL_disabled = 0b0;
    constexpr auto INT_REG_INT_CHANNEL_enabled  = 0b1;

    constexpr auto INT_REG_INT_MODE_threshold = 0b0;
    constexpr auto INT_REG_INT_MODE_delta     = 0b1;

    constexpr auto INT_REG_THRESHOLD_absolute = 0b0;
    constexpr auto INT_REG_THRESHOLD_signed   = 0b1;

    constexpr auto INT_REG_INT_EEPROM_disable = 0b0;
    constexpr auto INT_REG_INT_EEPROM_enable  = 0b1;

    // --------
    constexpr auto PWR_REG = 0x27;

    struct pwr_reg : base_reg
    {
        pwr_reg(whole_reg_t whole_reg);
        operator whole_reg_t() const override;
        uint8_t sleep : 2;
        uint8_t I2C_loop_mode : 2;
        uint8_t count_max_LP_mode : 3;
        uint32_t RESERVED : 25;
    };

    constexpr auto PWR_REG_SLEEP_MODE_active = 0b00;
    constexpr auto PWR_REG_SLEEP_MODE_sleep  = 0b01;
    constexpr auto PWR_REG_SLEEP_MODE_LPDCM  = 0b10; // Low-Power Duty Cycle Mode

    constexpr auto PWR_REG_LOOP_MODE_single    = 0b00;
    constexpr auto PWR_REG_LOOP_MODE_fast_loop = 0b01;
    constexpr auto PWR_REG_LOOP_MODE_full_loop = 0b10;

    enum PWR_REG_SLEEP_MODE
    {
        active          = PWR_REG_SLEEP_MODE_active,
        sleep           = PWR_REG_SLEEP_MODE_sleep,
        periodic_active = PWR_REG_SLEEP_MODE_LPDCM,
    };

    constexpr auto PWR_ON_DELAY_MS = 1; //  spec'd as 600µs at most

    // --------
    constexpr auto MEASUREMENTS_MSB_REG = 0x28;

    struct measurements_MSB_reg : base_reg
    {
        measurements_MSB_reg(whole_reg_t whole_reg);
        operator whole_reg_t() const override;
        uint8_t temperature_MSB : 6;
        bool int_flag : 1;
        bool new_data_flag : 1;
        uint8_t Z_MSB : 8;
        uint8_t Y_MSB : 8;
        uint8_t X_MSB : 8;
    };

    constexpr auto MEAS_REG_NEW_DATA_not_avail = 0b0;
    constexpr auto MEAS_REG_NEW_DATA_available = 0b1;

    // --------
    constexpr auto MEASUREMENTS_LSB_REG = 0x29;

    struct measurements_LSB_reg : base_reg
    {
        measurements_LSB_reg(whole_reg_t whole_reg);
        operator whole_reg_t() const override;
        uint8_t temperature_LSB : 6;
        uint8_t hall_mode_status : 2;
        uint8_t Z_LSB : 4;
        uint8_t Y_LSB : 4;
        uint8_t X_LSB : 4;
        bool int_eeprom_write_pending : 1;
        uint16_t RESERVED : 11;
    };

    float temperature_convert(uint16_t raw_temperature);

    // NOTE: device sensitivity HW fixed at 4 LSB/Gauss == 0.4 LSB/mT
    // All measurements are supposed to be raw 4 LSB/Gauss. No need to introduce fractions
    constexpr auto MEASUREMENTS_REG_bitlength_full_scale = 12;

    int16_t measurement_sign_convert(uint16_t raw_measurement,
                                     uint8_t bit_length = MEASUREMENTS_REG_bitlength_full_scale);
    // --------
    constexpr uint8_t CUSTOMER_ACCESS_REG          = 0x35;
    constexpr whole_reg_t CUSTOMER_ACCESS_REG_code = 0x2C413534;

    // --------

    /// give eeprom registers time to complete WRITE
    constexpr inline std::array<std::uint8_t, 3> EEPROM_REGS = {CONF_REG, INT_REG, CUSTOMER_ACCESS_REG};

    using std::chrono_literals::operator""ms;
    constexpr auto EEPROM_REG_WRITE_DELAY_MS = 60ms; // docs say 50ms, +10ms for good measure

    /////////////////////
} // namespace drivers::als31300
