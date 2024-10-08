// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "UsbStatusModel.hpp"

namespace app
{
    bool UsbStatusModel::isUsbConnected(const Store::Battery::State &state) const
    {
        return (isUsbConfigured() && (state == Store::Battery::State::PluggedNotCharging));
    }

    bool UsbStatusModel::isUsbConfigured() const
    {
        return Store::Usb::get().status == Store::Usb::Status::Configured;
    }
} // namespace app
