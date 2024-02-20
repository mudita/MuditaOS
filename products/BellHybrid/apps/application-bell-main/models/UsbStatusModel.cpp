// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "UsbStatusModel.hpp"

namespace app
{
    bool UsbStatusModel::isUsbConnected(const Store::Battery::State &state) const
    {
        return ((usbConnected == UsbStatus::Connected) && (state == Store::Battery::State::PluggedNotCharging));
    }

    void UsbStatusModel::setUsbStatus(UsbStatus status)
    {
        usbConnected = status;
    }
} // namespace app
