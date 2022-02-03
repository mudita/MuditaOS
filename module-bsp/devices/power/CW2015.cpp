// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CW2015.hpp"
#include "CW2015Regs.hpp"
#include "FreeRTOS.h"
#include "task.h"

/// CW2015 tips&tricks
/// 1. Setting POR bits in Mode register doesn't seem to reset the state of registers. Nevertheless it is required to
/// perform proper profile update.
/// 2. Setting POR, QRST or SLEEP bits always need to be followed with the wake-up. Slight delay (1ms is enough) between
/// two commands is also needed.
/// 3. Loading proper battery profile is crucial to the operation of the chip. It needs to be loaded each time the chip
/// is powered.

namespace
{
    constexpr std::uint32_t i2c_subaddr_size = 1;

} // namespace

namespace bsp::devices::power
{
    using namespace CW2015Regs;

    CW2015::CW2015(drivers::DriverI2C &i2c, units::SOC soc) : i2c{i2c}, alert_threshold{soc}
    {
        status = init_chip();
    }
    CW2015::~CW2015()
    {
        sleep();
    }
    std::optional<units::SOC> CW2015::get_battery_soc()
    {
        // Only higher byte of SOC register pair is needed here. Accuracy will be enough
        if (const auto result = read(SOC::ADDRESS_H)) {
            return *result;
        }
        else {
            return std::nullopt;
        }
    }
    std::optional<units::Voltage> CW2015::get_battery_voltage()
    {
        constexpr auto median_samples          = 3;
        constexpr auto micro_to_milli_ratio    = 1000;
        constexpr auto adc_conversion_constant = 305;

        uint8_t volt_h, volt_l;
        uint32_t ADMin = 0, ADMax = 0, ADResult = 0;

        /// Filter data using median
        /// https://en.wikipedia.org/wiki/Median
        for (int i = 0; i < median_samples; i++) {
            const auto lsb = read(VCELL::ADDRESS_L);
            const auto msb = read(VCELL::ADDRESS_H);
            if (not lsb or not msb) {
                return std::nullopt;
            }

            const auto vtt = VCELL{*lsb, *msb};

            const auto ADValue = vtt.get_voltage();
            if (i == 0) {
                ADMin = ADValue;
                ADMax = ADValue;
            }

            if (ADValue < ADMin) {
                ADMin = ADValue;
            }
            if (ADValue > ADMax) {
                ADMax = ADValue;
            }

            ADResult += ADValue;
        }
        ADResult -= ADMin;
        ADResult -= ADMax;

        return ADResult * adc_conversion_constant / micro_to_milli_ratio; // mV
    }
    CW2015::RetCodes CW2015::clear_irq()
    {
        const auto lsb = read(RRT_ALERT::ADDRESS_L);
        const auto msb = read(RRT_ALERT::ADDRESS_H);
        if (not lsb or not msb) {
            return RetCodes::CommunicationError;
        }
        auto rrt = RRT_ALERT{*lsb, *msb};
        if (not write(RRT_ALERT::ADDRESS_H, rrt.clear_alert().get_raw())) {
            return RetCodes::CommunicationError;
        }

        return RetCodes::Ok;
    }

    CW2015::RetCodes CW2015::init_chip()
    {
        if (const auto result = wake_up(); result != RetCodes::Ok) {
            return result;
        }

        /// Clear any pending interrupts(soc alarm)
        if (const auto result = clear_irq(); result != RetCodes::Ok) {
            return result;
        }

        if (const auto result = set_soc_threshold(alert_threshold); result != RetCodes::Ok) {
            return result;
        }

        if (const auto update = is_update_needed()) {
            if (*update) {
                LOG_INFO("Loading battery profile...");
                if (const auto res = load_profile(); res != RetCodes::Ok) {
                    return res;
                }
            }
        }
        else {
            return RetCodes::CommunicationError;
        }

        return wait_for_ready();
    }

    void CW2015::reinit()
    {
        status = init_chip();
    }

    CW2015::RetCodes CW2015::load_profile()
    {
        if (const auto result = write_profile(); result != RetCodes::Ok) {
            return result;
        }

        if (const auto result = verify_profile(); result != RetCodes::Ok) {
            return result;
        }

        if (const auto result = set_update_flag(); result != RetCodes::Ok) {
            return result;
        }
        /// Invoking reset procedure seems crucial for the chip to correctly load the new profile and show correct SOC
        /// at startup
        if (const auto result = reset_chip(); result != RetCodes::Ok) {
            return result;
        }

        return RetCodes::Ok;
    }
    CW2015::RetCodes CW2015::reset_chip()
    {
        if (not write(Mode::ADDRESS, Mode{}.set_power_on_reset().get_raw())) {
            return RetCodes::CommunicationError;
        }
        vTaskDelay(pdMS_TO_TICKS(1));
        if (not write(Mode::ADDRESS, Mode{}.set_wake_up().get_raw())) {
            return RetCodes::CommunicationError;
        }
        return RetCodes::Ok;
    }
    void CW2015::irq_handler()
    {
        if (const auto result = clear_irq(); result != RetCodes::Ok) {
            LOG_ERROR("Error during handling irq, %s", magic_enum::enum_name(result).data());
        }
    }
    void CW2015::init_irq_pin(std::shared_ptr<drivers::DriverGPIO> gpio, uint32_t pin)
    {
        drivers::DriverGPIOPinParams ALRTPinConfig{};
        ALRTPinConfig.dir      = drivers::DriverGPIOPinParams::Direction::Input;
        ALRTPinConfig.irqMode  = drivers::DriverGPIOPinParams::InterruptMode::IntFallingEdge;
        ALRTPinConfig.defLogic = 0;
        ALRTPinConfig.pin      = pin;
        gpio->ConfPin(ALRTPinConfig);

        gpio->EnableInterrupt(1 << pin);
    }
    CW2015::RetCodes CW2015::quick_start()
    {
        if (not write(Mode::ADDRESS, Mode{}.set_quick_start().get_raw())) {
            return RetCodes::CommunicationError;
        }
        vTaskDelay(pdMS_TO_TICKS(1));
        if (not write(Mode::ADDRESS, Mode{}.set_wake_up().get_raw())) {
            return RetCodes::CommunicationError;
        }
        return RetCodes::Ok;
    }
    CW2015::RetCodes CW2015::poll() const
    {
        return status;
    }

    CW2015::RetCodes CW2015::sleep()
    {
        return write(Mode::ADDRESS, Mode{}.set_sleep().get_raw()) ? RetCodes::Ok : RetCodes::CommunicationError;
    }

    CW2015::RetCodes CW2015::wake_up()
    {
        return write(Mode::ADDRESS, Mode{}.set_wake_up().get_raw()) ? RetCodes::Ok : RetCodes::CommunicationError;
    }

    CW2015::RetCodes CW2015::set_soc_threshold(const std::uint8_t threshold)
    {
        const auto result = read(Config::ADDRESS);
        if (not result) {
            return RetCodes::CommunicationError;
        }
        /// Update only if the current one is different than requested
        auto config = Config{*result};
        if (config.get_alert_threshold() != threshold) {
            return write(Config::ADDRESS, config.set_threshold(threshold).get_raw()) ? RetCodes::Ok
                                                                                     : RetCodes::CommunicationError;
        }
        return RetCodes::Ok;
    }
    CW2015::TriState CW2015::is_update_needed()
    {
        const auto result = read(Config::ADDRESS);
        if (not result) {
            return std::nullopt;
        }
        /// Inverted logic here, low state means CW2015 needs battery profile updated
        return not Config{*result}.is_ufg_set();
    }
    CW2015::TriState CW2015::is_sleep_mode()
    {
        const auto result = read(Mode::ADDRESS);
        if (not result) {
            return std::nullopt;
        }

        return Mode{*result}.is_sleep();
    }

    /// This will tell CW2015 to use the new battery profile
    CW2015::RetCodes CW2015::set_update_flag()
    {
        const auto result = read(Config::ADDRESS);
        if (not result) {
            return RetCodes::CommunicationError;
        }
        return write(Config::ADDRESS, Config{*result}.set_ufg(true).get_raw()) ? RetCodes::Ok
                                                                               : RetCodes::CommunicationError;
    }
    CW2015::RetCodes CW2015::wait_for_ready()
    {
        constexpr auto max_battery_soc = 100;
        constexpr auto poll_retries    = 20U;
        constexpr auto poll_interval   = pdMS_TO_TICKS(50); /// 20 * 50ms = 1sec
        RetCodes ret_code{RetCodes::NotReady};

        std::int8_t poll_count{poll_retries};
        while (poll_count-- > 0) {
            const auto soc     = get_battery_soc();
            const auto voltage = get_battery_voltage();
            if (not soc or not voltage) {
                return RetCodes::CommunicationError;
            }
            else if (*soc <= max_battery_soc and *voltage > 0) {
                return RetCodes::Ok;
            }
            vTaskDelay(poll_interval);
        }
        return ret_code;
    }
    CW2015::RetCodes CW2015::verify_profile()
    {
        BATTINFO profile{};
        RetCodes ret_code{RetCodes::Ok};

        const auto result = std::all_of(
            profile.cbegin(), profile.cend(), [this, ret_code, reg = BATTINFO::ADDRESS](const auto &e) mutable {
                const auto result = read(reg++);
                if (not result) {
                    ret_code = RetCodes::CommunicationError;
                    return false;
                }
                if (*result != e) {
                    ret_code = RetCodes::ProfileInvalid;
                    return false;
                }
                return true;
            });

        return ret_code;
    }
    void CW2015::dumpRegisters()
    {
        const auto mode   = read(Mode::ADDRESS);
        const auto config = read(Config::ADDRESS);
        const auto soc    = read(SOC::ADDRESS_H);
        LOG_DEBUG("Mode: 0x%x", *mode);
        LOG_DEBUG("Config: 0x%x", *config);
        LOG_DEBUG("SOC: 0x%x", *soc);
    }

    CW2015::RetCodes CW2015::write_profile()
    {
        BATTINFO profile{};

        const auto result =
            std::all_of(profile.cbegin(), profile.cend(), [this, reg = BATTINFO::ADDRESS](const auto &e) mutable {
                return write(reg++, e);
            });

        return result ? RetCodes::Ok : RetCodes::CommunicationError;
    }

    std::optional<std::uint8_t> CW2015::read(const std::uint8_t reg)
    {
        const drivers::I2CAddress fuelGaugeAddress = {ADDRESS, reg, i2c_subaddr_size};
        std::uint8_t ret_val{};
        if (const auto result = i2c.Read(fuelGaugeAddress, &ret_val, i2c_subaddr_size); result == i2c_subaddr_size) {
            return ret_val;
        }
        else {
            return std::nullopt;
        }
    }
    bool CW2015::write(const std::uint8_t reg, const std::uint8_t value)
    {
        const drivers::I2CAddress fuelGaugeAddress = {ADDRESS, reg, i2c_subaddr_size};
        return i2c.Write(fuelGaugeAddress, &value, i2c_subaddr_size) == i2c_subaddr_size;
    }
    CW2015::operator bool() const
    {
        return status == RetCodes::Ok;
    }
    CW2015::RetCodes CW2015::calibrate()
    {
        return quick_start();
    }

} // namespace bsp::devices::power
