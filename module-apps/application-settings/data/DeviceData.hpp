// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <gui/SwitchData.hpp>

namespace gui
{
    class DeviceData : public SwitchData
    {
      public:
        explicit DeviceData(std::vector<Devicei> devices) : devices(std::move(devices))
        {}
        auto getDevices() -> const std::vector<Devicei> &
        {
            return devices;
        }

      private:
        std::vector<Devicei> devices{};
    };
} // namespace gui
