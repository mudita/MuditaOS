// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "bsp/magnetometer/magnetometer.hpp"

#include <Utils.hpp> // for byte conversion functions. it is included first because of magic enum define

#include "ALS31300.hpp"
#include "board/BoardDefinitions.hpp"

#include "drivers/i2c/DriverI2C.hpp"

#include <fsl_common.h>
#include <timers.h>

using namespace drivers;
using namespace utils;

static std::shared_ptr<drivers::DriverI2C> i2c;

static I2CAddress addr = {.deviceAddress = als31300::I2C_ADDRESS, .subAddress = 0, .subAddressSize = 1};

union i2c_buf_t
{
    uint8_t buf[sizeof(als31300::whole_reg_t)];
    als31300::whole_reg_t whole_reg;
};

static i2c_buf_t i2c_buf;

static xQueueHandle qHandleIrq = NULL;

namespace bsp
{
    namespace magnetometer
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
        namespace
        {
            bool isTimeToCompleteWriteDefinedForRegistry(std::uint8_t address)
            {
                const auto it = std::find(als31300::EEPROM_REGS.begin(), als31300::EEPROM_REGS.end(), address);
                return it != als31300::EEPROM_REGS.end();
            }
        } // namespace

        std::shared_ptr<DriverGPIO> gpio;

        bsp::KeyCodes current_parsed = bsp::KeyCodes::Undefined;

        static TimerHandle_t timerHandle;
        static constexpr uint16_t MAGNETOMETER_POLL_INTERVAL_MS = 500;

        static void TimerHandler(TimerHandle_t xTimer)
        {
            if (qHandleIrq != nullptr) {
                uint8_t val = 0x01;
                xQueueSend(qHandleIrq, &val, 0);
            }
        }

        bool setActive(als31300::PWR_REG_SLEEP_MODE sleep_mode);

        bool i2cRead(const uint8_t reg_addr, als31300::whole_reg_t &whole_reg)
        {
            addr.subAddress = reg_addr;
            if (i2c->Read(addr, i2c_buf.buf, sizeof(als31300::whole_reg_t)) != sizeof(als31300::whole_reg_t)) {
                return false;
            }
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
            // magnetometr talks big endian
            i2c_buf.whole_reg = swapBytes(i2c_buf.whole_reg);
#endif
            whole_reg = i2c_buf.whole_reg;
            return true;
        }

        bool i2cWrite(const uint8_t reg_addr, const als31300::whole_reg_t whole_reg)
        {
            addr.subAddress = reg_addr;
#if __BYTE_ORDER__ != __ORDER_BIG_ENDIAN__
            // magnetometer talks big endian
            i2c_buf.whole_reg = swapBytes(whole_reg);
#else
            i2c_buf.whole_reg = whole_reg;
#endif
            auto wrote = i2c->Write(addr, i2c_buf.buf, sizeof(als31300::whole_reg_t)) == sizeof(als31300::whole_reg_t);
            if (isTimeToCompleteWriteDefinedForRegistry(reg_addr)) {
                vTaskDelay(pdMS_TO_TICKS(als31300::EEPROM_REG_WRITE_DELAY_MS.count()));
            }
            return wrote;
        }

        int32_t init(xQueueHandle qHandle)
        {
            i2c = DriverI2C::Create(
                static_cast<I2CInstances>(BoardDefinitions::MAGNETOMETER_I2C),
                DriverI2CParams{.baudrate = static_cast<uint32_t>(BoardDefinitions::MAGNETOMETER_I2C_BAUDRATE)});

            qHandleIrq = qHandle;

            // any configuration must be proceeded in active state
            setActive(als31300::PWR_REG_SLEEP_MODE::active);

            // GET WRITE ACCESS
            if (!i2cWrite(als31300::CUSTOMER_ACCESS_REG, als31300::CUSTOMER_ACCESS_REG_code)) {
                LOG_ERROR("magneto: CANNOT INIT SLIDER SENSOR");
                return kStatus_Fail;
            }

            // CONFIGURATION register read
            als31300::whole_reg_t read_reg;
            i2cRead(als31300::CONF_REG, read_reg);
            const als31300::conf_reg current_reg_conf(read_reg);
            LOG_DEBUG("CONF read:\t%" PRIu32, static_cast<uint32_t>(current_reg_conf));
            als31300::conf_reg reg_conf = current_reg_conf;
            reg_conf.I2C_threshold      = als31300::CONF_REG_I2C_THRES_1v8;
            reg_conf.int_latch_enable   = als31300::CONF_REG_LATCH_disabled; // we want to detect stable positions
            reg_conf.channel_X_en       = als31300::CONF_REG_CHANNEL_enabled;
            reg_conf.channel_Y_en       = als31300::CONF_REG_CHANNEL_enabled;
            reg_conf.channel_Z_en       = als31300::CONF_REG_CHANNEL_disabled;
            reg_conf.bandwidth          = static_cast<uint8_t>(BANDWIDTH_SELECT::bandwidth_7kHz);
            if (current_reg_conf != reg_conf) {
                [[maybe_unused]] auto ret = i2cWrite(als31300::CONF_REG, reg_conf);
                assert(ret);
                LOG_DEBUG("CONF wrote:\t%" PRIu32, static_cast<uint32_t>(reg_conf));

                i2cRead(als31300::CONF_REG, read_reg);
                LOG_DEBUG("CONF verify:\t%" PRIu32, static_cast<uint32_t>(als31300::conf_reg(read_reg)));
            }
            else {
                LOG_DEBUG("CONF is fine, sparing a write");
            }

            // INTERRUPTS register
            i2cRead(als31300::INT_REG, read_reg);
            const als31300::int_reg current_reg_int = read_reg;
            LOG_DEBUG("INT read:\t%" PRIu32, static_cast<uint32_t>(current_reg_int));
            als31300::int_reg reg_int    = current_reg_int;
            reg_int.int_eeprom_en        = als31300::INT_REG_INT_EEPROM_disable;
            reg_int.int_mode             = als31300::INT_REG_INT_MODE_threshold;
            reg_int.int_threshold_signed = als31300::INT_REG_THRESHOLD_absolute;
            reg_int.int_X_en             = als31300::INT_REG_INT_CHANNEL_disabled;
            reg_int.int_Y_en             = als31300::INT_REG_INT_CHANNEL_disabled;
            reg_int.int_Z_en             = als31300::INT_REG_INT_CHANNEL_disabled;
            reg_int.int_X_threshold      = 1;
            reg_int.int_Y_threshold      = 4;
            reg_int.int_Z_threshold      = 0;
            if (current_reg_int != reg_int) {
                [[maybe_unused]] auto ret = i2cWrite(als31300::INT_REG, reg_int);
                assert(ret);
                LOG_DEBUG("INT wrote:\t%" PRIu32, static_cast<uint32_t>(reg_int));

                i2cRead(als31300::INT_REG, read_reg);
                LOG_DEBUG("INT verify:\t%" PRIu32, static_cast<uint32_t>(als31300::int_reg(read_reg)));
            }
            else {
                LOG_DEBUG("INT is fine, sparing a write");
            }

            // INTERRUPT PIN
            gpio =
                DriverGPIO::Create(static_cast<GPIOInstances>(BoardDefinitions::MAGNETOMETER_GPIO), DriverGPIOParams{});

            // INTERRUPT PIN
            gpio->ClearPortInterrupts(1 << static_cast<uint32_t>(BoardDefinitions::MAGNETOMETER_IRQ));
            gpio->ConfPin(DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Input,
                                              .irqMode  = DriverGPIOPinParams::InterruptMode::IntFallingEdge,
                                              .defLogic = 0,
                                              .pin      = static_cast<uint32_t>(BoardDefinitions::MAGNETOMETER_IRQ)});
            // NOTE: irq not yet enabled
            // this version uses timer to poll the sensor
            // the timer requests to read the magnetometer periodically
            if (timerHandle == nullptr) {
                timerHandle = xTimerCreate(
                    "SliderTimer", pdMS_TO_TICKS(MAGNETOMETER_POLL_INTERVAL_MS), true, nullptr, TimerHandler);
                if (timerHandle == nullptr) {
                    LOG_ERROR("Could not create the timer for magnetometer state change detection!");
                    return kStatus_Fail;
                }
            }

            // POWER register
            i2cRead(als31300::PWR_REG, read_reg);
            const als31300::pwr_reg current_reg_pwr = read_reg;
            LOG_DEBUG("POWER read:\t%" PRIu32, static_cast<uint32_t>(current_reg_pwr));
            als31300::pwr_reg reg_pwr = current_reg_pwr;
            reg_pwr.I2C_loop_mode     = als31300::PWR_REG_LOOP_MODE_single; // we don't want constant data flow
            reg_pwr.sleep             = als31300::PWR_REG_SLEEP_MODE_active;
            reg_pwr.count_max_LP_mode = static_cast<uint8_t>(LPDCM_INACTIVE_TIME::inactive_10ms);

            i2cWrite(als31300::PWR_REG, reg_pwr);
            LOG_DEBUG("POWER wrote:\t%" PRIu32, static_cast<uint32_t>(reg_pwr));

            xTimerStart(timerHandle, 1000);

            return kStatus_Success;
        }

        void deinit()
        {
            gpio->DisableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::MAGNETOMETER_IRQ));
            if (timerHandle != nullptr) {
                xTimerStop(timerHandle, 0);
            }
        }

        std::pair<bool, Measurements> getMeasurement()
        {
            als31300::whole_reg_t read_reg;

            if (!i2cRead(als31300::MEASUREMENTS_MSB_REG, read_reg)) {
                LOG_DEBUG("magneto: CANNOT READ");
                return std::make_pair(false, Measurements()); // todo: nullopt
            }
            // is there anything new ?
            als31300::measurements_MSB_reg reg_msb = read_reg;

            if (reg_msb.int_flag == true) {
                LOG_DEBUG("magneto: INT flag in register");
            }

            if (reg_msb.new_data_flag != als31300::MEAS_REG_NEW_DATA_available) {
                return std::make_pair(false, Measurements());
            }
            else {
                if (reg_msb.int_flag == true) {
                    // clear INT flag
                    if (!i2cWrite(als31300::MEASUREMENTS_MSB_REG, reg_msb)) {
                        return std::make_pair(false, Measurements()); // todo: null opt
                    }
                }
                Measurements meas;

                i2cRead(als31300::MEASUREMENTS_LSB_REG, read_reg);

                als31300::measurements_LSB_reg reg_lsb = read_reg;

                meas.X = als31300::measurement_sign_convert(reg_msb.X_MSB << 4 | reg_lsb.X_LSB);
                meas.Y = als31300::measurement_sign_convert(reg_msb.Y_MSB << 4 | reg_lsb.Z_LSB);
                meas.Z = als31300::measurement_sign_convert(reg_msb.Z_MSB << 4 | reg_lsb.Z_LSB);

                return std::pair(true, meas);
            }
        }

        bool setActive(als31300::PWR_REG_SLEEP_MODE sleep_mode)
        {
            // POWER register
            als31300::whole_reg_t read_reg;

            if (!i2cRead(als31300::PWR_REG, read_reg)) {
                return false;
            }
            als31300::pwr_reg reg_pwr = read_reg;
            reg_pwr.sleep             = sleep_mode;

            if (!i2cWrite(als31300::PWR_REG, reg_pwr)) {
                return false;
            }
            if (sleep_mode == als31300::PWR_REG_SLEEP_MODE::active ||
                sleep_mode == als31300::PWR_REG_SLEEP_MODE::periodic_active) {
                vTaskDelay(pdMS_TO_TICKS(als31300::PWR_ON_DELAY_MS)); // give it some time to wake up
            }
            return true;
        }

        bool isPresent(void)
        {
            uint8_t buf;
            addr.subAddress = 0x00;
            auto read       = i2c->Read(addr, &buf, 1);

            return read == 1;
        }

        bsp::KeyCodes parse(const Measurements &measurements)
        {
            // X is tri-stable
            const auto X_lower_boundary  = -150;
            const auto X_upper_boundary  = 150;
            const auto X_lower_threshold = -65;
            const auto X_upper_threshold = 60;
            // Y is bi-stable
            const auto Y_threshold = -175;
            // Y is used only for proofing X, so no strict thresholds
            // Z is useless

            if (measurements.X > X_lower_boundary && measurements.X < X_upper_boundary) {
                if (measurements.X < X_lower_threshold) {
                    if (measurements.Y > Y_threshold) {
                        return bsp::KeyCodes::SSwitchDown;
                    }
                }
                else if (measurements.X > X_upper_threshold) {
                    if (measurements.Y > Y_threshold) {
                        return bsp::KeyCodes::SSwitchUp;
                    }
                }
                else {
                    if (measurements.Y < Y_threshold) {
                        return bsp::KeyCodes::SSwitchMid;
                    }
                }
            }
            return bsp::KeyCodes::Undefined;
        }
        void resetCurrentParsedValue()
        {
            current_parsed = bsp::KeyCodes::Undefined;
        }

        std::optional<bsp::KeyCodes> WorkerEventHandler()
        {
            // try to get new data from active magneto
            setActive(als31300::PWR_REG_SLEEP_MODE::active);
            auto [new_data, measurement] = getMeasurement();
            setActive(als31300::PWR_REG_SLEEP_MODE::sleep);
            if (new_data) {
                auto incoming_parsed = parse(measurement);
                if (incoming_parsed != bsp::KeyCodes::Undefined and incoming_parsed != current_parsed) {
                    current_parsed = incoming_parsed;
                    return current_parsed;
                }
            }
            return std::nullopt;
        }

        BaseType_t IRQHandler()
        {
            gpio->DisableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::MAGNETOMETER_IRQ));
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            if (qHandleIrq != NULL) {
                uint8_t val = 0x01;
                xQueueSendFromISR(qHandleIrq, &val, &xHigherPriorityTaskWoken);
            }
            return xHigherPriorityTaskWoken;
        }

        void enableIRQ()
        {
            gpio->EnableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::MAGNETOMETER_IRQ));
        }
    } // namespace magnetometer
} // namespace bsp
