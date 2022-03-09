// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory>
#include <devices/Device.hpp>

namespace drivers
{
    namespace name
    {
        constexpr auto ExternalRAM = "ExternalRAM";
    }

    class DriverSEMC : public devices::Device
    {
      public:
        static std::shared_ptr<DriverSEMC> Create(std::string name);

        explicit DriverSEMC(std::string name) : devices::Device(std::move(name))
        {}

        virtual void SwitchToPLL2ClockSource()       = 0;
        virtual void SwitchToPeripheralClockSource() = 0;

      private:
        static std::weak_ptr<DriverSEMC> singleton;
    };

} // namespace drivers
