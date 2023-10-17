// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Device.hpp"

namespace devices
{

    Device::Device(std::string name) : name(std::move(name))
    {}

    bool Device::IsEnabled() const noexcept
    {
        return isEnabled;
    }

    [[nodiscard]] auto Device::GetName() const noexcept -> std::string
    {
        return name;
    }

} // namespace devices
