// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory>
#include <devices/Device.hpp>
#include <magic_enum.hpp>

namespace drivers
{

    enum class USDHCInstances
    {
        USDHC_1,
        USDHC_2
    };

    class DriverUSDHC : public devices::Device
    {
      public:
        static std::shared_ptr<DriverUSDHC> Create(std::string name, const USDHCInstances inst);

        explicit DriverUSDHC(std::string name, USDHCInstances inst) : devices::Device(std::move(name)), instance(inst)
        {}

      protected:
        USDHCInstances instance;

      private:
        static std::weak_ptr<DriverUSDHC> singleton[magic_enum::enum_count<USDHCInstances>()];
    };

} // namespace drivers
