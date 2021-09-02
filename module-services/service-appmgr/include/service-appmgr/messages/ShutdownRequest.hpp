// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "BaseMessage.hpp"

namespace app::manager
{
    class ShutdownRequest : public BaseMessage
    {
      public:
        explicit ShutdownRequest(const ApplicationName &senderName);
    };
} // namespace app::manager
