// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ALS31300.hpp"

namespace drivers::als31300
{

    conf_reg::conf_reg(whole_reg_t whole_reg)
    {
        user_eeprom       = whole_reg & 0b11111;
        int_latch_enable  = (whole_reg >> 5) & 0b1;
        channel_X_en      = (whole_reg >> 6) & 0b1;
        channel_Y_en      = (whole_reg >> 7) & 0b1;
        channel_Z_en      = (whole_reg >> 8) & 0b1;
        I2C_threshold     = (whole_reg >> 9) & 0b1;
        slave_addr        = (whole_reg >> 10) & 0b1111111;
        disable_slave_ADC = (whole_reg >> 17) & 0b1;
        I2C_CRC_en        = (whole_reg >> 18) & 0b1;
        hall_mode         = (whole_reg >> 19) & 0b11;
        bandwidth         = (whole_reg >> 21) & 0b111;
        RESERVED          = (whole_reg >> 24) & 0xFF;
    }

    conf_reg::operator whole_reg_t() const
    {
        return (user_eeprom & 0b11111) | (int_latch_enable & 0b1) << 5 | (channel_X_en & 0b1) << 6 |
               (channel_Y_en & 0b1) << 7 | (channel_Z_en & 0b1) << 8 | (I2C_threshold & 0b1) << 9 |
               (slave_addr & 0b1111111) << 10 | (disable_slave_ADC & 0b1) << 17 | (I2C_CRC_en & 0b1) << 18 |
               (hall_mode & 0b11) << 19 | (bandwidth & 0b111) << 21 | (RESERVED & 0xFF) << 24;
    }

    int_reg::int_reg(whole_reg_t whole_reg)
    {
        int_X_threshold      = whole_reg & 0b111111;
        int_Y_threshold      = (whole_reg >> 6) & 0b111111;
        int_Z_threshold      = (whole_reg >> 12) & 0b111111;
        int_X_en             = (whole_reg >> 18) & 0b1;
        int_Y_en             = (whole_reg >> 19) & 0b1;
        int_Z_en             = (whole_reg >> 20) & 0b1;
        int_eeprom_en        = (whole_reg >> 21) & 0b1;
        int_eeprom_status    = (whole_reg >> 22) & 0b1;
        int_mode             = (whole_reg >> 23) & 0b1;
        int_threshold_signed = (whole_reg >> 24) & 0b1;
        RESERVED             = (whole_reg >> 25) & 0b1111111;
    }

    int_reg::operator whole_reg_t() const
    {
        return (int_X_threshold & 0b111111) | (int_Y_threshold & 0b111111) << 6 | (int_Z_threshold & 0b111111) << 12 |
               (int_X_en & 0b1) << 18 | (int_Y_en & 0b1) << 19 | (int_Z_en & 0b1) << 20 | (int_eeprom_en & 0b1) << 21 |
               (int_eeprom_status & 0b1) << 22 | (int_mode & 0b1) << 23 | (int_threshold_signed & 0b1) << 24 |
               (RESERVED & 0b1111111) << 25;
    }

    pwr_reg::pwr_reg(whole_reg_t whole_reg)
    {
        sleep             = whole_reg & 0b11;
        I2C_loop_mode     = (whole_reg >> 2) & 0b11;
        count_max_LP_mode = (whole_reg >> 4) & 0b111;
        RESERVED          = (whole_reg >> 7) & 0x1FFFFFF;
    }

    pwr_reg::operator whole_reg_t() const
    {
        return (sleep & 0b11) | (I2C_loop_mode & 0b11) << 2 | (count_max_LP_mode & 0b111) << 4 |
               (RESERVED & 0x1FFFFFF) << 7;
    }

    measurements_MSB_reg::measurements_MSB_reg(whole_reg_t whole_reg)
    {
        temperature_MSB = whole_reg & 0b111111;
        int_flag        = (whole_reg >> 6) & 0b1;
        new_data_flag   = (whole_reg >> 7) & 0b1;
        Z_MSB           = (whole_reg >> 8) & 0xFF;
        Y_MSB           = (whole_reg >> 16) & 0xFF;
        X_MSB           = (whole_reg >> 24) & 0xFF;
    }

    measurements_MSB_reg::operator whole_reg_t() const
    {
        return (temperature_MSB & 0b111111) | (int_flag & 0b1) << 6 | (new_data_flag & 0b1) << 7 | (Z_MSB & 0xFF) << 8 |
               (Y_MSB & 0xFF) << 16 | (X_MSB & 0xFF) << 24;
    }

    measurements_LSB_reg::measurements_LSB_reg(whole_reg_t whole_reg)
    {
        temperature_LSB          = whole_reg & 0b111111;
        hall_mode_status         = (whole_reg >> 6) & 0b11;
        Z_LSB                    = (whole_reg >> 8) & 0b1111;
        Y_LSB                    = (whole_reg >> 12) & 0b1111;
        X_LSB                    = (whole_reg >> 16) & 0b1111;
        int_eeprom_write_pending = (whole_reg >> 20) & 0b1;
        RESERVED                 = (whole_reg >> 21) & 0x7FF;
    }

    measurements_LSB_reg::operator whole_reg_t() const
    {
        return (temperature_LSB & 0b111111) | (hall_mode_status & 0b11) << 6 | (Z_LSB & 0b1111) << 8 |
               (Y_LSB & 0b1111) << 12 | (X_LSB & 0b1111) << 16 | (int_eeprom_write_pending & 0b1) << 20 |
               (RESERVED & 0x7FF) << 21;
    }

    float temperature_convert(uint16_t raw_temperature)
    {
        const int32_t intermediate = raw_temperature - 1708;
        return intermediate * 0.0737;
    }

    int16_t measurement_sign_convert(uint16_t raw_measurement, uint8_t bit_length)
    {
        // via: https://stackoverflow.com/questions/16946801/n-bit-2s-binary-to-decimal-in-c
        const auto sign_flag = 1 << (bit_length - 1);
        if (raw_measurement & sign_flag) {
            raw_measurement |= -(1 << bit_length);
        }
        return raw_measurement;
    }
} // namespace drivers::als31300
