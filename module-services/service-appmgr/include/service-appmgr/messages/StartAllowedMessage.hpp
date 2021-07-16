// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "BaseMessage.hpp"
#include <service-appmgr/StartupType.hpp>

namespace app::manager
{
    class CheckIfStartAllowedMessage : public BaseMessage
    {};
    class StartAllowedMessage : public BaseMessage
    {
      public:
        explicit StartAllowedMessage(StartupType startupType);

        [[nodiscard]] StartupType getStartupType() const noexcept;

      private:
        StartupType startupType;
    };
} // namespace app::manager
