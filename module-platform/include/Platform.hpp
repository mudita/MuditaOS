// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

namespace platform
{
    class Platform
    {
      public:
        virtual ~Platform() = default;
        virtual void init() = 0;
        virtual void initCommonUserFolders();
        virtual void deinit() = 0;
    };
} // namespace platform
