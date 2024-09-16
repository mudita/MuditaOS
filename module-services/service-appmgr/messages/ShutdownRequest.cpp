// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "ShutdownRequest.hpp"

namespace app::manager
{
    ShutdownRequest::ShutdownRequest(const ApplicationName &senderName) : BaseMessage(MessageType::APMClose, senderName)
    {}
} // namespace app::manager
