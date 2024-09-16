// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

namespace sys
{
    class Watchdog
    {
      public:
        virtual void refresh() = 0;
    };
} // namespace sys
