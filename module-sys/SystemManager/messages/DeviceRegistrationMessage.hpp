// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "devices/Device.hpp"

namespace sys
{
    class DeviceRegistrationMessage : public sys::DataMessage
    {
      public:
        explicit DeviceRegistrationMessage(std::shared_ptr<devices::Device> devicePtr)
            : sys::DataMessage(MessageType::DeviceRegistration), device(devicePtr)
        {}

        [[nodiscard]] auto getDevice() const noexcept -> std::shared_ptr<devices::Device>
        {
            return device;
        };

      private:
        std::shared_ptr<devices::Device> device;
    };
} // namespace sys
