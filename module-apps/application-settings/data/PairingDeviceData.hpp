// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <Device.hpp>

namespace gui
{

    class PairingDeviceData : public SwitchData
    {
      public:
        explicit PairingDeviceData(const Devicei &pairingDevice) : pairingDevice(pairingDevice)
        {}
        [[nodiscard]] auto getPairingDevice() const noexcept -> const Devicei &
        {
            return pairingDevice;
        }

      private:
        const Devicei pairingDevice;
    };
} // namespace gui
