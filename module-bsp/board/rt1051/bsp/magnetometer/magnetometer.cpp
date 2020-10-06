// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "bsp/magnetometer/magnetometer.hpp"

#include "ALS31300.hpp"
#include "bsp/BoardDefinitions.hpp"
#include "bsp/magnetometer/magnetometer.hpp"

#include "drivers/i2c/DriverI2C.hpp"

#include <module-utils/Utils.hpp> // for byte conversion funcions

#include <fsl_common.h>
#include <timers.h>

using namespace drivers;
using namespace utils;

static std::shared_ptr<drivers::DriverI2C> i2c;

static I2CAddress addr = {.deviceAddress = ALS31300_I2C_ADDRESS, .subAddressSize = 1};

static xQueueHandle qHandleIrq = NULL;

namespace bsp
{
    namespace magnetometer
    {
        std::shared_ptr<DriverGPIO> gpio;

        bool waitForWrite(uint32_t subAddress);

        int32_t init(xQueueHandle qHandle)
        {
            i2c = DriverI2C::Create(
                static_cast<I2CInstances>(BoardDefinitions::MAGNETOMETER_I2C),
                DriverI2CParams{.baudrate = static_cast<uint32_t>(BoardDefinitions::MAGNETOMETER_I2C_BAUDRATE)});

            qHandleIrq = qHandle;

            // init device
            uint8_t buf[4];

            // GET WRITE ACCESS
            addr.subAddress = ALS31300_CUSTOMER_ACCESS_REG;
            toBigEndian(ALS31300_CUSTOMER_ACCESS_REG_code, buf);
            assert(i2c->Write(addr, buf, 4) == 4);
            waitForWrite(addr.subAddress);

            // CONFIGURATION register read
            addr.subAddress = ALS31300_CONF_REG;

            i2c->Read(addr, buf, 4);
            const als31300_conf_reg current_reg_conf(fromBigEndian(buf));
            LOG_DEBUG("CONF read 1:\t%" PRIu32, static_cast<uint32_t>(current_reg_conf));
            als31300_conf_reg reg_conf = current_reg_conf;
            reg_conf.I2C_threshold     = ALS31300_CONF_REG_I2C_THRES_1V8;
            reg_conf.int_latch_enable  = ALS31300_CONF_REG_LATCH_DISABLED; // we want to detect stable positions
            reg_conf.channel_X_en      = ALS31300_CONF_REG_CHANNEL_ENABLED;
            reg_conf.channel_Y_en      = ALS31300_CONF_REG_CHANNEL_ENABLED;
            reg_conf.channel_Z_en      = ALS31300_CONF_REG_CHANNEL_DISABLED;
            reg_conf.bandwidth         = 1; // longest unit measurement
            toBigEndian(reg_conf, buf);
            if (current_reg_conf != reg_conf) {
                assert(i2c->Write(addr, buf, 4) == 4);
                waitForWrite(addr.subAddress);
                LOG_DEBUG("CONF wrote:\t%" PRIu32, static_cast<uint32_t>(reg_conf));
            }
            i2c->Read(addr, buf, 4);
            auto reg_conf_2 = fromBigEndian(buf);
            LOG_DEBUG("CONF read 2:\t%" PRIu32, static_cast<uint32_t>(reg_conf_2));

            // INTERRUPTS register
            addr.subAddress = ALS31300_INT_REG;

            i2c->Read(addr, buf, 4);
            const als31300_int_reg current_reg_int = fromBigEndian(buf);
            LOG_DEBUG("INT read 1:\t%" PRIu32, static_cast<uint32_t>(current_reg_int));
            als31300_int_reg reg_int     = current_reg_int;
            reg_int.int_mode             = ALS31300_INT_REG_INT_MODE_threshold;
            reg_int.int_threshold_signed = ALS31300_INT_REG_THRESHOLD_absolute;
            reg_int.int_X_en             = ALS31300_INT_REG_INT_CHANNEL_enabled;
            reg_int.int_Y_en             = ALS31300_INT_REG_INT_CHANNEL_disabled;
            reg_int.int_Z_en             = ALS31300_INT_REG_INT_CHANNEL_disabled;
            reg_int.int_X_threshold      = 1;
            reg_int.int_Y_threshold      = 4;
            reg_int.int_Z_threshold      = 0;
            toBigEndian(reg_int, buf);
            if (current_reg_int != reg_int) {
                i2c->Write(addr, buf, 4);
                waitForWrite(addr.subAddress);
                LOG_DEBUG("INT wrote:\t%" PRIu32, static_cast<uint32_t>(reg_int));
            }
            i2c->Read(addr, buf, 4);
            als31300_int_reg reg_int_2 = fromBigEndian(buf);
            LOG_DEBUG("INT read 2:\t%" PRIu32, static_cast<uint32_t>(reg_int_2));

            // INTERRUPT PIN
            gpio =
                DriverGPIO::Create(static_cast<GPIOInstances>(BoardDefinitions::MAGNETOMETER_GPIO), DriverGPIOParams{});

            // INTERRUPT PIN
            gpio->ClearPortInterrupts(1 << static_cast<uint32_t>(BoardDefinitions::MAGNETOMETER_IRQ));
            gpio->ConfPin(DriverGPIOPinParams{.dir     = DriverGPIOPinParams::Direction::Input,
                                              .irqMode = DriverGPIOPinParams::InterruptMode::IntFallingEdge,
                                              .pin     = static_cast<uint32_t>(BoardDefinitions::MAGNETOMETER_IRQ)});

            // EN IRQ
            gpio->EnableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::MAGNETOMETER_IRQ));

            // POWER register
            addr.subAddress = ALS31300_PWR_REG;

            i2c->Read(addr, buf, 4);
            const als31300_pwr_reg current_reg_pwr = fromBigEndian(buf);
            LOG_DEBUG("POWER read 1:\t%" PRIu32, static_cast<uint32_t>(current_reg_pwr));
            als31300_pwr_reg reg_pwr  = current_reg_pwr;
            reg_pwr.I2C_loop_mode     = ALS31300_PWR_REG_LOOP_MODE_single; // we don't want constant data flow
            reg_pwr.sleep             = ALS31300_PWR_REG_SLEEP_MODE_active;
            reg_pwr.count_max_LP_mode = 3U; // get an update every 500 ms
            toBigEndian(reg_pwr, buf);
            //            if (current_reg_pwr != reg_pwr) {
            i2c->Write(addr, buf, 4);
            LOG_DEBUG("POWER wrote:\t%" PRIu32, static_cast<uint32_t>(reg_pwr));

            return kStatus_Success;
        }

        bool waitForWrite(uint32_t subAddress)
        {
            //            if (ALS31300_EEPROM_REGS.count(static_cast<uint8_t>(subAddress)) == 1) {
            vTaskDelay(pdMS_TO_TICKS(50));
            LOG_DEBUG("waiting after writing reg %lx", addr.subAddress);
            //            }
            //            else {
            //                LOG_DEBUG("NOT waiting after writing reg %lx", addr.subAddress);
            //            }
            return true;
        }

        std::pair<bool, Measurements> getMeasurements()
        {
            uint8_t buf[4];

            addr.subAddress = ALS31300_MEASUREMENTS_MSB_REG;
            if (i2c->Read(addr, buf, 4) != 4) {
                LOG_DEBUG("CANNOT READ magneto");
                return std::make_pair(false, Measurements());
            }
            // is there anything new ?
            als31300_measurements_MSB_reg reg_msb =
                fromBigEndian(buf); // it arrives as big-endian, but we are little-endian

            if (reg_msb.int_flag == true) {
                LOG_DEBUG("INT flag magneto");
            }

            if (reg_msb.new_data_flag != ALS31300_MEAS_REG_NEW_DATA_available) {
                return std::make_pair(false, Measurements());
            }
            else {
                if (reg_msb.int_flag == true) {
                    // clear INT flag
                    toBigEndian(reg_msb, buf);
                    if (i2c->Write(addr, buf, 4) != 4) {
                        LOG_DEBUG("cannot clear INT flag. quitting");
                        return std::make_pair(false, Measurements());
                    }
                }
                Measurements meas;

                addr.subAddress = ALS31300_MEASUREMENTS_LSB_REG;
                i2c->Read(addr, buf, 4);

                als31300_measurements_LSB_reg reg_lsb = fromBigEndian(buf);

                meas.X     = als31300_measurement_sign_convert(reg_msb.X_MSB << 4 | reg_lsb.X_LSB);
                meas.Y     = als31300_measurement_sign_convert(reg_msb.Y_MSB << 4 | reg_lsb.Z_LSB);
                meas.Z     = als31300_measurement_sign_convert(reg_msb.Z_MSB << 4 | reg_lsb.Z_LSB);
                meas.tempC = als31300_temperature_convert((reg_msb.temperature_MSB << 6) | (reg_lsb.temperature_LSB));

                LOG_DEBUG("magneto measured: %d, %d, %d", meas.X, meas.Y, meas.Z);

                return std::pair(true, meas);
            }
        }

        bool setPowerState(const uint8_t pwr_value)
        {
            // refresh memory registers by toggling active for a moment
            // POWER register
            uint8_t buf[4];
            addr.subAddress = ALS31300_PWR_REG;

            als31300_pwr_reg reg_pwr;

            //            i2c->Read(addr, buf, 4);
            //            const als31300_pwr_reg current_reg_pwr = fromBigEndian(buf);
            //            als31300_pwr_reg reg_pwr(current_reg_pwr);
            //            LOG_DEBUG("POWER read 1:\t%" PRIu32, static_cast<uint32_t>(current_reg_pwr));
            reg_pwr.sleep             = pwr_value;
            reg_pwr.count_max_LP_mode = 3U;
            reg_pwr.I2C_loop_mode     = ALS31300_PWR_REG_LOOP_MODE_single;
            toBigEndian(reg_pwr, buf);
            //            if (current_reg_pwr != reg_pwr) {
            if(i2c->Write(addr, buf, 4) != 4){
                return false;
            }
            LOG_DEBUG("POWER wrote:\t%" PRIu32, static_cast<uint32_t>(reg_pwr));

            // INTERRUPTS register
            addr.subAddress = ALS31300_INT_REG;

            if(i2c->Read(addr, buf, 4) != 4){
                return false;
            }
            const als31300_int_reg current_reg_int = fromBigEndian(buf);
            als31300_int_reg reg_int(current_reg_int);
            switch (reg_pwr.sleep) {
            case ALS31300_PWR_REG_SLEEP_MODE_active:
                reg_int.int_X_en = ALS31300_INT_REG_INT_CHANNEL_disabled;
                break;
            case ALS31300_PWR_REG_SLEEP_MODE_LPDCM:
                reg_int.int_X_en = ALS31300_INT_REG_INT_CHANNEL_enabled;
                reg_int.int_mode = ALS31300_INT_REG_INT_MODE_delta;
                break;
            default:
                break;
            }

            if (current_reg_int != reg_int) {
                toBigEndian(reg_int, buf);
                i2c->Write(addr, buf, 4);
                waitForWrite(addr.subAddress);
                LOG_DEBUG("INT wrote:\t%" PRIu32, static_cast<uint32_t>(reg_int));
            }
            //            }
            return true;
        }

        bool isPresent(void)
        {
            uint8_t buf;
            addr.subAddress = 0x00;
            auto read       = i2c->Read(addr, &buf, 1);

            if (read != 1) {
                return false;
            }
            return true;
        }

        bsp::KeyCodes getDiscrete(const Measurements &measurements)
        {
            if (measurements.X < -150 || measurements.X > 150) {
                return bsp::KeyCodes::Undefined;
            }
            else {
                if (measurements.X < -65) {
                    return bsp::KeyCodes::SSwitchDown;
                }
                if (measurements.X > 60) {
                    return bsp::KeyCodes::SSwitchUp;
                }
                else
                    return bsp::KeyCodes::SSwitchMid;
            }
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

        bsp::Board GetBoard(void)
        {
            if (isPresent()) {
                return bsp::Board::T4;
            }
            return bsp::Board::T3;
        }

        void enableIRQ()
        {
            gpio->EnableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::MAGNETOMETER_IRQ));
        }
    } // namespace magnetometer
} // namespace bsp
