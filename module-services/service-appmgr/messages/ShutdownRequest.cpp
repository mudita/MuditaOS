// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <service-appmgr/messages/ShutdownRequest.hpp>

namespace app::manager
{
    ShutdownRequest::ShutdownRequest(const ApplicationName &senderName) : BaseMessage(MessageType::APMClose, senderName)
    {}
} // namespace app::manager
