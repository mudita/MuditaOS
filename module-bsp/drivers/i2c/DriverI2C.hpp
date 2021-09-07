// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#ifndef PUREPHONE_DRIVERI2C_HPP
#define PUREPHONE_DRIVERI2C_HPP

#include <memory>
#include <sys/types.h>

namespace drivers
{

    enum class I2CInstances
    {
        I2C1,
        I2C2,
        I2C3,
        I2C4,
        COUNT
    };

    struct DriverI2CParams
    {
        uint32_t baudrate;
        // TODO:M.P add slave conf
    };

    struct I2CAddress
    {
        uint32_t deviceAddress;
        uint32_t subAddress;
        size_t subAddressSize;
    };

    class DriverI2C
    {
      public:
        static std::shared_ptr<DriverI2C> Create(const I2CInstances inst, const DriverI2CParams &params);

        DriverI2C(const DriverI2CParams &params, const I2CInstances inst) : instance(inst), parameters(params)
        {}

        virtual ~DriverI2C()
        {}

        virtual ssize_t Write(const I2CAddress &addr, const uint8_t *txBuff, const size_t size) = 0;

        virtual ssize_t Read(const I2CAddress &addr, uint8_t *rxBuff, const size_t size) = 0;

        virtual ssize_t Modify(const I2CAddress &addr, const uint32_t mask, bool setClr, const size_t size) = 0;

      protected:
        I2CInstances instance;
        const DriverI2CParams parameters;

      private:
        static std::weak_ptr<DriverI2C> singleton[static_cast<uint32_t>(I2CInstances ::COUNT)];
    };

} // namespace drivers

#endif // PUREPHONE_DRIVERI2C_HPP
