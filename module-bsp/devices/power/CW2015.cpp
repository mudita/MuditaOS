// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CW2015.hpp"
#include "CW2015CHBD.hpp"
#include "FreeRTOS.h"
#include "task.h"

namespace
{
    using namespace bsp::power;
    auto constexpr FuelGaugeAlertCapacityPercent                                                         = 5;
    constexpr std::uint32_t i2cSubaddresSize                                                             = 1;
    constexpr inline auto MAGIC_LOOP_VALUE_DEFINED_BY_CHIP_VENDOR                                        = 30;
    constexpr inline auto MAGIC_VALUE_DEFINING_MAXIMUM_BATTERY_CAPACITY_IN_PERCENT                       = 100;
    constexpr inline auto MAGIC_VALUE_REPRESENTING_VOLTAGE_RESOLUTION_FOR_AD_CONVERTER_IN_MICROVOLTS     = 305;
    constexpr inline auto MAGIC_VALUE_REPRESENTING_DECIMAL_MULTIPLIER_TO_CONVERT_MICROVOLTS_TO_MILIVOLTS = 1000;
    constexpr inline auto MAGIC_VALUE_DEFINING_HOW_MANY_SAMPLES_ARE_TAKEN_TO_CALCULATE_MEDIAN            = 3;

    constexpr inline auto CW201X_ATHD =
        (static_cast<uint8_t>(FuelGaugeAlertCapacityPercent) << static_cast<uint8_t>(CONFIG::shift)) &
        static_cast<uint8_t>(CONFIG::mask);

    enum class CW201xRetCode : int
    {
        Success = 0,
        Error   = 1
    };

    std::shared_ptr<drivers::DriverGPIO> gpio;

    drivers::I2CAddress fuelGaugeAddress = {FUEL_GAUGE_I2C_ADDR, 0, i2cSubaddresSize};
} // namespace

namespace bsp::devices::power
{
    CW2015::CW2015(std::shared_ptr<drivers::DriverI2C> i2c) : i2c{std::move(i2c)}
    {
        state = init_chip();
    }
    CW2015::~CW2015()
    {
        // enable sleep
        uint8_t mode                = static_cast<uint8_t>(MODE::Sleep);
        fuelGaugeAddress.subAddress = static_cast<std::uint32_t>(static_cast<std::uint32_t>(Registers::MODE));
        auto ret                    = i2c->Write(fuelGaugeAddress, &mode, sizeof(uint8_t));

        if (ret != sizeof(uint8_t)) {
            LOG_ERROR("failed to put fuel gauge IC to sleep");
        }
    }
    std::optional<units::SOC> CW2015::get_battery_soc()
    {
        uint8_t level, reset_loop = 0;

        /// Added here to test how the quick start procedure affects SOC calculation. It probably should be moved to a
        /// different place, where the current draw is significantly smaller.
        quick_start();

        // Only higher byte of SOC register pair is needed here. Accuracy will be enough
        fuelGaugeAddress.subAddress = static_cast<std::uint32_t>(Registers::SOC_H);
        const auto ret              = i2c->Read(fuelGaugeAddress, &level, sizeof(uint8_t));

        if (ret != sizeof(uint8_t)) {
            return {};
        }

        if (level > MAGIC_VALUE_DEFINING_MAXIMUM_BATTERY_CAPACITY_IN_PERCENT) {
            // error in getting capacity in %. It should be less than
            // MAGIC_VALUE_DEFINING_MAXIMUM_BATTERY_CAPACITY_IN_PERCENT
            reset_loop++;
            if (reset_loop > 5) {
                if (reset_chip()) // por ic
                    level = 0;
                reset_loop = 0;
            }
            return {};
        }
        else {
            reset_loop = 0;
        }

        return level;
    }
    units::Voltage CW2015::get_battery_voltage()
    {
        uint8_t volt_h, volt_l;
        uint32_t ADMin = 0, ADMax = 0, ADResult = 0;

        /* Filter data - median*/
        for (int i = 0; i < MAGIC_VALUE_DEFINING_HOW_MANY_SAMPLES_ARE_TAKEN_TO_CALCULATE_MEDIAN; i++) {
            fuelGaugeAddress.subAddress = static_cast<std::uint32_t>(Registers::VCELL_H);
            auto ret                    = i2c->Read(fuelGaugeAddress, &volt_h, sizeof(uint8_t));
            fuelGaugeAddress.subAddress = static_cast<std::uint32_t>(Registers::VCELL_L);
            ret += i2c->Read(fuelGaugeAddress, &volt_l, sizeof(uint8_t));
            if (ret != sizeof(uint16_t)) {
                LOG_ERROR("failed to read fuel gauge alert");
            }

            uint16_t ADValue = (static_cast<uint16_t>(volt_h) << 8) | static_cast<uint16_t>(volt_l);
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

        /* Note: if above looks strange than it has to be explained that this is step sister of mean value called median
         * (ref: https://en.wikipedia.org/wiki/Median) */

        return static_cast<int>(ADResult * MAGIC_VALUE_REPRESENTING_VOLTAGE_RESOLUTION_FOR_AD_CONVERTER_IN_MICROVOLTS /
                                MAGIC_VALUE_REPRESENTING_DECIMAL_MULTIPLIER_TO_CONVERT_MICROVOLTS_TO_MILIVOLTS); // mV
    }
    void CW2015::clear_irq()
    {
        uint8_t rrt;

        fuelGaugeAddress.subAddress = static_cast<std::uint32_t>(static_cast<std::uint32_t>(Registers::SOC_H));
        auto ret                    = i2c->Read(fuelGaugeAddress, &rrt, sizeof(uint8_t));
        if (ret != sizeof(uint8_t)) {
            LOG_ERROR("failed to read fuel gauge alert");
        }
        // clear flag
        rrt &= ~static_cast<uint8_t>(RRT_ALRT::nALRT);

        ret = i2c->Write(fuelGaugeAddress, &rrt, sizeof(uint8_t));
        if (ret != sizeof(uint8_t)) {
            LOG_ERROR("failed to reset fuel gauge alert flag");
        }
    }

    std::int8_t CW2015::init_chip()
    {
        uint8_t i, RegVal = static_cast<uint8_t>(MODE::NORMAL);

        /* wake up cw2015/13 from sleep mode */
        fuelGaugeAddress.subAddress = static_cast<std::uint32_t>(Registers::MODE);
        auto ret                    = i2c->Write(fuelGaugeAddress, &RegVal, sizeof(uint8_t));
        if (ret != 1) {
            LOG_ERROR("Fuel gauge: write CW201X_REG_MODE error!");
            return static_cast<uint8_t>(CW201xRetCode::Error);
        }

        /* check CW201X_ATHD if not right */
        fuelGaugeAddress.subAddress = static_cast<std::uint32_t>(Registers::CONFIG);
        ret                         = i2c->Read(fuelGaugeAddress, &RegVal, sizeof(uint8_t));
        if (ret != 1) {
            LOG_ERROR("Fuel gauge: read CW201X_REG_CONFIG error!");
            return static_cast<uint8_t>(CW201xRetCode::Error);
        }
        if ((RegVal & 0xf8) != CW201X_ATHD) {
            //"the new CW201X_ATHD need set"
            RegVal &= ~(static_cast<uint8_t>(CONFIG::mask)); /* clear ATHD */
            RegVal |= CW201X_ATHD;                           /* set CW201X_ATHD */
            fuelGaugeAddress.subAddress = static_cast<std::uint32_t>(Registers::CONFIG);
            ret                         = i2c->Write(fuelGaugeAddress, &RegVal, sizeof(uint8_t));
            if (ret != 1) {
                LOG_ERROR("Fuel gauge: write CW201X_REG_CONFIG error!");
                return static_cast<uint8_t>(CW201xRetCode::Error);
            }
        }

        /* check config_update_flag if not right */
        fuelGaugeAddress.subAddress = static_cast<std::uint32_t>(Registers::CONFIG);
        ret                         = i2c->Read(fuelGaugeAddress, &RegVal, sizeof(uint8_t));
        if (ret != 1) {
            LOG_ERROR("Fuel gauge: read CW201X_REG_CONFIG error!");
            return static_cast<uint8_t>(CW201xRetCode::Error);
        }
        if (!(RegVal & static_cast<uint8_t>(CONFIG::UFG))) {
            //"update flag for new battery info need set"
            if (update_chip_config()) {
                LOG_ERROR("Fuel gauge: update_config_info error!");
                return static_cast<uint8_t>(CW201xRetCode::Error);
            }
        }
        else {
            for (i = 0; i < BATTERY_INFO_SIZE; i++) {
                fuelGaugeAddress.subAddress = static_cast<std::uint32_t>(Registers::BATINFO) + i;
                ret                         = i2c->Read(fuelGaugeAddress, &RegVal, sizeof(uint8_t));
                if (ret != 1) {
                    LOG_ERROR("Fuel gauge: read CW201X_REG_BATINFO error!");
                    return static_cast<uint8_t>(CW201xRetCode::Error);
                }
                if (battery_info_config_info[i] != RegVal) {
                    break;
                }
            }

            if (i != BATTERY_INFO_SIZE) {
                RegVal                      = static_cast<uint8_t>(MODE::Sleep);
                fuelGaugeAddress.subAddress = static_cast<std::uint32_t>(Registers::MODE);
                ret                         = i2c->Write(fuelGaugeAddress, &RegVal, sizeof(uint8_t));
                if (ret != 1) {
                    LOG_ERROR("Fuel gauge: write CW201X_REG_MODE error!");
                    return static_cast<uint8_t>(CW201xRetCode::Error);
                }

                vTaskDelay(30); // delay 30ms

                RegVal                      = static_cast<uint8_t>(MODE::NORMAL);
                fuelGaugeAddress.subAddress = static_cast<std::uint32_t>(Registers::MODE);
                ret                         = i2c->Write(fuelGaugeAddress, &RegVal, sizeof(uint8_t));
                if (ret != 1) {
                    LOG_ERROR("Fuel gauge: write CW201X_REG_MODE error!");
                    return static_cast<uint8_t>(CW201xRetCode::Error);
                }

                //"update flag for new battery info need set"
                if (update_chip_config()) {
                    LOG_ERROR("Fuel gauge: update_config_info error!");
                    return static_cast<uint8_t>(CW201xRetCode::Error);
                }
            }
        }
        /* check SOC if not equal 255 */
        for (i = 0; i < MAGIC_LOOP_VALUE_DEFINED_BY_CHIP_VENDOR; i++) {
            vTaskDelay(100); // delay 100ms
            fuelGaugeAddress.subAddress = static_cast<std::uint32_t>(Registers::SOC_H);
            ret                         = i2c->Read(fuelGaugeAddress, &RegVal, sizeof(uint8_t));
            if (ret != 1) {
                LOG_ERROR("Fuel gauge: read CW201X_REG_SOC error!");
                return static_cast<uint8_t>(CW201xRetCode::Error);
            }
            else if (RegVal <= MAGIC_VALUE_DEFINING_MAXIMUM_BATTERY_CAPACITY_IN_PERCENT)
                break;
        }

        if (i >= MAGIC_LOOP_VALUE_DEFINED_BY_CHIP_VENDOR) {
            RegVal                      = static_cast<uint8_t>(MODE::Sleep);
            fuelGaugeAddress.subAddress = static_cast<std::uint32_t>(Registers::MODE);
            ret                         = i2c->Write(fuelGaugeAddress, &RegVal, sizeof(uint8_t));
            if (ret != 1) {
                LOG_ERROR("Fuel gauge: write CW201X_REG_MODE error!");
            }
            return static_cast<uint8_t>(CW201xRetCode::Error);
        }
        return static_cast<uint8_t>(CW201xRetCode::Success);
    }
    std::int8_t CW2015::update_chip_config()
    {
        uint8_t ResetVal = 0;
        uint8_t RegVal   = 0;

        /* make sure no in sleep mode */
        fuelGaugeAddress.subAddress = static_cast<std::uint32_t>(Registers::MODE);
        auto ret                    = i2c->Read(fuelGaugeAddress, &RegVal, sizeof(uint8_t));
        if (ret != 1) {
            LOG_ERROR("Failed to read charger mode");
            return static_cast<uint8_t>(CW201xRetCode::Error);
        }
        if ((RegVal & static_cast<uint8_t>(MODE::Sleep_mask)) == static_cast<uint8_t>(MODE::Sleep)) {
            LOG_ERROR("Charger in sleep mode during init");
            return static_cast<uint8_t>(CW201xRetCode::Error);
        }
        /* update new battery info */
        for (int i = 0; i < BATTERY_INFO_SIZE; i++) {
            RegVal                      = battery_info_config_info[i];
            fuelGaugeAddress.subAddress = static_cast<std::uint32_t>(Registers::BATINFO) + i;
            ret                         = i2c->Write(fuelGaugeAddress, &RegVal, sizeof(uint8_t));
            if (ret != 1) {
                LOG_ERROR("Failed to upload battery info");
                return static_cast<uint8_t>(CW201xRetCode::Error);
            }
        }

        /* readback & check */
        for (int i = 0; i < BATTERY_INFO_SIZE; i++) {
            fuelGaugeAddress.subAddress = static_cast<std::uint32_t>(Registers::BATINFO) + i;
            ret                         = i2c->Read(fuelGaugeAddress, &RegVal, sizeof(uint8_t));
            if (ret != 1) {
                LOG_ERROR("Failed to read battery info");
                return static_cast<uint8_t>(CW201xRetCode::Error);
            }
            if (RegVal != battery_info_config_info[i]) {
                LOG_ERROR("Battery info invalid");
                return static_cast<uint8_t>(CW201xRetCode::Error);
            }
        }
        /* set cw2015/cw2013 to use new battery info */
        fuelGaugeAddress.subAddress = static_cast<std::uint32_t>(static_cast<std::uint32_t>(Registers::CONFIG));
        ret                         = i2c->Read(fuelGaugeAddress, &RegVal, sizeof(uint8_t));
        if (ret != 1) {
            LOG_ERROR("Failed to switch to custom battery info");
            return static_cast<uint8_t>(CW201xRetCode::Error);
        }

        RegVal |= static_cast<uint8_t>(CONFIG::UFG);     /* set UPDATE_FLAG */
        RegVal &= ~(static_cast<uint8_t>(CONFIG::mask)); /* clear ATHD */
        RegVal |= CW201X_ATHD;                           /* set ATHD */
        fuelGaugeAddress.subAddress = static_cast<std::uint32_t>(Registers::CONFIG);
        ret                         = i2c->Write(fuelGaugeAddress, &RegVal, sizeof(uint8_t));
        if (ret != 1) {
            LOG_ERROR("Failed to update ALARM treshold value");
            return static_cast<uint8_t>(CW201xRetCode::Error);
        }

        /* reset */
        ResetVal                    = static_cast<uint8_t>(MODE::NORMAL);
        fuelGaugeAddress.subAddress = static_cast<std::uint32_t>(Registers::MODE);
        ret                         = i2c->Write(fuelGaugeAddress, &ResetVal, sizeof(uint8_t));
        if (ret != 1) {
            LOG_ERROR("Failed to set charger to NORMAL mode");
            return static_cast<uint8_t>(CW201xRetCode::Error);
        }

        vTaskDelay(1); // delay 100us. 100us is desired value but minimum resolution for delay here is 1ms

        return static_cast<uint8_t>(CW201xRetCode::Success);
    }
    std::int8_t CW2015::reset_chip()
    {
        uint8_t ResetVal = static_cast<uint8_t>(MODE::Sleep);

        fuelGaugeAddress.subAddress = static_cast<std::uint32_t>(Registers::MODE);
        auto ret                    = i2c->Write(fuelGaugeAddress, &ResetVal, sizeof(uint8_t));
        if (ret != 1)
            return static_cast<uint8_t>(CW201xRetCode::Error);
        vTaskDelay(1); // delay 100us. 100us is desired value but minimum resolution for delay here is 1ms

        ResetVal                    = static_cast<uint8_t>(MODE::NORMAL);
        fuelGaugeAddress.subAddress = static_cast<std::uint32_t>(Registers::MODE);
        ret                         = i2c->Write(fuelGaugeAddress, &ResetVal, sizeof(uint8_t));
        if (ret != 1) {
            return static_cast<uint8_t>(CW201xRetCode::Error);
        }
        vTaskDelay(1); // delay 100us. 100us is desired value but minimum resolution for delay here is 1ms

        return init_chip();
    }
    void CW2015::irq_handler()
    {
        clear_irq();
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
    std::int8_t CW2015::quick_start()
    {
        drivers::I2CAddress address = {
            FUEL_GAUGE_I2C_ADDR, static_cast<std::uint32_t>(Registers::MODE), i2cSubaddresSize};
        uint8_t mode = static_cast<uint8_t>(MODE::QSTRT);
        auto result  = i2c->Write(fuelGaugeAddress, &mode, sizeof(uint8_t));
        if (result != 1) {
            return result;
        }
        vTaskDelay(pdMS_TO_TICKS(1));
        mode = static_cast<uint8_t>(MODE::NORMAL);
        return i2c->Write(fuelGaugeAddress, &mode, sizeof(uint8_t));
    }
    std::int8_t CW2015::poll() const
    {
        return state;
    }
} // namespace bsp::devices::power
