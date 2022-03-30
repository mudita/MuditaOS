// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DeviceData.hpp"

namespace bluetooth
{
    DeviceData::DeviceData(const Devicei &device) : device(device)
    {}

    auto DeviceData::getData() -> DataVariant
    {
        return device;
    }

} // namespace bluetooth
