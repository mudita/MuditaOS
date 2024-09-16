// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "BaseMessage.hpp"

namespace app::manager
{
    class PreventBlockingRequest : public BaseMessage
    {
      public:
        explicit PreventBlockingRequest(const ApplicationName &senderName);
    };
} // namespace app::manager
