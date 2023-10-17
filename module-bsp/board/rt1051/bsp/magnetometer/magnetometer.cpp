// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "bsp/magnetometer/magnetometer.hpp"

#include <Utils.hpp> // for byte conversion functions. it is included first because of magic enum define

#include "ALS31300.hpp"
#include "board/BoardDefinitions.hpp"

#include "drivers/i2c/DriverI2C.hpp"

#include <fsl_common.h>
#include <timers.h>

using namespace drivers;

namespace bsp::magnetometer
{
    namespace
    {
        enum class LPDCM_INACTIVE_TIME
        {
            inactive_500us,
            inactive_1ms,
            inactive_5ms,
            inactive_10ms,
            inactive_50ms,
            inactive_100ms,
            inactive_500ms,
            inactive_1s
        };

        enum class BANDWIDTH_SELECT
        {
            bandwidth_3500Hz = 0,
            bandwidth_7kHz   = 1,
            bandwidth_14kHz  = 2,
            bandwidth_10kHz  = 4,
            bandwidth_20kHz  = 5,
            bandwidth_40kHz  = 6
        };

        union i2c_buf_t
        {
            std::uint8_t buf[sizeof(als31300::whole_reg_t)];
            als31300::whole_reg_t whole_reg;
        };

        constexpr std::uint16_t magnetometerPollIntervalMs = 500;

        I2CAddress i2cAddress = {.deviceAddress = als31300::I2C_ADDRESS, .subAddress = 0, .subAddressSize = 1};
        std::shared_ptr<drivers::DriverI2C> i2c;
        i2c_buf_t i2cBuffer;

        xQueueHandle qHandleIrq = nullptr;
        TimerHandle_t pollingTimer;

        bsp::KeyCodes currentParsedKey   = bsp::KeyCodes::Undefined;
        bsp::KeyCodes lastSliderPosition = bsp::KeyCodes::Undefined;
        Measurements lastMeasurement{};

        bool isTimeToCompleteWriteDefinedForRegistry(std::uint8_t address)
        {
            const auto it = std::find(als31300::EEPROM_REGS.begin(), als31300::EEPROM_REGS.end(), address);
            return it != als31300::EEPROM_REGS.end();
        }

        void timerHandler([[maybe_unused]] TimerHandle_t xTimer)
        {
            if (qHandleIrq == nullptr) {
                return;
            }

            std::uint8_t val = 0x01;
            xQueueSend(qHandleIrq, &val, 0);
        }

        // Magnetometer talks big endian
        als31300::whole_reg_t correctRegisterEndianness(const als31300::whole_reg_t &wholeReg)
        {
#if __BYTE_ORDER__ != __ORDER_BIG_ENDIAN__
            return utils::swapBytes(wholeReg);
#else
            return wholeReg;
#endif
        }

        bool i2cRead(std::uint8_t regAddr, als31300::whole_reg_t &wholeReg)
        {
            if (i2c == nullptr) {
                return false;
            }

            i2cAddress.subAddress = regAddr;
            if (i2c->Read(i2cAddress, i2cBuffer.buf, sizeof(als31300::whole_reg_t)) != sizeof(als31300::whole_reg_t)) {
                return false;
            }

            wholeReg = correctRegisterEndianness(i2cBuffer.whole_reg);
            return true;
        }

        bool i2cWrite(std::uint8_t regAddr, const als31300::whole_reg_t wholeReg)
        {
            if (i2c == nullptr) {
                return false;
            }

            i2cAddress.subAddress = regAddr;
            i2cBuffer.whole_reg   = correctRegisterEndianness(wholeReg);

            const auto writeStatus =
                i2c->Write(i2cAddress, i2cBuffer.buf, sizeof(als31300::whole_reg_t)) == sizeof(als31300::whole_reg_t);
            if (isTimeToCompleteWriteDefinedForRegistry(regAddr)) {
                vTaskDelay(pdMS_TO_TICKS(als31300::EEPROM_REG_WRITE_DELAY_MS.count()));
            }
            return writeStatus;
        }

        bool setActive(als31300::PWR_REG_SLEEP_MODE sleepMode)
        {
            // POWER register
            als31300::whole_reg_t read_reg;

            if (!i2cRead(als31300::PWR_REG, read_reg)) {
                return false;
            }
            als31300::pwr_reg reg_pwr = read_reg;
            reg_pwr.sleep             = sleepMode;

            if (!i2cWrite(als31300::PWR_REG, reg_pwr)) {
                return false;
            }
            if (sleepMode == als31300::PWR_REG_SLEEP_MODE::active ||
                sleepMode == als31300::PWR_REG_SLEEP_MODE::periodic_active) {
                vTaskDelay(pdMS_TO_TICKS(als31300::PWR_ON_DELAY_MS)); // give it some time to wake up
            }
            return true;
        }

        bool sliderChangedPosition(const Measurements &currentMeasurement)
        {
            /// The magnetometer is quite noisy. In some cases, noise can switch slider mode.
            /// So we did a few measurements to calculate the level of noises and it should work fine.
            constexpr auto maxNoiseLevel = 45;

            /// We don't check Z axis, because it isn't logic dependent
            const auto positionChanged = ((std::abs(currentMeasurement.X - lastMeasurement.X) > maxNoiseLevel) ||
                                          (std::abs(currentMeasurement.Y - lastMeasurement.Y) > maxNoiseLevel));
            lastMeasurement            = currentMeasurement;

            return positionChanged;
        }
    } // namespace

    std::int32_t init(xQueueHandle qHandle)
    {
        i2c = DriverI2C::Create(
            static_cast<I2CInstances>(BoardDefinitions::MAGNETOMETER_I2C),
            DriverI2CParams{.baudrate = static_cast<std::uint32_t>(BoardDefinitions::MAGNETOMETER_I2C_BAUDRATE)});

        qHandleIrq = qHandle;

        // Any configuration must be proceeded in active state
        setActive(als31300::PWR_REG_SLEEP_MODE::active);

        // Get write access
        if (!i2cWrite(als31300::CUSTOMER_ACCESS_REG, als31300::CUSTOMER_ACCESS_REG_code)) {
            LOG_ERROR("Failed to init magnetometer!");
            return kStatus_Fail;
        }

        als31300::whole_reg_t readRegister{};

        // CONFIGURATION register read
        auto readStatus                              = i2cRead(als31300::CONF_REG, readRegister);
        const als31300::conf_reg currentConfRegister = readRegister;
        if (!readStatus) {
            LOG_ERROR("Failed to read CONF register!");
        }
        else {
            LOG_DEBUG("Current CONF register value: %" PRIu32, static_cast<std::uint32_t>(currentConfRegister));
        }

        als31300::conf_reg updatedConfRegister = currentConfRegister;
        updatedConfRegister.I2C_threshold      = als31300::CONF_REG_I2C_THRES_1v8;
        updatedConfRegister.int_latch_enable = als31300::CONF_REG_LATCH_disabled; // we want to detect stable positions
        updatedConfRegister.channel_X_en     = als31300::CONF_REG_CHANNEL_enabled;
        updatedConfRegister.channel_Y_en     = als31300::CONF_REG_CHANNEL_enabled;
        updatedConfRegister.channel_Z_en     = als31300::CONF_REG_CHANNEL_disabled;
        updatedConfRegister.bandwidth        = static_cast<std::uint8_t>(BANDWIDTH_SELECT::bandwidth_7kHz);

        if (currentConfRegister == updatedConfRegister) {
            LOG_DEBUG("CONF register unchanged, skipping write");
        }
        else {
            if (!i2cWrite(als31300::CONF_REG, updatedConfRegister)) {
                LOG_ERROR("Failed to write to CONF register!");
                return kStatus_Fail;
            }
            LOG_DEBUG("CONF register value written: %" PRIu32, static_cast<std::uint32_t>(updatedConfRegister));
        }

        // INTERRUPTS register
        readStatus                                 = i2cRead(als31300::INT_REG, readRegister);
        const als31300::int_reg currentIntRegister = readRegister;
        if (!readStatus) {
            LOG_ERROR("Failed to read INT register!");
        }
        else {
            LOG_DEBUG("Current INT register value: %" PRIu32, static_cast<std::uint32_t>(currentIntRegister));
        }

        als31300::int_reg updatedIntRegister    = currentIntRegister;
        updatedIntRegister.int_eeprom_en        = als31300::INT_REG_INT_EEPROM_disable;
        updatedIntRegister.int_mode             = als31300::INT_REG_INT_MODE_threshold;
        updatedIntRegister.int_threshold_signed = als31300::INT_REG_THRESHOLD_absolute;
        updatedIntRegister.int_X_en             = als31300::INT_REG_INT_CHANNEL_disabled;
        updatedIntRegister.int_Y_en             = als31300::INT_REG_INT_CHANNEL_disabled;
        updatedIntRegister.int_Z_en             = als31300::INT_REG_INT_CHANNEL_disabled;
        updatedIntRegister.int_X_threshold      = 1;
        updatedIntRegister.int_Y_threshold      = 4;
        updatedIntRegister.int_Z_threshold      = 0;

        if (currentIntRegister == updatedIntRegister) {
            LOG_DEBUG("INT register unchanged, skipping write");
        }
        else {
            if (!i2cWrite(als31300::INT_REG, updatedIntRegister)) {
                LOG_ERROR("Failed to write to INT register!");
                return kStatus_Fail;
            }
            LOG_DEBUG("INT register value written: %" PRIu32, static_cast<std::uint32_t>(updatedIntRegister));
        }

        if (pollingTimer == nullptr) {
            pollingTimer = xTimerCreate(
                "MagnetometerPollingTimer", pdMS_TO_TICKS(magnetometerPollIntervalMs), pdTRUE, nullptr, timerHandler);
            if (pollingTimer == nullptr) {
                LOG_ERROR("Failed to create magnetometer polling timer!");
                return kStatus_Fail;
            }
        }

        // POWER register
        readStatus                    = i2cRead(als31300::PWR_REG, readRegister);
        als31300::pwr_reg pwrRegister = readRegister;
        if (!readStatus) {
            LOG_ERROR("Failed to read POWER register!");
        }
        else {
            LOG_DEBUG("Current POWER register value: %" PRIu32, static_cast<std::uint32_t>(pwrRegister));
        }

        pwrRegister.I2C_loop_mode     = als31300::PWR_REG_LOOP_MODE_single; // We don't want constant data flow
        pwrRegister.sleep             = als31300::PWR_REG_SLEEP_MODE_active;
        pwrRegister.count_max_LP_mode = static_cast<std::uint8_t>(LPDCM_INACTIVE_TIME::inactive_10ms);

        if (!i2cWrite(als31300::PWR_REG, pwrRegister)) {
            LOG_ERROR("Failed to write to POWER register!");
            return kStatus_Fail;
        }
        LOG_DEBUG("POWER register value written: %" PRIu32, static_cast<std::uint32_t>(pwrRegister));

        xTimerStart(pollingTimer, 1000);
        return kStatus_Success;
    }

    void deinit()
    {
        if (pollingTimer != nullptr) {
            xTimerStop(pollingTimer, 0);
        }

        i2c.reset();
    }

    bool isPresent()
    {
        std::uint8_t dummy;
        i2cAddress.subAddress = 0x00;

        if (i2c == nullptr) {
            return false;
        }
        if (i2c->Read(i2cAddress, &dummy, sizeof(dummy)) != sizeof(dummy)) {
            return false;
        }
        return true;
    }

    std::optional<Measurements> getMeasurement()
    {
        als31300::whole_reg_t readRegister;

        if (!i2cRead(als31300::MEASUREMENTS_MSB_REG, readRegister)) {
            LOG_ERROR("Failed to read measurements MSB register!");
            return std::nullopt;
        }
        const als31300::measurements_MSB_reg measurementsMsb = readRegister;

        if (measurementsMsb.int_flag) {
            LOG_DEBUG("INT flag in register is set");
        }

        if (measurementsMsb.new_data_flag != als31300::MEAS_REG_NEW_DATA_available) {
            return std::nullopt;
        }

        if (measurementsMsb.int_flag) {
            if (!i2cWrite(als31300::MEASUREMENTS_MSB_REG, measurementsMsb)) {
                LOG_ERROR("Failed to clear INT flag in register");
                return std::nullopt;
            }
        }

        if (!i2cRead(als31300::MEASUREMENTS_LSB_REG, readRegister)) {
            LOG_ERROR("Failed to read measurements LSB register!");
            return std::nullopt;
        }
        const als31300::measurements_LSB_reg measurementsLsb = readRegister;

        const Measurements measurement{
            .X = als31300::measurement_sign_convert(measurementsMsb.X_MSB << 4 | measurementsLsb.X_LSB),
            .Y = als31300::measurement_sign_convert(measurementsMsb.Y_MSB << 4 | measurementsLsb.Y_LSB),
            .Z = als31300::measurement_sign_convert(measurementsMsb.Z_MSB << 4 | measurementsLsb.Z_LSB)};
        return measurement;
    }

    bsp::KeyCodes parse(const Measurements &measurements)
    {
        // X is tri-stable
        constexpr auto X_lower_threshold = -85;
        constexpr auto X_upper_threshold = 80;
        // Y is bi-stable
        constexpr auto Y_threshold = -200;
        // Y is used only for proofing X, so no strict thresholds
        // Z is useless

        auto position = lastSliderPosition;

        if (sliderChangedPosition(measurements)) {
            if ((measurements.X < X_lower_threshold) && (measurements.Y > Y_threshold)) {
                position = bsp::KeyCodes::SSwitchDown;
            }
            else if ((measurements.X > X_upper_threshold) && (measurements.Y > Y_threshold)) {
                position = bsp::KeyCodes::SSwitchUp;
            }
            else if (measurements.Y < Y_threshold) {
                position = bsp::KeyCodes::SSwitchMid;
            }
            lastSliderPosition = position;
        }
        return position;
    }

    void resetCurrentParsedValue()
    {
        currentParsedKey = bsp::KeyCodes::Undefined;
    }

    std::optional<bsp::KeyCodes> WorkerEventHandler()
    {
        // Try to get new data from active magneto
        setActive(als31300::PWR_REG_SLEEP_MODE::active);
        const auto measurement = getMeasurement();
        setActive(als31300::PWR_REG_SLEEP_MODE::sleep);

        if (!measurement.has_value()) {
            return std::nullopt;
        }

        const auto incomingParsedKey = parse(measurement.value());
        if ((incomingParsedKey == bsp::KeyCodes::Undefined) || (incomingParsedKey == currentParsedKey)) {
            return std::nullopt;
        }

        currentParsedKey = incomingParsedKey;
        return currentParsedKey;
    }
} // namespace bsp::magnetometer
