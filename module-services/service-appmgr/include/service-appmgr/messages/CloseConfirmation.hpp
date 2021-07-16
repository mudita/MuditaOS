// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "BaseMessage.hpp"

namespace app::manager
{
    class CloseConfirmation : public BaseMessage
    {
      public:
        explicit CloseConfirmation(const ApplicationName &sender);
    };
} // namespace app::manager
