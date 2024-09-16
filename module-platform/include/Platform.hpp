// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

namespace platform
{
    class Platform
    {
      public:
        virtual ~Platform() = default;
        virtual void init() = 0;
        virtual void deinit() = 0;
    };
} // namespace platform
