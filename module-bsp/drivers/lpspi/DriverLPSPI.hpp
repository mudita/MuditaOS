// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory>
#include <devices/Device.hpp>
#include <magic_enum.hpp>

namespace drivers
{

    enum class LPSPIInstances
    {
        LPSPI_1,
        LPSPI_2,
        LPSPI_3,
        LPSPI_4
    };

    class DriverLPSPI : public devices::Device
    {
      public:
        static std::shared_ptr<DriverLPSPI> Create(std::string name, const LPSPIInstances inst);

        explicit DriverLPSPI(std::string name, LPSPIInstances inst) : devices::Device(std::move(name)), instance(inst)
        {}

      protected:
        LPSPIInstances instance;

      private:
        static std::weak_ptr<DriverLPSPI> singleton[magic_enum::enum_count<LPSPIInstances>()];
    };

} // namespace drivers
