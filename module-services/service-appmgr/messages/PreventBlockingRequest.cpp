// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PreventBlockingRequest.hpp"

namespace app::manager
{
    PreventBlockingRequest::PreventBlockingRequest(const ApplicationName &senderName)
        : BaseMessage(MessageType::APMPreventBlocking, senderName)
    {}
} // namespace app::manager
