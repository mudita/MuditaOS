// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "PowerSaveModeInitRequest.hpp"

namespace app::manager
{
    PowerSaveModeInitRequest::PowerSaveModeInitRequest(const ApplicationName &senderName)
        : BaseMessage(MessageType::APMInitPowerSaveMode, senderName)
    {}
} // namespace app::manager
