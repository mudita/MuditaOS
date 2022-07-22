// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#ifndef PUREPHONE_RT1051DRIVERI2C_HPP
#define PUREPHONE_RT1051DRIVERI2C_HPP

#include "drivers/i2c/DriverI2C.hpp"

#include <fsl_common.h>
#include <fsl_lpi2c.h>
#include "board/clock_config.h"

#include "mutex.hpp"

namespace drivers
{

    class RT1051DriverI2C : public DriverI2C
    {
      public:
        RT1051DriverI2C(const I2CInstances inst, const DriverI2CParams &params);

        ~RT1051DriverI2C();

        ssize_t Write(const I2CAddress &addr, const uint8_t *txBuff, const size_t size) override final;

        ssize_t Read(const I2CAddress &addr, uint8_t *rxBuff, const size_t size) override final;

        ssize_t Modify(const I2CAddress &addr, const uint32_t mask, bool setClr, const size_t size) override final;

      private:
        status_t BOARD_LPI2C_Receive(LPI2C_Type *base,
                                     uint8_t deviceAddress,
                                     uint32_t subAddress,
                                     uint8_t subAddressSize,
                                     uint8_t *rxBuff,
                                     uint8_t rxBuffSize);

        status_t BOARD_LPI2C_Send(LPI2C_Type *base,
                                  uint8_t deviceAddress,
                                  uint32_t subAddress,
                                  uint8_t subAddressSize,
                                  uint8_t *txBuff,
                                  uint8_t txBuffSize);

        LPI2C_Type *base;
        cpp_freertos::MutexStandard mutex;
    };

} // namespace drivers

#endif // PUREPHONE_RT1051DRIVERI2C_HPP
