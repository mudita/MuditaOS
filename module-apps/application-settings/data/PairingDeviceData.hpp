// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

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
