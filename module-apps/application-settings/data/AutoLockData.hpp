// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <service-appmgr/messages/AutoLockRequests.hpp>

namespace gui
{
    class AutoLockData : public gui::SwitchData
    {
        std::chrono::seconds value;

      public:
        explicit AutoLockData(std::chrono::seconds value) : value{value}
        {}

        [[nodiscard]] auto getValue() const noexcept
        {
            return value;
        }
    };

} // namespace gui
