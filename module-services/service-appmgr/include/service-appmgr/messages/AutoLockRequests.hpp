// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "BaseMessage.hpp"

namespace app::manager
{
    class SetAutoLockTimeoutRequest : public BaseMessage
    {
        std::chrono::seconds value;

      public:
        explicit SetAutoLockTimeoutRequest(std::chrono::seconds value) : value{value}
        {}

        [[nodiscard]] auto getValue() const noexcept
        {
            return value;
        }
    };

} // namespace app::manager
