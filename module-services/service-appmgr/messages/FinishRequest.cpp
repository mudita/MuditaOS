// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "FinishRequest.hpp"

namespace app::manager
{
    FinishRequest::FinishRequest(const ApplicationName &senderName) : BaseMessage{MessageType::APMFinish, senderName}
    {}
} // namespace app::manager
