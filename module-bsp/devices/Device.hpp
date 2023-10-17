// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>

namespace devices
{
    class Device
    {
      public:
        explicit Device(std::string name);
        virtual ~Device() = default;

        virtual void Enable()  = 0;
        virtual void Disable() = 0;
        [[nodiscard]] bool IsEnabled() const noexcept;
        [[nodiscard]] auto GetName() const noexcept -> std::string;

      protected:
        bool isEnabled{false};

        const std::string name;
    };

} // namespace devices
