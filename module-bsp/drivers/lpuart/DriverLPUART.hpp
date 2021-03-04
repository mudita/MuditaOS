// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory>
#include <devices/Device.hpp>
#include <magic_enum.hpp>

namespace drivers
{

    enum class LPUARTInstances
    {
        LPUART_1,
        LPUART_2,
        LPUART_3,
        LPUART_4,
        LPUART_5,
        LPUART_6,
        LPUART_7,
        LPUART_8
    };

    class DriverLPUART : public devices::Device
    {
      public:
        static std::shared_ptr<DriverLPUART> Create(std::string name, const LPUARTInstances inst);

        explicit DriverLPUART(std::string name, LPUARTInstances inst) : devices::Device(std::move(name)), instance(inst)
        {}

      protected:
        LPUARTInstances instance;

      private:
        static std::weak_ptr<DriverLPUART> singleton[magic_enum::enum_count<LPUARTInstances>()];
    };

} // namespace drivers
