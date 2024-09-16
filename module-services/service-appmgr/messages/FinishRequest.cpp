// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "FinishRequest.hpp"

namespace app::manager
{
    FinishRequest::FinishRequest(const ApplicationName &senderName) : BaseMessage{MessageType::APMFinish, senderName}
    {}
} // namespace app::manager
