// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include "CommandData.hpp"
#include "Device.hpp"

namespace bluetooth
{

    class DeviceData : public CommandData
    {
      public:
        explicit DeviceData(const Devicei &device);
        auto getData() -> DataVariant override;

      private:
        Devicei device;
    };

} // namespace bluetooth
