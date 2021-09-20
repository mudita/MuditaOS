// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

namespace sys
{
    class Watchdog
    {
      public:
        virtual void refresh() = 0;
    };
} // namespace sys
