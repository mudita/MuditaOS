// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "StartAllowedMessage.hpp"

namespace app::manager
{
    StartAllowedMessage::StartAllowedMessage(StartupType startupType) : BaseMessage(), startupType(startupType)
    {}

    [[nodiscard]] StartupType StartAllowedMessage::getStartupType() const noexcept
    {
        return startupType;
    }

} // namespace app::manager
