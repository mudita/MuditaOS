// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RT1051DriverI2C.hpp"
#include "log.hpp"
#include "../board.h"

namespace drivers
{

    RT1051DriverI2C::RT1051DriverI2C(const I2CInstances inst, const DriverI2CParams &params) : DriverI2C(params, inst)
    {

        lpi2c_master_config_t lpi2cConfig = {};

        switch (instance) {
        case I2CInstances ::I2C1:
            base = LPI2C1;
            LOG_DEBUG("Init: I2C1");
            break;
        case I2CInstances ::I2C2:
            base = LPI2C2;
            LOG_DEBUG("Init: I2C2");
            break;
        default:
            break;
        }
        LPI2C_MasterGetDefaultConfig(&lpi2cConfig);
        LPI2C_MasterInit(base, &lpi2cConfig, GetPerphSourceClock(PerphClock_I2C));
    }

    RT1051DriverI2C::~RT1051DriverI2C()
    {
        switch (instance) {
        case I2CInstances ::I2C1:
            LOG_DEBUG("Deinit: I2C1");
            break;
        case I2CInstances ::I2C2:
            LOG_DEBUG("Deinit: I2C2");
            break;
        default:
            break;
        }
        LPI2C_MasterDeinit(base);
    }

    ssize_t RT1051DriverI2C::Write(const drivers::I2CAddress &addr, const uint8_t *txBuff, const size_t size)
    {
        cpp_freertos::LockGuard lock(mutex);
        auto ret = BOARD_LPI2C_Send(
            base, addr.deviceAddress, addr.subAddress, addr.subAddressSize, const_cast<uint8_t *>(txBuff), size);
        if (ret != kStatus_Success) {
            return -1; // TODO:M.P: fix me
        }
        else {
            return size;
        }
    }

    ssize_t RT1051DriverI2C::Read(const drivers::I2CAddress &addr, uint8_t *rxBuff, const size_t size)
    {
        cpp_freertos::LockGuard lock(mutex);
        auto ret = BOARD_LPI2C_Receive(base, addr.deviceAddress, addr.subAddress, addr.subAddressSize, rxBuff, size);
        if (ret != kStatus_Success) {
            return -1; // TODO:M.P: fix me
        }
        else {
            return size;
        }
    }

    ssize_t RT1051DriverI2C::Modify(const drivers::I2CAddress &addr,
                                    const uint32_t mask,
                                    bool setClr,
                                    const size_t size)
    {
        cpp_freertos::LockGuard lock(mutex);
        uint16_t rx = 0;
        auto ret =
            BOARD_LPI2C_Receive(base, addr.deviceAddress, addr.subAddress, addr.subAddressSize, (uint8_t *)&rx, size);

        // Set specified bits
        if (setClr) {
            rx |= mask;
        }
        else {
            rx &= ~mask;
        }

        ret |= BOARD_LPI2C_Send(base, addr.deviceAddress, addr.subAddress, addr.subAddressSize, (uint8_t *)&rx, size);

        if (ret != kStatus_Success) {
            return -1; // TODO:M.P: fix me
        }
        else {
            return size;
        }
    }

    status_t RT1051DriverI2C::BOARD_LPI2C_Send(LPI2C_Type *base,
                                               uint8_t deviceAddress,
                                               uint32_t subAddress,
                                               uint8_t subAddressSize,
                                               uint8_t *txBuff,
                                               uint8_t txBuffSize)
    {
        status_t reVal;

        /* Send master blocking data to slave */
        reVal = LPI2C_MasterStart(base, deviceAddress, kLPI2C_Write);
        if (kStatus_Success == reVal) {
            while (LPI2C_MasterGetStatusFlags(base) & kLPI2C_MasterNackDetectFlag) {}

            reVal = LPI2C_MasterSend(base, &subAddress, subAddressSize);
            if (reVal != kStatus_Success) {
                return reVal;
            }

            reVal = LPI2C_MasterSend(base, txBuff, txBuffSize);
            if (reVal != kStatus_Success) {
                return reVal;
            }

            reVal = LPI2C_MasterStop(base);
            if (reVal != kStatus_Success) {
                return reVal;
            }
        }

        return reVal;
    }

    status_t RT1051DriverI2C::BOARD_LPI2C_Receive(LPI2C_Type *base,
                                                  uint8_t deviceAddress,
                                                  uint32_t subAddress,
                                                  uint8_t subAddressSize,
                                                  uint8_t *rxBuff,
                                                  uint8_t rxBuffSize)
    {
        status_t reVal;

        reVal = LPI2C_MasterStart(base, deviceAddress, kLPI2C_Write);
        if (kStatus_Success == reVal) {
            while (LPI2C_MasterGetStatusFlags(base) & kLPI2C_MasterNackDetectFlag) {}

            reVal = LPI2C_MasterSend(base, &subAddress, subAddressSize);
            if (reVal != kStatus_Success) {
                return reVal;
            }

            reVal = LPI2C_MasterRepeatedStart(base, deviceAddress, kLPI2C_Read);

            if (reVal != kStatus_Success) {
                return reVal;
            }

            reVal = LPI2C_MasterReceive(base, rxBuff, rxBuffSize);
            if (reVal != kStatus_Success) {
                return reVal;
            }

            reVal = LPI2C_MasterStop(base);
            if (reVal != kStatus_Success) {
                return reVal;
            }
        }
        return reVal;
    }

} // namespace drivers
